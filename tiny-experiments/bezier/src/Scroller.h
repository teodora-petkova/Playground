#pragma once

#include "Point.h"
#include "DrawingUtils.h"

class Scroller
{
public:
    Scroller()
    {
    }

    Point getPoint() { return scrollerPoint; }

    bool isPointInScrollerSpace(Point p) const
    {
        return p.x <= width && p.y <= height;
    }

    void updatePointOnScroller(Point p)
    {
        int x = (p.x <= start.x) ? start.x
                : (p.x >= end.x) ? end.x
                                 : p.x;
        int y = (p.y <= start.y) ? start.y
                : (p.y >= end.y) ? end.y
                                 : p.y;

        this->scrollerPoint = Point(x, y);
        this->_t = (end.x - x) / (end.x - start.x);
    }

    void draw()
    {
        DrawingUtils::drawLine(start, end, 0.3f, 0.3f, 0.3f);

        _t = _t * (end.x - start.x) / (end.x - start.x);

        DrawingUtils::drawPoint(scrollerPoint, 0.3f, 0.3f, 0.3f, 7.0f);
    }

private:
    int width = 200;
    int height = 100;

    Point start = Point(20, 50);
    Point end = Point(120, 50);
    Point scrollerPoint = Point(70, 50);

    int _t = 0.5;
};