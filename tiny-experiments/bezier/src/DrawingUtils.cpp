#include "DrawingUtils.h"

void DrawingUtils::drawPoint(Point p, double r, double g, double b, double size)
{
    glPointSize(size);
    glColor3f(r, g, b);
    glBegin(GL_POINTS);
    glVertex2f(p.x, p.y);
    glEnd();
}

void DrawingUtils::drawLine(Point point1, Point point2, double r, double g, double b)
{
    glColor3f(r, g, b);
    glBegin(GL_LINE_STRIP);
    glVertex2f(point1.x, point1.y);
    glVertex2f(point2.x, point2.y);
    glEnd();
}

void DrawingUtils::drawCurve(std::vector<Point> points, double r, double g, double b)
{
    glColor3f(r, g, b);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < points.size(); i++)
    {
        glVertex2f(points[i].x, points[i].y);
    }
    glEnd();
}