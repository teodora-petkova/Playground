#include "drawalgorithms.h"

std::tuple<int, int> getMinMax(int x1, int x2)
{
	return (x1 < x2) ? std::make_tuple(x1, x2) : std::make_tuple(x2, x1);
}

/*
* Naive approach - just following the line equation
*/
void DrawAlgorithms::drawNaiveLine(const std::function<int(int, int)> &drawPoint,
								   int x1, int y1, int x2, int y2)
{
	if (x1 == x2)
	{
		int miny = 0;
		int maxy = 0;
		std::tie(miny, maxy) = getMinMax(y1, y2);
		for (int y = miny; y <= maxy; y++)
		{
			drawPoint(x1, y);
		}
	}
	else
	{
		int minx = 0;
		int maxx = 0;
		std::tie(minx, maxx) = getMinMax(x1, x2);
		float slope = (y2 - y1) / float(x2 - x1);
		for (int x = minx; x <= maxx; x++)
		{
			int y = slope * (x - x1) + y1;
			drawPoint(x, y);
		}
	}
}

/* Bresenham's algorithm - short derivation for 1st Octant - 0 <= m <=1 and a positive slope for m
* References: 
*   - https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html
*   - http://members.chello.at/~easyfilter/Bresenham.pdf
*   - http://members.chello.at/easyfilter/bresenham.html
*   - Wikipedia: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
*	- https://blog.mbedded.ninja/programming/algorithms-and-data-structures/bresenhams-line-algorithm/
* m = dy/dx
* y = mx + b
We will choose to plot (x+1,y), if the difference between this new value and y is less than 0.5.
y + e + m < y + 0.5
e + m < 0.5
otherwise we will plot (x+1,y+1).
if choose (x+1, y)   -> e = e + m;     y = y
if choose (x+1, y+1) -> e = e + m - 1; y = y+1
trying to remove floating points in m and 0.5 (faster and useful in no FPUs in CPU or in GPU)
e + dy/dx < 0.5 / .dx
e.dx + dy < 0.5.dx / .2
2dx.e + 2.dy < dx (eq.1.0.)
Substitute e' = dx.e =>  2.e'+ 2.dy < dx (eq.1.1.)
And the equation for choosing point:
if choose (x+1, y)   -> dx.e = dx.e + dy      => e' = e' + dy (eq.2.1.)
if choose (x+1, y+1) -> dx.e = dx.e + dy - dx => e' = e' + dy - dx (eq.2.2.)
*/
void DrawAlgorithms::drawBresenhamLine1Octant(const std::function<int(int, int)> &drawPoint,
											  int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int y = y1;
	int e = 0;

	for (int x = x1; x <= x2; x += 1)
	{
		drawPoint(x, y);
		e = e + dy;
		if (2 * e >= dx) // eq.1.1. (for the case when we choose (x+1, y+1))
		{
			y = y + 1;
			e = e - dx;
		}
	}
}

/* Bresenham's algorithm - short derivation for a negative slope
We will choose to plot (x+1,y), if the difference between this new value and y is less than -0.5 as y+1 < y
y + e + m < y - 0.5 => y - (y+e+m) < 0.5
e + m > -0.5
otherwise we will plot (x+1,y+1).
if choose (x+1, y)   -> e = (y+e+m) - y = e + m;        y = y
if choose (x+1, y+1) -> e = (y+e+m) - (y-1)= e + m + 1; y = y-1
trying to remove floating points in m and 0.5
2.dx.e + 2dy > -dx
Substitute e' = dx.e =>  2.(e'+ dy) > -dx (eq.1.1.)
And the equation for choosing point:
if choose (x+1, y)   -> dx.e = dx.e + dy      => e' = e' + dy (eq.2.1.)
if choose (x+1, y+1) -> dx.e = dx.e + dy + dx => e' = e' + dy + dx (eq.2.2.)
*/
void DrawAlgorithms::drawBresenhamLineNegativeSlope(const std::function<int(int, int)> &drawPoint,
													int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int y = y1;
	int e = 0;

	for (int x = x1; x <= x2; x += 1)
	{
		drawPoint(x, y);
		e = e + dy;
		if (2 * e <= -dx)
		{
			y = y - 1;
			e = e + dx;
		}
	}
}

void DrawAlgorithms::drawLineWithSlopeY(const std::function<int(int, int)> &drawPoint,
										int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = abs(y2 - y1);
	int y = y1;
	int e = 0;
	int sy = y2 - y1 < 0 ? -1 : 1;

	for (int x = x1; x <= x2; x += 1)
	{
		drawPoint(x, y);
		e = e + dy;
		if (2 * e >= dx)
		{
			y = y + sy;
			e = e - dx;
		}
	}
}

void DrawAlgorithms::drawLineWithSlopeX(const std::function<int(int, int)> &drawPoint,
										int x1, int y1, int x2, int y2)
{
	int dx = abs(x2 - x1);
	int dy = y2 - y1;
	int x = x1;
	int e = 0;
	int sx = x2 - x1 < 0 ? -1 : 1;

	for (int y = y1; y <= y2; y += 1)
	{
		drawPoint(x, y);
		e = e + dx;
		if (2 * e >= dy)
		{
			x = x + sx;
			e = e - dy;
		}
	}
}

/* Bresenham's aglorithm - generalised - all kinds of lines */
void DrawAlgorithms::drawBresenhamLine1(const std::function<int(int, int)> &drawPoint,
										int x1, int y1, int x2, int y2)
{
	// -45 <= alpha <= 45 i.e. 0 < m < 1
	if (abs(y2 - y1) < abs(x2 - x1))
	{
		if (x1 < x2)
		{
			// x1 < x2 && y1 < y2
			drawLineWithSlopeY(drawPoint, x1, y1, x2, y2);
		}
		else
		{
			// x2 < x1 && y1 < y2 => swap
			drawLineWithSlopeY(drawPoint, x2, y2, x1, y1);
		}
	}
	// 45 < alpha < 135 i.e. 1 < m < inf
	else
	{
		if (y1 < y2)
		{
			// x1 < x2 && y1 < y2
			drawLineWithSlopeX(drawPoint, x1, y1, x2, y2);
		}
		else
		{
			// x1 < x2 && y1 > y2 => swap
			drawLineWithSlopeX(drawPoint, x2, y2, x1, y1);
		}
	}
}

/* Bresenham's aglorithm - generalised - all kinds of lines */
void DrawAlgorithms::drawBresenhamLine2(const std::function<int(int, int)> &drawPoint,
										int x1, int y1, int x2, int y2, int pixelSize)
{
	if (x1 == x2)
	{
		int miny = 0;
		int maxy = 0;
		std::tie(miny, maxy) = getMinMax(y1, y2);
		for (int y = miny; y <= maxy; y += pixelSize)
		{
			drawPoint(x1, y);
		}
	}
	else if (y1 == y2)
	{
		int minx = 0;
		int maxx = 0;
		std::tie(minx, maxx) = getMinMax(x1, x2);
		for (int x = minx; x <= maxx; x += pixelSize)
		{
			drawPoint(x, y1);
		}
	}
	else
	{
		int dy = -abs(y2 - y1);
		int dx = abs(x2 - x1);

		int sx = x1 < x2 ? pixelSize : -pixelSize;
		int sy = y1 < y2 ? pixelSize : -pixelSize;

		int e = dx + dy;
		int x = x1;
		int y = y1;

		while (true)
		{
			drawPoint(x, y);

			if (x == x2 || y == y2)
				break;

			if (2 * e >= dy)
			{ // step x
				e += dy;
				x += sx;
			}
			if (2 * e <= dx)
			{ // step y
				e += dx;
				y += sy;
			}
		}
	}
}