#include "Physics.h"
#include "../containers/Range.h"
#include "../containers/Array.h"
#include "../utils/MinMax.h"
#include "../utils/Console.h"
#include <stdio.h>
#include <math.h>

float coefficentOfRestitution = 0.65f;
// Function to compute the length of a 2D vector using NEON intrinsics

inline Vector2 calculateImpulse(float pm0VelX, float pm0VelY, float pm0Mass, float pm1VelX, float pm1VelY, float pm1Mass, Vector2 segmentNormal, float pointVelX, float pointVelY, float pointMass, float minT)
{
    float velocityLineSegmentX = pm0VelX + (pm1VelX - pm0VelX) * minT;
    float velocityLineSegmentY = pm0VelY + (pm1VelY - pm0VelY) * minT;
    float relativeVelocityX = pointVelX - velocityLineSegmentX;
    float relativeVelocityY = pointVelY - velocityLineSegmentY;

    float inverseMass = (1.0f / pointMass) + (2.0f / (pm0Mass)) + (2.0f / (pm1Mass));
    float dotProduct = Vector2::vec2dot(relativeVelocityX, relativeVelocityY, segmentNormal.x, segmentNormal.y);

    float impulseMagnitude = (-(1.0f + coefficentOfRestitution) * dotProduct) / inverseMass;
    return segmentNormal * impulseMagnitude;
}

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
struct IntersectionResult
{
    Vector2 point;
    float t;
    int segmentIndex;
    bool found;
};

// Function to calculate the intersection point of two line segments
IntersectionResult lineIntersection(const Vector2 &s1, const Vector2 &s2, const Vector2 &p1, const Vector2 &p2)
{
    float x1 = s1.x, y1 = s1.y;
    float x2 = s2.x, y2 = s2.y;
    float x3 = p1.x, y3 = p1.y;
    float x4 = p2.x, y4 = p2.y;

    float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (fabsf(denom) < 1e-6f)
    {
        IntersectionResult res = {
            Vector2::zero(),
            0,
            0,
            false};
        return res;
    }

    float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
    float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        Vector2 intersectionPoint = {
            x1 + t * (x2 - x1),
            y1 + t * (y2 - y1)};
        return {intersectionPoint, t, -1, true};
    }

    IntersectionResult res = {
        Vector2::zero(),
        0,
        0,
        false};

    return res; // No interseweqction within segments
}

int countNumCollisions(PointMassesRange collisionShape, float pointX, float pointY, float outX)
{
    int numIntersections = 0;

    // Check intersections between (point.pos.x, point.pos.y) -> (outX, point.pos.y) and each line segment in shape1
    for (int i = 0; i < collisionShape.size(); i++)
    {
        Vector2 p0 = collisionShape.pos[i];
        Vector2 p1 = collisionShape.pos[(i + 1) % collisionShape.size()];

        // If point is outside line segments vertical range, it can never intersect since line is horziontal
        if ((pointY < p0.y && pointY < p1.y) || (pointY > p0.y && pointY > p1.y))
        {
            continue;
        }
        else if (p0.x == p1.x)
        {
            if (pointX <= p1.x && outX >= p1.x)
            {
                numIntersections++;
            }
        }
        else
        {
            float m = (p1.y - p0.y) / (p1.x - p0.x);
            float intersectionX = p0.x + (pointY - p0.y) / m;
            if ((intersectionX >= pointX && intersectionX <= outX) &&
                ((intersectionX >= p0.x && intersectionX <= p1.x) || (intersectionX >= p1.x && intersectionX <= p0.x)))
            {
                numIntersections++;
            }
        }
    }

    return numIntersections;
}

void findClosestLineSegmentToPoint(PointMassesRange collisionShape, const Vector2 &point, const Vector2 &velocity, int &minIndex, Vector2 &minPoint, float &minT)
{
    float minDistanceSquared = __FLT_MAX__;
    int collisionShapeSize = collisionShape.pos.size;
    for (int i = 0; i < collisionShapeSize; i++)
    {
        Vector2 segment0 = collisionShape.pos[i];
        Vector2 segment1 = collisionShape.pos[(i + 1) % collisionShapeSize];

        // Vector from A to B
        Vector2 segment = segment1 - segment0;
        // Vector from A to P
        Vector2 segmentToPoint = point - segment0;
        Vector2 segmentNormal = segment.normalVector().normalized();

        if (fabs(segmentNormal.dot(velocity)) < 0.000001f)
        {
            continue;
        }

        // The projection of point P onto the line defined by segment AB is given by:
        // v dot w / v dot v
        // Compute projection t
        float t = segmentToPoint.dot(segment) / segment.dot();
        t = clamp(t, 0.0f, 1.0f);

        Vector2 closestPoint = segment0 + segment * t;
        Vector2 pointToClosestPoint = closestPoint - point;

        float distanceToClosestPointSquared = (pointToClosestPoint).lengthSquared();

        if (distanceToClosestPointSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceToClosestPointSquared;
            minIndex = i;
            minPoint = closestPoint;
            minT = t;
        }
    }

    if (minDistanceSquared == __FLT_MAX__)
    {
        Console::log("ERROR: Failed to find line segment!");
    }
}
// PointMassesRange collisionShape, float pointX, float pointY, float outX
bool isPointOutsideShape(float pointX, float pointY, const ShapeBoundingBox &box, PointMassesRange shape)
{
    // First check - is the point outside the bounding box of the other shape?
    // Then extend horizontal line from point to the right,  outside of bounding box.
    return pointX < box.x1 ||
           pointX > box.x2 ||
           pointY < box.y1 ||
           pointY > box.y2 ||
           // If the number of intersections is even, that means that the point is definitively outside
           // of our shape. If it is odd, then it is inside.
           countNumCollisions(shape, pointX, pointY, box.x2 + 10.0f) % 2 == 0;
}

int calculateCollisions(
    PointMassesRange collisionShape,
    PointMassesRange movingShape,
    const ShapeBoundingBox &collisionBox,
    const ShapeBoundingBox &movingBox,
    Range<int> &collisionCounterForPoints1,
    Range<int> &collisionCounterForPoints2,
    float step)
{

    int numCollisions = 0;

    for (int i = 0; i < movingShape.pos.size; i++)
    {
        Vector2 pointPos = movingShape.pos[i];
        Vector2 pointVelocity = movingShape.velocity[i];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(pointPos.x, pointPos.y, collisionBox, collisionShape))
        {
            continue;
        }
        // If point is not moving, we need to skip it, because otherwise findClosestLineSegmentToPoint won't work
        // (it uses velocity to determine closest point on line segment). And if it is not moving it cannot collide with anything -
        // the other shape will collide with it.
        else if (fabs(pointVelocity.x) < 0.00001f && fabs(pointVelocity.y) < 0.00001f)
        {
            continue;
        }

        numCollisions++;
        collisionCounterForPoints1[i]++;
        collisionCounterForPoints2[i]++;

        int minIndex = -1;
        float minT = 0.0f;
        Vector2 minPoint = {0.0f, 0.0f};
        findClosestLineSegmentToPoint(collisionShape, pointPos, pointVelocity, minIndex, minPoint, minT);
        float pointMass = movingShape.mass[i];
        if (minIndex == -1)
        {
            continue;
        }

        // PointMass &pm0 = collisionShape[minIndex];
        // PointMass &pm1 = collisionShape[(minIndex + 1) % collisionShape.size];
        // inline Vector2 calculateImpulse(float pm0VelX, float pm0VelY, float pm0Mass, float pm1VelX, float pm1VelY, float pm1Mass, float segmentNormalX, float segmentNormalY, float pointVelX, float pointVelY, float pointMass, float minT)
        Vector2 pm0Vel = collisionShape.velocity[minIndex];
        float pm0Mass = collisionShape.mass[minIndex];
        Vector2 pm1Vel = collisionShape.velocity[(minIndex + 1) % collisionShape.pos.size];
        float pm1Mass = collisionShape.mass[(minIndex + 1) % collisionShape.pos.size];
        Vector2 pm0Pos = collisionShape.pos[minIndex];
        Vector2 pm1Pos = collisionShape.pos[(minIndex + 1) % collisionShape.pos.size];

        Vector2 segmentNormal = Vector2(-pm1Pos.y + pm0Pos.y, pm1Pos.x - pm0Pos.x).normalized();

        // Since this is an impulse and not a continuously applied force, we need to divide by the step
        // so that when the velocity and position are updated, the impulse is applied correctly.
        Vector2 impulse = calculateImpulse(pm0Vel.x, pm0Vel.y, pm0Mass, pm1Vel.x, pm1Vel.y, pm1Mass, segmentNormal, pointVelocity.x, pointVelocity.y, pointMass, minT) / step;
        movingShape.acceleration[i] += impulse / pointMass;

        Vector2 pm0Acceleration = (impulse * (1.0f - minT)) / pm0Mass;
        collisionShape.acceleration[minIndex] -= pm0Acceleration;
        Vector2 pm1Acceleration = (impulse * minT) / pm1Mass;
        collisionShape.acceleration[(minIndex + 1) % collisionShape.pos.size] -= pm1Acceleration;

        Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();
        movingShape.pos[i] = minPoint + reflection * 0.1f;
    }

    return numCollisions;
}

int calculateStaticCollisions(
    PointMassesRange staticShape,
    PointMassesRange movingShape,
    const ShapeBoundingBox &staticBox,
    const ShapeBoundingBox &movingBox,
    Range<int> &collisionCounterForMovingShape,
    float step)
{
    int numCollisions = 0;

    for (int i = 0; i < movingShape.pos.size; i++)
    {
        Vector2 point = movingShape.pos[i];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(point.x, point.y, staticBox, staticShape))
        {
            continue;
        }
        Vector2 velocity = movingShape.velocity[i];

        numCollisions++;
        collisionCounterForMovingShape[i]++;

        int minIndex = -1;
        float minT = 0.0f;
        Vector2 minPoint = {0.0f, 0.0f};
        findClosestLineSegmentToPoint(staticShape, point, velocity, minIndex, minPoint, minT);

        if (minIndex == -1)
        {
            continue;
        }

        float pointMass = movingShape.mass[i];
        int nextIndex = (minIndex + 1) % staticShape.pos.size;

        Vector2 pm0 = staticShape.pos[minIndex];
        Vector2 pm0Vel = staticShape.velocity[minIndex];
        float pm0Mass = staticShape.mass[minIndex];

        Vector2 pm1 = staticShape.pos[nextIndex];
        Vector2 pm1Vel = staticShape.velocity[nextIndex];
        float pm1Mass = staticShape.mass[nextIndex];

        Vector2 segmentDirection(pm1.x - pm0.x, pm1.y - pm0.y);
        Vector2 segmentNormal = Vector2(-segmentDirection.y, segmentDirection.x).normalized();

        // Since this is an impulse and not a continuously applied force, we need to divide by the step
        // so that when the velocity and position are updated, the impulse is applied correctly.
        Vector2 impulse = calculateImpulse(pm0Vel.x, pm0Vel.y, pm0Mass, pm1Vel.x, pm1Vel.y, pm1Mass, segmentNormal, velocity.x, velocity.y, pointMass, minT) / step;
        Vector2 reflection = velocity.reflect(segmentNormal).normalized();
        movingShape.acceleration[i] += impulse / pointMass;
        movingShape.pos[i] = minPoint + reflection * 0.1f;
    }

    return numCollisions;
}

ShapeBoundingBox calculateShapeBoundingBox(const Shape &shape, int shapeIndex, const PointMasses &points)
{
    const PointMassesRange &range = points.range(shape.start, shape.end);
    Vector2 pos = range.pos[0];
    float minX = pos.x;
    float maxX = pos.x;
    float minY = pos.y;
    float maxY = pos.y;

    for (int j = 1; j < range.pos.size; j++)
    {
        Vector2 otherPos = range.pos[j];

        minX = min(minX, otherPos.x);
        maxX = max(maxX, otherPos.x);
        minY = min(minY, otherPos.y);
        maxY = max(maxY, otherPos.y);
    }

    return {shapeIndex, minX, minY, maxX, maxY};
}

void calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes, const Array<Shape> &shapes, const PointMasses &points)
{
    boundingBoxes.clear();

    for (int i = 0; i < shapes.size(); i++)
    {
        boundingBoxes.push(calculateShapeBoundingBox(shapes[i], i, points));
    }
}

float springFactor = 0.085f * 0.001f;

void applySpringDerivatives(PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives)
{
    for (int i = 0; i < springs.size; i++)
    {
        const Spring &spring = springs[i];

        Vector2 p0(points.pos[spring.pointA]);
        Vector2 p1(points.pos[spring.pointB]);

        PointDerivative &derivative1 = derivatives[spring.pointA];
        PointDerivative &derivative2 = derivatives[spring.pointB];

        Vector2 direction(p1 - p0);
        float offsetLength = direction.length();

        if (offsetLength > 0.001f)
        {
            float delta = (offsetLength - spring.length);

            float springDamping = spring.damping;
            float springStiffness = spring.stiffness;

            float springForce = delta * springStiffness;
            springForce = min(springForce, 10000.0f);
            Vector2 directionNormalized = direction / offsetLength;

            Vector2 dv(points.velocity[spring.pointB] - points.velocity[spring.pointA]);
            float dampForce = directionNormalized.dot(dv * springDamping);
            dampForce = min(dampForce, 10000.0f);
            float combinedForce = (springForce + dampForce) * springFactor;

            Vector2 force(directionNormalized * combinedForce);

            float p1Mass = points.mass[spring.pointA];
            float p2Mass = points.mass[spring.pointB];

            derivative1.acceleration += force / p1Mass;
            derivative2.acceleration -= force / p2Mass;
        }
    }
}