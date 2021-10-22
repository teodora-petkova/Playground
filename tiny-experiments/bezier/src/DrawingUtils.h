
#pragma once
#include <vector>
#include <tuple>
#include "GLFW\glfw3.h"
#include "Point.h"

namespace DrawingUtils
{
    void drawPoint(Point p, std::tuple<float, float, float> rgbColor, double size = 0.7);

    void drawLine(Point point1, Point point2, std::tuple<float, float, float> rgbColor);

    void drawCurve(std::vector<Point> points, std::tuple<float, float, float> rgbColor);
}