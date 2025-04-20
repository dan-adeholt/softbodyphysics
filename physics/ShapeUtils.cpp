#include "ShapeUtils.h"
#include "./Physics.h"
#include "../utils/Console.h"

ShapeVelocities ShapeUtils::getAverageShapeVelocity(PointMassesRange points, const Shape &shape)
{
    Vector2 centerOfMassVelocity;
    Vector2 centerOfMass;
    for (int j = shape.start; j < shape.end; j++)
    {
        centerOfMassVelocity += points.velocity[j];
        centerOfMass += points.pos[j];
    }

    int numPoints = shape.end - shape.start;

    centerOfMassVelocity /= numPoints;
    centerOfMass /= numPoints;

    // compute proper angular velocity = (sum r×v) / (sum |r|^2)

    float numerator = 0.0f;
    float denom = 0.0f;

    /*
     * We now build up two quantities to extract the shape’s scalar angular velocity ω:
     *
     *   • numerator   = Σ_i (r_i × v_i)
     *       - r_i      is the vector from the center of mass to point i
     *       - v_i      is the velocity of point i relative to the center of mass velocity
     *       - (r_i × v_i) in 2D is a scalar giving that point’s angular momentum about the COM
     *
     *   • denom       = Σ_i |r_i|^2
     *       - |r_i|^2  is the squared distance (moment arm) of point i from the COM
     *       - summing these gives the moment of inertia for unit masses
     *
     * Finally, ω = (total angular momentum) / (moment of inertia)
     *           = numerator / denom
     */
    for (int j = shape.start; j < shape.end; j++)
    {
        Vector2 rel = points.pos[j] - centerOfMass;
        Vector2 relVel = points.velocity[j] - centerOfMassVelocity;

        numerator += rel.cross(relVel);
        denom += rel.dot(rel); // |r|^2
    }

    float angularVelocity = (denom > 0.0f)
                                ? numerator / denom
                                : 0.0f;

    return {centerOfMassVelocity, centerOfMass, angularVelocity};
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
    if (dragData.dragShapeIndex == shape.index && shape.index != -1)
    {
        p0 += dragData.center;
    }
    else
    {
        p0 += averages.center;
    }

    return p0;
}
