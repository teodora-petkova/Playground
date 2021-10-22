#pragma once

#include <math.h>
#include <vector>
#include <tuple>

#include "Point.h"
#include "BinomialCoefficients.h"
#include "DrawingUtils.h"

#define MAX_CONTROL_POINTS_COUNT 101
#define CURVE_POINTS_COUNT 101

class BezierCurve
{
public:
    BezierCurve()
    {
        this->curvePoints = std::vector<Point>(CURVE_POINTS_COUNT);

        for (int i = 0; i < this->curvePoints.size(); i++)
        {
            this->curvePoints[i] = Point(0, 0);
        }
    }

    BezierCurve(int count)
        : BezierCurve()
    {
        this->controlPoints = std::vector<Point>(count);

        for (int i = 0; i < this->controlPoints.size(); i++)
        {
            this->controlPoints[i] = Point(0, 0);
        }
    }

    ~BezierCurve()
    {
    }

    std::tuple<int, double> getClosestControlPointIndexAndDistance(Point p)
    {
        double minDistance = INFINITY;
        int minIndex = 0;
        for (int i = 0; i < this->controlPoints.size(); i++)
        {
            double distance = p.getDistanceTo(this->controlPoints[i]);
            if (distance < minDistance)
            {
                minDistance = distance;
                minIndex = i;
            }
        }
        return {minIndex, minDistance};
    }

    void setControlPoint(int i, Point p)
    {
        if (i >= 0 && i < this->controlPoints.size())
        {
            this->controlPoints[i] = p;
        }
    }

    void addControlPoint(Point p)
    {
        if (this->controlPoints.size() + 1 < MAX_CONTROL_POINTS_COUNT)
        {
            controlPoints.push_back(p);
        }
    }

    void deleteLastControlPoint()
    {
        if (controlPoints.size() > 0)
        {
            controlPoints.pop_back();
        }
    }

    void deleteAllControlPoints()
    {
        while (controlPoints.size() > 0)
        {
            controlPoints.pop_back();
        }
    }

    bool empty() const
    {
        return this->controlPoints.empty();
    }

    void drawCurve()
    {
        drawControlPoints(this->controlPoints, colorControlPoints);

        this->curvePoints = getCurve(this->controlPoints);

        DrawingUtils::drawCurve(this->curvePoints, colorCurve);

        //points.clear();
        //points.shrink_to_fit();
    }

    void drawBernsteins()
    {
        int n = this->controlPoints.size() - 1;
        int size = CURVE_POINTS_COUNT;

        std::vector<std::vector<Point>> bernsteins = std::vector<std::vector<Point>>(n + 1);
        auto coefficients = BinomialCoefficients::BinomialCoefficients(n + 1);

        for (int k = 0; k < size; k++)
        {
            double t = (double)k / (size - 1);

            for (int i = 0; i <= n; i++)
            {
                double bernstein = getBernstein(i, n, t, coefficients);

                bernsteins[i].push_back(Point(t, bernstein));
            }
        }
        for (auto bernstein : bernsteins)
        {
            DrawingUtils::drawCurve(bernstein, colorBernstein);
        }
    }

    void drawPolar(double t)
    {
        int n = this->controlPoints.size() - 1;
        if (n >= 1)
        {
            auto middlePoints = std::vector<Point>(n);
            for (int i = 0; i < n; i++)
            {
                middlePoints[i] = this->controlPoints[i] * (1 - t) + this->controlPoints[i + 1] * t;
            }

            auto polar = getCurve(middlePoints);

            // the point where the polar and bezier curve match t = t1
            int k = t * (CURVE_POINTS_COUNT - 1);
            DrawingUtils::drawPoint(polar[k], colorPolar, 7.0f);

            // tangents
            for (int k = 0; k < CURVE_POINTS_COUNT; k += 5)
            {
                DrawingUtils::drawLine(this->curvePoints[k], polar[k], colorPolar);
            }

            DrawingUtils::drawCurve(polar, colorPolar);
        }
    }

    void drawHodograph(Point center)
    {
        int n = this->controlPoints.size();

        if (n >= 2)
        {
            // the starting point for drawing the vectors!
            Point p1 = center;

            // actually end points for the hodograph vectors
            auto hodographControlPoints = std::vector<Point>(n - 1);

            for (int j = 0; j < n - 1; j++)
            {
                Point pj1 = this->controlPoints[j + 1];
                Point pj = this->controlPoints[j];
                Point p2 = Point((pj1.x - pj.x), (pj1.y - pj.y));

                hodographControlPoints[j] = Point(p1.x + p2.x, p1.y + p2.y);
            }

            for (auto p2 : hodographControlPoints)
            {
                DrawingUtils::drawLine(p1, p2, colorVectors);
            }
            drawControlPoints(hodographControlPoints, colorControlPoints);

            std::vector<Point> points = getCurve(hodographControlPoints);

            DrawingUtils::drawCurve(points, colorCurve);
        }
    }

    void drawSubdividedCurve(double t = 0.5)
    {
        int n = this->controlPoints.size();
        if (n > 0)
        {
            // de Casteljau
            auto middlePoints = std::vector<std::vector<Point>>(n);
            auto pointsCurve0c = std::vector<Point>(n);
            auto pointsCurvec1 = std::vector<Point>(n);

            for (int r = 0; r < n; r++)
            {
                middlePoints[0].push_back(this->controlPoints[r]);
            }

            for (int r = 1; r < n; r++)
            {
                for (int i = 0; i < n - r; i++)
                {
                    middlePoints[r].push_back(middlePoints[r - 1][i] * (1 - t) + middlePoints[r - 1][i + 1] * t);
                }
            }

            for (int r = 0; r <= n - 1; r++)
            {
                pointsCurve0c[r] = middlePoints[r][0];
            }

            for (int r = n - 1; r >= 0; r--)
            {
                pointsCurvec1[n - 1 - r] = middlePoints[r][n - 1 - r];
            }

            // drawing
            DrawingUtils::drawCurve(this->controlPoints, colorControlPoints);

            auto curve0c = getCurve(pointsCurve0c);
            auto curvec1 = getCurve(pointsCurvec1);

            drawControlPoints(pointsCurve0c, colorSubdividedSegment1);
            DrawingUtils::drawCurve(curve0c, colorSubdividedSegment1);

            drawControlPoints(pointsCurvec1, colorSubdividedSegment2);
            DrawingUtils::drawCurve(curvec1, colorSubdividedSegment2);
        }
    }

private:
    std::vector<Point> controlPoints;
    std::vector<Point> bernstein;
    std::vector<Point> curvePoints;

    // colours
    const std::tuple<float, float, float> colorControlPoints = {1.0f, 0.8f, 0.0f};
    const std::tuple<float, float, float> colorCurve = {0.0f, 1.0f, 0.0f};
    const std::tuple<float, float, float> colorBernstein = {0.0f, 0.5f, 1.0f};
    const std::tuple<float, float, float> colorVectors = {1.0f, 0.0f, 1.0f};
    const std::tuple<float, float, float> colorPolar = {0.0f, 0.5f, 1.0f};
    const std::tuple<float, float, float> colorSubdividedSegment1 = {0.8f, 0.0f, 0.6f};
    const std::tuple<float, float, float> colorSubdividedSegment2 = {0.6f, 0.8f, 0.0f};

    double getBernstein(int i, int n, double t, BinomialCoefficients coefficients) const
    {
        //compute consecutive Bernstein polinomial B[i][n](t)=coeff[i]*t^i*(1-t)^(n-i)
        double bernstein = (double)coefficients.getCoefficient(i) * pow(t, i) * pow(1 - t, n - i);
        return bernstein;
    }

    std::vector<Point> getCurve(std::vector<Point> controlPoints) const
    {
        std::vector<Point> curvePoints = std::vector<Point>(CURVE_POINTS_COUNT);

        if (controlPoints.size() >= 1)
        {
            auto coefficients = BinomialCoefficients::BinomialCoefficients(controlPoints.size());

            for (int k = 0; k < curvePoints.size(); k++)
            {
                double t = (double)k / (curvePoints.size() - 1);

                //compute a point on the Bezier curve for a fixed t
                //medium point (de Casteljau) b[i][r](t)=Sum(b[i+j]B[j][r](t), {j, 0, r})
                //in particular b(t)=b[0][n](t)=Sum(b[j]B[j][n](t), {j, 0, n})
                //where b[j] are actually the points of the control polygon
                Point currentPoint = Point(0, 0);

                int n = controlPoints.size() - 1;
                for (int i = 0; i <= n; i++)
                {
                    double bernstein = getBernstein(i, n, t, coefficients);

                    Point p = controlPoints[i] * bernstein;
                    currentPoint = currentPoint + p;
                }
                curvePoints[k] = currentPoint;
            }
        }

        return curvePoints;
    }

    void drawControlPoints(std::vector<Point> controlPoints, std::tuple<float, float, float> color) const
    {
        for (int i = 0; i < controlPoints.size(); i++)
        {
            Point pointi = controlPoints[i];

            DrawingUtils::drawPoint(pointi, color, 7.0f);

            if (i >= 1)
            {
                Point pointiprev = controlPoints[i - 1];
                DrawingUtils::drawLine(pointi, pointiprev, color);
            }
        }
    }
};
