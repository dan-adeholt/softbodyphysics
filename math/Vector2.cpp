#include "./Vector2.h"
#include <math.h>
#include "../utils/MinMax.h"

Vector2 Vector2::zero()
{
    return {0.0f, 0.0f};
}

Vector2 Vector2::one()
{
    return {1.0f, 1.0f};
}

Vector2 Vector2::up()
{
    return {0.0f, 1.0f};
}

Vector2 Vector2::down()
{
    return {0.0f, -1.0f};
}

Vector2 Vector2::left()
{
    return {-1.0f, 0.0f};
}

Vector2 Vector2::right()
{
    return {1.0f, 0.0f};
}

Vector2 intersectLineSegmentPoint(const Vector2 &p0, const Vector2 &p1, Vector2 d)
{
    Vector2 segment = p1 - p0;
    d = d.normalized();

    float denom = d.dot(segment);
    if (abs(denom) < 1e-6f)
    {
        // Line and direction are parallel
        return p0;
    }

    float t = segment.dot(d) / segment.dot(segment);

    if (t < 0)
    {
        // Intersection is behind p0, reverse direction
        d = Vector2(-d.x, -d.y);
        t = segment.dot(d) / segment.dot(segment);
    }

    // Clamp t between 0 and 1
    t = clamp(t, 0.0f, 1.0f);
    return p0 + segment * t;
}

Vector2 closestPointToLineSegment(const Vector2 &p0, const Vector2 &p1, const Vector2 &point)
{
    // Vector from A to B
    Vector2 segment = p1 - p0;
    // Vector from A to P
    Vector2 segmentToPoint = point - p0;
    Vector2 segmentNormal = segment.normalVector().normalized();
    Vector2 pointOutside = (p0 + segment * 0.5f) - segmentNormal * 2.0f;

    // The projection of point P onto the line defined by segment AB is given by:
    // v dot w / v dot v
    // Compute projection t
    float t = segmentToPoint.dot(segment) / segment.dot();
    t = clamp(t, 0.0f, 1.0f);

    return p0 + segment * t;
}