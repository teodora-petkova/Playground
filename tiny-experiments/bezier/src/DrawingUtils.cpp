#include "DrawingUtils.h"

void DrawingUtils::drawPoint(Point p, std::tuple<float, float, float> rgbColor, double size)
{
    float r, g, b;
    std::tie(r, g, b) = rgbColor;
    glPointSize(size);
    glColor3f(r, g, b);
    glBegin(GL_POINTS);
    glVertex2f(p.x, p.y);
    glEnd();
}

void DrawingUtils::drawLine(Point point1, Point point2, std::tuple<float, float, float> rgbColor)
{
    float r, g, b;
    std::tie(r, g, b) = rgbColor;
    glColor3f(r, g, b);
    glBegin(GL_LINE_STRIP);
    glVertex2f(point1.x, point1.y);
    glVertex2f(point2.x, point2.y);
    glEnd();
}

void DrawingUtils::drawCurve(std::vector<Point> points, std::tuple<float, float, float> rgbColor)
{
    float r, g, b;
    std::tie(r, g, b) = rgbColor;
    glColor3f(r, g, b);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < points.size(); i++)
    {
        glVertex2f(points[i].x, points[i].y);
    }
    glEnd();
}