#pragma once

struct Point
{
    double x;
    double y;
    Point(double nx = 0, double ny = 0)
    {
        x = nx;
        y = ny;
    }

    double getDistanceTo(Point p2) const
    {
        Point p1 = *this;
        return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    }
};