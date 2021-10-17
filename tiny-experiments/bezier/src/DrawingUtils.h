
#pragma once
#include <vector>
#include "GLFW\glfw3.h"
#include "Point.h"

namespace DrawingUtils
{
    void drawPoint(Point p, double r, double g, double b, double size = 0.7);

    void drawLine(Point point1, Point point2, double r, double g, double b);

    void drawCurve(std::vector<Point> points, double r, double g, double b);
}