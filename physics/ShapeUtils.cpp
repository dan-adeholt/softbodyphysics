#include "ShapeUtils.h"
#include "./Physics.h"

Vector2 ShapeUtils::getAverageShapeVelocity(PointMassesRange points, const Shape &shape)
{
    Vector2 velocity;
    for (int j = shape.start; j < shape.end; j++)
    {
        velocity += points.velocity[j];
    }

    int numPoints = shape.end - shape.start;

    velocity /= numPoints;

    return velocity;
}

ShapeProperties
ShapeUtils::getShapeProperties(PointMassesRange points, const Shape &shape)
{
    ShapeProperties result = {};

    for (ShapeIterator s(shape); s.isValid(); s.next())
    {
        int j = s.index();
        result.center += points.pos[j];
        result.origCenter += points.shapeOriginalPos[j];
    }

    int numPoints = shape.size();
    result.center /= numPoints;
    result.origCenter /= numPoints;

    float A = 0.0f;
    float B = 0.0f;

    // Average angle calculation taken from: https://lisyarus.github.io/blog/posts/soft-body-physics.html
    // More accurate than my first attempt
    for (ShapeIterator s(shape); s.isValid(); s.next())
    {
        int j = s.index();
        Vector2 translatedPos = points.pos[j] - result.center;
        Vector2 translatedOrigPos = points.shapeOriginalPos[j] - result.origCenter;
        A += translatedPos.dot(translatedOrigPos);
        B += translatedPos.cross(translatedOrigPos);
    }

    result.diffAngle = -atan2f(B, A);

    return result;
}

Vector2 ShapeUtils::getShapePos(const PointMassesRange &points, const Shape &shape, int j, const ShapeProperties &averages, const ShapeMatchDragData &dragData)
{
    Vector2 p0 = (points.shapeOriginalPos[j] - averages.origCenter).rotate(averages.diffAngle);
    if (dragData.dragShapeIndex == shape.index)
    {
        p0 += dragData.center;
    }
    else
    {
        p0 += averages.center;
    }

    return p0;
}
