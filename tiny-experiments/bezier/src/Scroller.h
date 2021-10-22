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
        this->_t = (scrollerPoint.x - start.x) / (end.x - start.x);
    }

    void initDraw()
    {
        DrawingUtils::drawLine(start, end, {0.3f, 0.3f, 0.3f});

        this->_t = (scrollerPoint.x - start.x) / (end.x - start.x);

        DrawingUtils::drawPoint(scrollerPoint, {0.3f, 0.3f, 0.3f}, 7.0f);
    }

    double t() const
    {
        return _t;
    }

private:
    int width = 120;
    int height = 30;

    Point start = Point(15, 15);
    Point end = Point(115, 15);
    Point scrollerPoint = Point(70, 15);

    double _t = 0.5;
};