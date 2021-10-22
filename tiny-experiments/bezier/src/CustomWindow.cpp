#pragma once

#include "GLFW\glfw3.h"

#include <math.h>

#include "Point.h"
#include "Scroller.h"
#include "BezierCurve.h"
#include "DrawingUtils.h"

class CustomWindow
{
public:
    CustomWindow(int sceneWidth, int sceneHeight, const char *title)
    {
        this->window = glfwCreateWindow(sceneWidth, sceneHeight, title, NULL, NULL);

        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        this->scroller = Scroller();

        glfwMakeContextCurrent(this->window);

        glfwSetWindowUserPointer(this->window, this);
        glfwSetMouseButtonCallback(this->window, onMouse);
        glfwSetKeyCallback(window, onKey);
    }

    ~CustomWindow()
    {
        glfwDestroyWindow(this->window);
    }

    bool isCreated()
    {
        return this->window != NULL;
    }

    bool shouldClose()
    {
        return glfwWindowShouldClose(this->window);
    }

    void init(int sceneWidth, int16_t sceneHeight)
    {
        clearFrameBuffer();

        setAntialiasing();

        setUpMatrix(this->sceneWidth, this->sceneHeight);
    }

    void swapBuffers()
    {
        glfwSwapBuffers(this->window);
    }

    void onMouse(int button, int action, int mods)
    {
        Point cursorPoint = getCursorPoint();

        leftButtonDown = isMouseLeftButtonDown(button, action);

        if (isPointInScene(cursorPoint))
        {
            if (button == GLFW_MOUSE_BUTTON_LEFT &&
                action == GLFW_PRESS)
            {
                double distanceToMovePoint = cursorPoint.getDistanceTo(scroller.getPoint());

                if (!scroller.isPointInScrollerSpace(cursorPoint))
                {
                    // adding control points or moving the ones too close to the cursor
                    if (!this->curve.empty())
                    {
                        int minIndex;
                        double minDistance;
                        std::tie(minIndex, minDistance) = this->curve.getClosestControlPointIndexAndDistance(cursorPoint);

                        if (minDistance <= minPixelsToGrabAPoint)
                        {
                            curve.setControlPoint(minIndex, cursorPoint);
                        }
                        else
                        {
                            curve.addControlPoint(cursorPoint);
                        }
                    }
                    else
                    {
                        curve.addControlPoint(cursorPoint);
                    }
                }
            }
            else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
            {
                this->curve.deleteLastControlPoint();
            }
        }
    }

    void onKey(int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        if (key == GLFW_KEY_ESCAPE)
        {
            return;
        }
        if (key == GLFW_KEY_DELETE)
        {
            this->curve.deleteAllControlPoints();
        }
        if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
             glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) &&
            glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        {
            this->curve.deleteLastControlPoint();
        }
    }

    void renderScene()
    {
        int w = sceneWidth * 0.5;
        int h = sceneHeight * 0.5;

        glEnable(GL_SCISSOR_TEST);

        // box (0, 0)
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1, 1);
        glViewport(0, w, w, h);
        glScissor(0, w, w, h);
        glClearColor(0.98, 0.88, 0.88, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        renderMainView();

        // box (0, 1)
        glLoadIdentity();
        glOrtho(-0.1, 1.1, -0.1, 1.1, -1, 1);
        glViewport(w, h, w, h);
        glScissor(w, h, w, h);
        glClearColor(0.8, 0.9, 0.9, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        renderBernsteinView();

        // box (1, 0)
        glLoadIdentity();
        glOrtho(-w * 0.6, w * 0.6, h * 0.6, -h * 0.6, -1, 1);
        glViewport(0, 0, w, h);
        glScissor(0, 0, w, h);
        glClearColor(0.9, 0.8, 0.8, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        renderHodographView();

        // box (1, 1)
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1, 1);
        glViewport(w, 0, w, h);
        glScissor(w, 0, w, h);
        glClearColor(0.9, 0.9, 0.8, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        renderSubdivisionView();

        glDisable(GL_SCISSOR_TEST);
    }

private:
    GLFWwindow *window;
    int sceneWidth;
    int sceneHeight;

    Scroller scroller;

    BezierCurve curve;

    double minPixelsToGrabAPoint = 8;
    bool leftButtonDown = false;

    void renderMainView()
    {
        // main view
        if (leftButtonDown)
        {
            onMouseDragging();
        }

        this->scroller.initDraw();

        this->curve.drawCurve();

        this->curve.drawPolar(this->scroller.t());
    }

    void renderBernsteinView()
    {
        // render axis - Ox and Oy
        DrawingUtils::drawLine(Point(0, 0), Point(1, 0), {1, 0, 1});
        DrawingUtils::drawLine(Point(0, 0), Point(0, 1), {1, 0, 1});

        this->curve.drawBernsteins();
    }

    void renderHodographView()
    {
        Point center = Point(0, 0);

        this->curve.drawHodograph(center);
    }

    void renderSubdivisionView()
    {
        this->curve.drawSubdividedCurve(this->scroller.t());
    }

    bool isPointInScene(Point p) const
    {
        return p.x >= 0 && p.y >= 0 && p.x < this->sceneWidth * 0.5 && p.y < this->sceneHeight * 0.5;
    }

    void onMouseDragging()
    {
        Point cursorPoint = getCursorPoint();

        if (scroller.isPointInScrollerSpace(cursorPoint))
        {
            scroller.updatePointOnScroller(cursorPoint);
        }
        else
        {
            if (isPointInScene(cursorPoint) && !this->curve.empty())
            {
                int minIndex;
                double minDistance;
                std::tie(minIndex, minDistance) = this->curve.getClosestControlPointIndexAndDistance(cursorPoint);

                //if (minDistance <= minPixelsToGrabAPoint)
                {
                    this->curve.setControlPoint(minIndex, cursorPoint);
                }
            }
        }
    }

    Point getCursorPoint()
    {
        double mx, my;
        glfwGetCursorPos(this->window, &mx, &my);
        return Point(mx, my);
    }

    bool isMouseLeftButtonDown(int button, int action) const
    {
        bool leftButtonDown = false;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (GLFW_PRESS == action)
                leftButtonDown = true;
            else if (GLFW_RELEASE == action)
                leftButtonDown = false;
        }
        return leftButtonDown;
    }

    //////////////// EVENTS ////////////////////////
    static void onMouse(GLFWwindow *window, int button, int action, int mods)
    {
        CustomWindow *obj = (CustomWindow *)glfwGetWindowUserPointer(window);
        obj->onMouse(button, action, mods);
    }

    static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        CustomWindow *obj = (CustomWindow *)glfwGetWindowUserPointer(window);
        obj->onKey(key, scancode, action, mods);
    }

    ////////////////////////////////////////////////

    ///////////////////// INIT GLFW ////////////////
    void clearFrameBuffer()
    {
        glClearColor(1.0, 1.0, 1.0, 0.0); // set white background color
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void setAntialiasing()
    {
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
        glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
        glLineWidth(1.5);
    }

    void setUpMatrix(int sceneWidth, int sceneHeight)
    {
        glMatrixMode(GL_PROJECTION);
    }
    ////////////////////////////////////////////////
};