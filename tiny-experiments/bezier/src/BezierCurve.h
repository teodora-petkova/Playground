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
    }

    BezierCurve(int count)
        : BezierCurve()
    {
        this->controlPoints = std::vector<Point>(count);

        for (int i = 0; i < this->controlPoints.size(); i++)
        {
            Point p = Point(0, 0);
            this->controlPoints[i] = p;
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
        drawControlPoints(this->controlPoints);

        std::vector<Point> points = getCurve(this->controlPoints);

        float r, g, b;
        std::tie(r, g, b) = colorCurve;
        DrawingUtils::drawCurve(points, r, g, b);

        //points.clear();
        //points.shrink_to_fit();
    }

    void drawBernsteins()
    {
        int n = this->controlPoints.size() - 1;
        int size = CURVE_POINTS_COUNT;

        std::vector<std::vector<Point>> bernsteins = std::vector<std::vector<Point>>(size);
        auto coefficients = BinomialCoefficients::BinomialCoefficients(n + 1);

        for (int k = 0; k < size; k++)
        {
            double t = (double)k / (size - 1);

            for (int i = 0; i <= n; i++)
            {
                double bernstein = getBernstein(i, n, t, coefficients);

                int scale = 200;
                bernsteins[i].push_back(Point(t * scale, bernstein * scale));
            }

            for (auto bernstein : bernsteins)
            {
                float r, g, b;
                std::tie(r, g, b) = colorBernstein;
                DrawingUtils::drawCurve(bernstein, r, g, b);
            }
        }
    }

private:
    std::vector<Point> controlPoints;
    std::vector<Point> bernstein;

    // colours
    std::tuple<float, float, float> colorControlPoints = {1.0f, 0.8f, 0.0f};
    std::tuple<float, float, float> colorCurve = {0.0f, 1.0f, 0.0f};
    std::tuple<float, float, float> colorBernstein = {0.0f, 0.5f, 1.0f};

    double getBernstein(int i, int n, double t, BinomialCoefficients coefficients) const
    {
        //compute consecutive Bernstein polinomial B[i][n](t)=coeff[i]*t^i*(1-t)^(n-i)
        double bernstein = (double)coefficients.getCoefficient(i) * pow(t, i) * pow(1 - t, n - i);
        return bernstein;
    }

    std::vector<Point> getCurve(std::vector<Point> controlPoints) const
    {
        std::vector<Point> curvePoints = std::vector<Point>(CURVE_POINTS_COUNT);

        if (controlPoints.size() >= 2)
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

    void drawControlPoints(std::vector<Point> controlPoints) const
    {
        for (int i = 0; i < controlPoints.size(); i++)
        {
            Point pointi = controlPoints[i];
            float r, g, b;
            std::tie(r, g, b) = colorControlPoints;
            DrawingUtils::drawPoint(pointi, r, g, b, 7.0f);

            if (i >= 1)
            {
                Point pointiprev = controlPoints[i - 1];
                DrawingUtils::drawLine(pointi, pointiprev, r, g, b);
            }
        }
    }
};
