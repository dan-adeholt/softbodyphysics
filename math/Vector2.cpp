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
