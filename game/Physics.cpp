#include "Physics.h"
#include "../containers/Range.h"
#include "../containers/Array.h"
#include "../utils/MinMax.h"
#include "../utils/Console.h"
#include <stdio.h>
#include <math.h>

float coefficentOfRestitution = 0.65f;

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
    for (int i = 0; i < collisionShape.x.size; i++)
    {

        float x0 = collisionShape.x[i];
        float y0 = collisionShape.y[i];
        float x1 = collisionShape.x[(i + 1) % collisionShape.x.size];
        float y1 = collisionShape.y[(i + 1) % collisionShape.x.size];

        // If point is outside line segments vertical range, it can never intersect since line is horziontal
        if ((pointY < y0 && pointY < y1) || (pointY > y0 && pointY > y1))
        {
            continue;
        }
        else if (x0 == x1)
        {
            if (pointX <= x1 && outX >= x1)
            {
                numIntersections++;
            }
        }
        else
        {
            float m = (y1 - y0) / (x1 - x0);
            float intersectionX = x0 + (pointY - y0) / m;
            if ((intersectionX >= pointX && intersectionX <= outX) &&
                ((intersectionX >= x0 && intersectionX <= x1) || (intersectionX >= x1 && intersectionX <= x0)))
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
    int collisionShapeSize = collisionShape.x.size;
    for (int i = 0; i < collisionShapeSize; i++)
    {
        float segment0X = collisionShape.x[i];
        float segment0Y = collisionShape.y[i];
        float segment1X = collisionShape.x[(i + 1) % collisionShapeSize];
        float segment1Y = collisionShape.y[(i + 1) % collisionShapeSize];

        Vector2 segment0 = {segment0X, segment0Y};
        Vector2 segment1 = {segment1X, segment1Y};

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
    double step)
{
    int numCollisions = 0;

    for (int i = 0; i < movingShape.x.size; i++)
    {
        float pointX = movingShape.x[i];
        float pointY = movingShape.y[i];
        float pointVelocityX = movingShape.velocityX[i];
        float pointVelocityY = movingShape.velocityY[i];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(pointX, pointY, collisionBox, collisionShape))
        {
            continue;
        }
        // If point is not moving, we need to skip it, because otherwise findClosestLineSegmentToPoint won't work
        // (it uses velocity to determine closest point on line segment). And if it is not moving it cannot collide with anything -
        // the other shape will collide with it.
        else if (fabs(pointVelocityX) < 0.00001f && fabs(pointVelocityY) < 0.00001f)
        {
            continue;
        }

        numCollisions++;
        collisionCounterForPoints1[i]++;
        collisionCounterForPoints2[i]++;

        int minIndex = -1;
        float minT = 0.0f;
        Vector2 minPoint = {0.0f, 0.0f};
        Vector2 pointPos = {pointX, pointY};
        Vector2 pointVelocity = {pointVelocityX, pointVelocityY};
        findClosestLineSegmentToPoint(collisionShape, pointPos, pointVelocity, minIndex, minPoint, minT);
        float pointMass = movingShape.mass[i];
        if (minIndex == -1)
        {
            continue;
        }

        // PointMass &pm0 = collisionShape[minIndex];
        // PointMass &pm1 = collisionShape[(minIndex + 1) % collisionShape.size];
        // inline Vector2 calculateImpulse(float pm0VelX, float pm0VelY, float pm0Mass, float pm1VelX, float pm1VelY, float pm1Mass, float segmentNormalX, float segmentNormalY, float pointVelX, float pointVelY, float pointMass, float minT)
        float pm0VelX = collisionShape.velocityX[minIndex];
        float pm0VelY = collisionShape.velocityY[minIndex];
        float pm0Mass = collisionShape.mass[minIndex];
        float pm1VelX = collisionShape.velocityX[(minIndex + 1) % collisionShape.x.size];
        float pm1VelY = collisionShape.velocityY[(minIndex + 1) % collisionShape.x.size];
        float pm1Mass = collisionShape.mass[(minIndex + 1) % collisionShape.x.size];
        float pm0PosX = collisionShape.x[minIndex];
        float pm0PosY = collisionShape.y[minIndex];
        float pm1PosX = collisionShape.x[(minIndex + 1) % collisionShape.x.size];
        float pm1PosY = collisionShape.y[(minIndex + 1) % collisionShape.x.size];

        Vector2 segmentNormal = Vector2(-pm1PosY + pm0PosY, pm1PosX - pm0PosX).normalized();

        // Since this is an impulse and not a continuously applied force, we need to divide by the step
        // so that when the velocity and position are updated, the impulse is applied correctly.
        Vector2 impulse = calculateImpulse(pm0VelX, pm0VelY, pm0Mass, pm1VelX, pm1VelY, pm1Mass, segmentNormal, pointVelocityX, pointVelocityY, pointMass, minT) / step;
        movingShape.accelerationX[i] += impulse.x / pointMass;
        movingShape.accelerationY[i] += impulse.y / pointMass;

        Vector2 pm0Acceleration = (impulse * (1.0f - minT)) / pm0Mass;
        collisionShape.accelerationX[minIndex] -= pm0Acceleration.x;
        collisionShape.accelerationY[minIndex] -= pm0Acceleration.y;
        Vector2 pm1Acceleration = (impulse * minT) / pm1Mass;
        collisionShape.accelerationX[(minIndex + 1) % collisionShape.x.size] -= pm1Acceleration.x;
        collisionShape.accelerationY[(minIndex + 1) % collisionShape.x.size] -= pm1Acceleration.y;

        Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();
        movingShape.x[i] = minPoint.x + reflection.x * 0.1f;
        movingShape.y[i] = minPoint.y + reflection.y * 0.1f;
    }

    return numCollisions;
}

int calculateStaticCollisions(
    PointMassesRange staticShape,
    PointMassesRange movingShape,
    const ShapeBoundingBox &staticBox,
    const ShapeBoundingBox &movingBox,
    Range<int> &collisionCounterForMovingShape,
    double step)
{
    int numCollisions = 0;

    for (int i = 0; i < movingShape.x.size; i++)
    {
        float pointX = movingShape.x[i];
        float pointY = movingShape.y[i];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(pointX, pointY, staticBox, staticShape))
        {
            continue;
        }
        float pointVelocityX = movingShape.velocityX[i];
        float pointVelocityY = movingShape.velocityY[i];

        numCollisions++;
        collisionCounterForMovingShape[i]++;

        int minIndex = -1;
        float minT = 0.0f;
        Vector2 minPoint = {0.0f, 0.0f};
        Vector2 point = {pointX, pointY};
        Vector2 velocity = {pointVelocityX, pointVelocityY};

        findClosestLineSegmentToPoint(staticShape, point, velocity, minIndex, minPoint, minT);

        if (minIndex == -1)
        {
            continue;
        }

        float pointMass = movingShape.mass[i];
        int nextIndex = (minIndex + 1) % staticShape.x.size;

        float pm0X = staticShape.x[minIndex];
        float pm0Y = staticShape.y[minIndex];
        float pm0VelX = staticShape.velocityX[minIndex];
        float pm0VelY = staticShape.velocityY[minIndex];
        float pm0Mass = staticShape.mass[minIndex];

        float pm1X = staticShape.x[nextIndex];
        float pm1Y = staticShape.y[nextIndex];
        float pm1VelX = staticShape.velocityX[nextIndex];
        float pm1VelY = staticShape.velocityY[nextIndex];
        float pm1Mass = staticShape.mass[nextIndex];

        Vector2 segmentDirection(pm1X - pm0X, pm1Y - pm0Y);
        Vector2 segmentNormal = Vector2(-segmentDirection.y, segmentDirection.x).normalized();

        // Since this is an impulse and not a continuously applied force, we need to divide by the step
        // so that when the velocity and position are updated, the impulse is applied correctly.
        Vector2 impulse = calculateImpulse(pm0VelX, pm0VelY, pm0Mass, pm1VelX, pm1VelY, pm1Mass, segmentNormal, pointVelocityX, pointVelocityY, pointMass, minT) / step;
        Vector2 reflection = velocity.reflect(segmentNormal).normalized();
        movingShape.accelerationX[i] += impulse.x / pointMass;
        movingShape.accelerationY[i] += impulse.y / pointMass;
        movingShape.x[i] = minPoint.x + reflection.x * 0.1f;
        movingShape.y[i] = minPoint.y + reflection.y * 0.1f;
    }

    return numCollisions;
}

ShapeBoundingBox calculateShapeBoundingBox(const Shape &shape, int shapeIndex, const PointMasses &points)
{
    const PointMassesRange &range = points.range(shape.start, shape.end);
    float minX = range.x[0];
    float maxX = range.x[0];
    float minY = range.y[0];
    float maxY = range.y[0];

    for (int j = 0; j < range.x.size; j++)
    {
        float x = range.x[j];
        float y = range.y[j];

        minX = min(minX, x);
        maxX = max(maxX, x);
        minY = min(minY, y);
        maxY = max(maxY, y);
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

void applyGravity(PointMassesRange &points, double timeStep)
{
    for (int i = 0; i < points.y.size; i++)
    {
        // Multiply by mass to cancel out in the subsequent force calculation
        points.accelerationY[i] += (0.05f * 0.003f);
    }
}

float springFactor = 0.085f * 0.001f;

void applySpringDerivatives(PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives)
{
    for (int i = 0; i < springs.size; i++)
    {
        const Spring &spring = springs[i];

        float x1 = points.x[spring.pointA];
        float y1 = points.y[spring.pointA];
        float x2 = points.x[spring.pointB];
        float y2 = points.y[spring.pointB];

        PointDerivative &derivative1 = derivatives[spring.pointA];
        PointDerivative &derivative2 = derivatives[spring.pointB];

        float dx = x2 - x1;
        float dy = y2 - y1;
        float offsetLength = Vector2::vec2length(dx, dy);
        float delta = (offsetLength - spring.length);

        if (offsetLength > 0.001f)
        {
            float springForce = delta * spring.stiffness * 5.0f;
            springForce = min(springForce, 10000.0f);
            float dxn = dx / offsetLength;
            float dyn = dy / offsetLength;

            float v1x = points.velocityX[spring.pointA];
            float v1y = points.velocityY[spring.pointA];
            float v2x = points.velocityX[spring.pointB];
            float v2y = points.velocityY[spring.pointB];
            float dvx = v2x - v1x;
            float dvy = v2y - v1y;

            float dampForce = Vector2::vec2dot(dxn, dyn, dvx, dvy) * spring.damping;
            dampForce = min(dampForce, 10000.0f);
            float combinedForce = springForce + dampForce;
            float forceX = dxn * combinedForce * springFactor;
            float forceY = dyn * combinedForce * springFactor;

            float p1Mass = points.mass[spring.pointA];
            float p2Mass = points.mass[spring.pointB];

            derivative1.acceleration.x += forceX / p1Mass;
            derivative1.acceleration.y += forceY / p1Mass,

                derivative2.acceleration.x -= forceX / p2Mass;
            derivative2.acceleration.y -= forceY / p2Mass;
        }
    }
}