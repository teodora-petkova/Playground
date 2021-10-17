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

        int curvePointsCount = this->curvePoints.size();
        for (int i = 0; i < curvePointsCount; i++)
        {
            Point p = Point(0, 0);
            this->curvePoints[i] = p;
        }
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

    void drawControlPoints()
    {
        for (int i = 0; i < this->controlPoints.size(); i++)
        {
            Point pointi = this->controlPoints[i];
            float r, g, b;
            std::tie(r, g, b) = colorControlPoints;
            DrawingUtils::drawPoint(pointi, r, g, b, 7.0f);

            if (i >= 1)
            {
                Point pointiprev = this->controlPoints[i - 1];
                DrawingUtils::drawLine(pointi, pointiprev, r, g, b);
            }
        }
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

    void updateCurve()
    {
        auto coefficients = BinomialCoefficients::BinomialCoefficients(this->controlPoints.size());
        for (int k = 0; k <= this->curvePoints.size(); k++)
        {
            double t = (double)k / this->curvePoints.size();
            this->curvePoints[k] = getPointOnBezierCurve(t, coefficients);
        }
    }

    void drawCurve() const
    {
        float r, g, b;
        std::tie(r, g, b) = colorBezierCurve;

        DrawingUtils::drawCurve(this->curvePoints, r, g, b);
    }

    void updateBernsteins()
    {
        int n = this->controlPoints.size() - 1;

        this->bernsteins.clear();
        auto coefficients = BinomialCoefficients::BinomialCoefficients(n + 1);

        int size = CURVE_POINTS_COUNT / 2;
        for (int k = 0; k <= size; k++)
        {
            double t = (double)k / size;

            double x = 0;
            double y = 0;

            for (int i = 0; i <= n; i++)
            {
                //compute consecutive Bernstein polinomial B[i][n](t)=coeff[i]*t^i*(1-t)^(n-i)
                double bernstein = (double)coefficients.getCoefficient(i) * pow(t, i) * pow(1 - t, n - i);

                int scale = 200;
                Point p = Point(t * scale, bernstein * scale);
                if (k == 0)
                {
                    std::vector<Point> points;
                    points.push_back(p);
                    this->bernsteins.push_back(points);
                }
                else
                {
                    this->bernsteins[i].push_back(p);
                }
            }
        }
    }

    void drawBernsteins() const
    {
        for (auto bernstein : this->bernsteins)
        {
            float r, g, b;
            std::tie(r, g, b) = colorBernstein;
            DrawingUtils::drawCurve(bernstein, r, g, b);
        }
    }

private:
    std::vector<Point> controlPoints;
    std::vector<Point> curvePoints;
    std::vector<std::vector<Point>> bernsteins;

    // colours
    std::tuple<float, float, float> colorControlPoints = {1.0f, 0.8f, 0.0f};
    std::tuple<float, float, float> colorBezierCurve = {0.0f, 1.0f, 0.0f};
    std::tuple<float, float, float> colorBernstein = {0.0f, 1.0f, 0.0f};

    //compute a point on the Bezier curve for a fixed t
    //medium point (de Casteljau) b[i][r](t)=Sum(b[i+j]B[j][r](t), {j, 0, r})
    //in particular b(t)=b[0][n](t)=Sum(b[j]B[j][n](t), {j, 0, n})
    //where b[j] are actually the points of the control polygon
    Point getPointOnBezierCurve(double t, BinomialCoefficients coefficients) const
    {
        double x = 0;
        double y = 0;

        int n = this->controlPoints.size() - 1;
        for (int i = 0; i <= n; i++)
        {
            //compute consecutive Bernstein polinomial B[i][n](t)=coeff[i]*t^i*(1-t)^(n-i)
            double bernstein = coefficients.getCoefficient(i) * pow(t, i) * pow(1 - t, n - i);
            x += this->controlPoints[i].x * bernstein;
            y += this->controlPoints[i].y * bernstein;
        }

        return Point(x, y);
    }
};
