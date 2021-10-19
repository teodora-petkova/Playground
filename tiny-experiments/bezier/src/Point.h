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

    double getDistanceTo(const Point &p2) const
    {
        Point p1 = *this;
        return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    }

    Point operator*(double d)
    {
        Point p = *this;
        return Point(p.x * d, p.y * d);
    }

    Point operator+(Point p2)
    {
        Point p1 = *this;
        return Point(p1.x + p2.x, p1.y + p2.y);
    }
};