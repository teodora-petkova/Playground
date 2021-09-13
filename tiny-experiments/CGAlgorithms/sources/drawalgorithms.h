#pragma once;

#include <functional>

namespace DrawAlgorithms
{
    // Lines
    void drawNaiveLine(const std::function<int(int, int)> &drawPoint,
                       int x1, int y1, int x2, int y2);
    void drawBresenhamLine1Octant(const std::function<int(int, int)> &drawPoint,
                                  int x1, int y1, int x2, int y2);
    void drawBresenhamLineNegativeSlope(const std::function<int(int, int)> &drawPoint,
                                        int x1, int y1, int x2, int y2);
    void drawLineWithSlopeY(const std::function<int(int, int)> &drawPoint,
                            int x1, int y1, int x2, int y2);
    void drawLineWithSlopeX(const std::function<int(int, int)> &drawPoint,
                            int x1, int y1, int x2, int y2);
    void drawBresenhamLine1(const std::function<int(int, int)> &drawPoint,
                            int x1, int y1, int x2, int y2);
    void drawBresenhamLine2(const std::function<int(int, int)> &drawPoint,
                            int x1, int y1, int x2, int y2, int pixelSize = 1);
};