#include "ShapeAxisSeparator.h"
#include "../math/Vector2.h"
#include "../physics/Physics.h"
#include "../utils/Console.h"
#include "../utils/MinMax.h"
#include <math.h>

struct ShapeIntersectionInfo
{
    bool isValid;
    int numIntersections;
    Vector2 averageIntersectionPoint;
    Vector2 averageIntersectionDirection;
};

Vector2 closestPointToLine(const Vector2 &A, const Vector2 &B, const Vector2 &P)
{
    Vector2 AP = P - A;
    Vector2 AB = B - A;

    float magnitudeAB = AB.dot();
    float t = AP.dot(AB) / magnitudeAB;

    return A + AB * t;
}

struct FarthestPointResult
{
    float distance;
    bool negative;
    int shape;
    Vector2 point;
};

float distanceToLine(const Vector2 &A, const Vector2 &B, const Vector2 &P)
{
    Vector2 closest = closestPointToLine(A, B, P);
    return (P - closest).length();
}

ShapeIntersectionInfo getShapeIntersectionInfo(const PointMassesRange &range1, const PointMassesRange &range2)
{
    ShapeIntersectionInfo result;
    int numIntersections = 0;

    for (int i = 0; i < range1.size(); i++)
    {
        Vector2 p0 = range1.pos[i];
        Vector2 p1 = range1.pos[(i + 1) % range1.size()];

        for (int j = 0; j < range2.size(); j++)
        {
            Vector2 op0 = range2.pos[j];
            Vector2 op1 = range2.pos[(j + 1) % range2.size()];
            float t_point, t_edge;
            if (lineSegmentIntersection(p0, p1, op0, op1, t_point, t_edge))
            {
                Vector2 intersection = p0 + (p1 - p0) * t_point;
                Vector2 dir1 = (p1 - p0).normalized();
                Vector2 dir2 = (op1 - op0).normalized();
                result.averageIntersectionDirection += dir1 + dir2;
                numIntersections++;
                result.averageIntersectionPoint += intersection;
                // Console::drawPoint(intersection, 0x00FF00);
            }
        }
    }

    result.isValid = numIntersections > 0;
    result.numIntersections = numIntersections;

    if (result.isValid)
    {
        result.averageIntersectionPoint /= numIntersections;
        result.averageIntersectionDirection = result.averageIntersectionDirection.normalized();
    }

    return result;
}

Vector2 getCentroid(const PointMassesRange &range)
{
    Vector2 centroid;

    for (int i = 0; i < range.size(); i++)
    {
        centroid += range.pos[i];
    }

    return centroid / range.size();
}

void placeOutsideRange(PointMassesRange &range, const Vector2 &averageIntersectionPoint, const Vector2 &averageIntersectionDirection, int shapeIndex, const FarthestPointResult &result)
{
    for (int i = 0; i < range.size(); i++)
    {
        Vector2 p0 = range.pos[i];
        bool isNegative = (p0 - averageIntersectionPoint).cross(averageIntersectionDirection) < 0;
        bool shouldBeNegative = result.shape == shapeIndex ? result.negative : !result.negative;

        if (isNegative != shouldBeNegative)
        {
            Vector2 closestPoint = closestPointToLine(averageIntersectionPoint, averageIntersectionPoint + averageIntersectionDirection, p0);

            Vector2 direction = (closestPoint - p0);
            float directionLength = direction.length();

            if (directionLength != 0.0f)
            {
                direction = direction / directionLength;
            }

            Vector2 oldPos = range.pos[i];
            range.pos[i] = closestPoint + direction * 0.01f;
            float diff = (range.pos[i] - oldPos).length();
            range.velocity[i] *= 0.8f;
        }
    }
}

void findFarthestPointFromRange(const PointMassesRange &range, const Vector2 &averageIntersectionPoint, const Vector2 &averageIntersectionDirection, int shapeIndex, FarthestPointResult &result)
{
    Vector2 A = averageIntersectionPoint;
    Vector2 B = averageIntersectionPoint + averageIntersectionDirection * 10.0f;

    for (int i = 0; i < range.size(); i++)
    {
        Vector2 p0 = range.pos[i];
        bool isNegative = (p0 - averageIntersectionPoint).cross(averageIntersectionDirection) < 0;

        float distance = distanceToLine(A, B, p0);

        if (distance > result.distance)
        {
            result.distance = distance;
            result.negative = isNegative;
            result.shape = shapeIndex;
            result.point = p0;
        }
    }
}

void ShapeAxisSeparator::separateShapesFromIntersectionAxis(PointMassesRange &range1, PointMassesRange &range2)
{
    ShapeIntersectionInfo info = getShapeIntersectionInfo(range1, range2);

    if (info.isValid > 0)
    {
        Vector2 center1 = getCentroid(range1);
        Vector2 center2 = getCentroid(range2);

        Vector2 direction = (center2 - center1).normalized().normalVector();

        FarthestPointResult result = {__FLT_MIN__, false, 0, Vector2()};

        findFarthestPointFromRange(range1, info.averageIntersectionPoint, direction, 0, result);
        findFarthestPointFromRange(range2, info.averageIntersectionPoint, direction, 1, result);
        placeOutsideRange(range1, info.averageIntersectionPoint, direction, 0, result);
        placeOutsideRange(range2, info.averageIntersectionPoint, direction, 1, result);
    }
}
