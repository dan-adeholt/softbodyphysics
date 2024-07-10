#include "Physics.h"
#include "../containers/Range.h"
#include "../containers/Array.h"
#include "../utils/MinMax.h"
#include "../utils/Console.h"
#include <stdio.h>
#include <math.h>

float coefficentOfRestitution = 0.65f;

inline Vector2 calculateImpulse(PointMass pm0, PointMass pm1, Vector2 segmentNormal, PointMass point, Vector2 minPoint, float minT)
{
    Vector2 velocityLineSegment = pm0.velocity + (pm1.velocity - pm0.velocity) * minT;
    Vector2 relativeVelocity = point.velocity - velocityLineSegment;

    float inverseMass = (1.0f / point.mass) + (2.0f / (pm0.mass)) + (2.0f / (pm1.mass));
    float dotProduct = relativeVelocity.dot(segmentNormal);

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

Vector2 Vector2::fromAngle(float angle)
{
    return {cosf(angle), sinf(angle)};
}

Vector2 Vector2::lerp(const Vector2 &a, const Vector2 &b, float t)
{
    return a + (b - a) * t;
}

void Vector2::operator+=(const Vector2 &other)
{
    x += other.x;
    y += other.y;
}

void Vector2::operator-=(const Vector2 &other)
{
    x -= other.x;
    y -= other.y;
}

void Vector2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
}

void Vector2::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
}

bool Vector2::operator==(const Vector2 &other) const
{
    return x == other.x && y == other.y;
}

bool Vector2::operator!=(const Vector2 &other) const
{
    return x != other.x || y != other.y;
}

float Vector2::length() const
{
    return sqrtf(x * x + y * y);
}

float Vector2::lengthSquared() const
{
    return x * x + y * y;
}

float Vector2::distance(const Vector2 &other) const
{
    return (*this - other).length();
}

float Vector2::distanceSquared(const Vector2 &other) const
{
    return (*this - other).lengthSquared();
}

float Vector2::angle() const
{
    return atan2f(y, x);
}

float Vector2::angle(const Vector2 &other) const
{
    return atan2f(y - other.y, x - other.x);
}

Vector2 Vector2::rotate(float angle) const
{
    return {x * cosf(angle) - y * sinf(angle), x * sinf(angle) + y * cosf(angle)};
}

Vector2 Vector2::reflect(const Vector2 &normal) const
{
    return *this - normal * 2.0f * normal.dot();
}

Vector2 Vector2::operator+(const Vector2 &other) const
{
    return Vector2{x + other.x, y + other.y};
}

Vector2 Vector2::operator-(const Vector2 &other) const
{
    return Vector2{x - other.x, y - other.y};
}

Vector2 Vector2::operator*(float scalar) const
{
    return Vector2{x * scalar, y * scalar};
}

Vector2 Vector2::operator/(float scalar) const
{
    return Vector2{x / scalar, y / scalar};
}

Vector2 Vector2::normalized()
{
    float length = sqrtf(x * x + y * y);
    return {x / length, y / length};
}

int countNumCollisions(Range<PointMass> collisionShape, PointMass point, float outX)
{
    int numIntersections = 0;

    // Check intersections between (point.pos.x, point.pos.y) -> (outX, point.pos.y) and each line segment in shape1
    for (int i = 0; i < collisionShape.size; i++)
    {
        float x0 = collisionShape[i].pos.x;
        float y0 = collisionShape[i].pos.y;
        float x1 = collisionShape[(i + 1) % collisionShape.size].pos.x;
        float y1 = collisionShape[(i + 1) % collisionShape.size].pos.y;

        // If point is outside line segments vertical range, it can never intersect since line is horziontal
        if ((point.pos.y < y0 && point.pos.y < y1) || (point.pos.y > y0 && point.pos.y > y1))
        {
            continue;
        }
        else if (x0 == x1)
        {
            if (point.pos.x <= x1 && outX >= x1)
            {
                numIntersections++;
            }
        }
        else
        {
            float m = (y1 - y0) / (x1 - x0);
            float intersectionX = x0 + (point.pos.y - y0) / m;
            if ((intersectionX >= point.pos.x && intersectionX <= outX) &&
                ((intersectionX >= x0 && intersectionX <= x1) || (intersectionX >= x1 && intersectionX <= x0)))
            {
                numIntersections++;
            }
        }
    }

    return numIntersections;
}

void findClosestLineSegmentToPoint(Range<PointMass> collisionShape, const Vector2 &point, int &minIndex, Vector2 &minPoint, float &minT)
{
    float minDistanceSquared = __FLT_MAX__;

    for (int i = 0; i < collisionShape.size; i++)
    {
        Vector2 segment0 = collisionShape[i].pos;
        Vector2 segment1 = collisionShape[(i + 1) % collisionShape.size].pos;

        // Vector from A to B
        Vector2 segment = segment1 - segment0;
        // Vector from A to P
        Vector2 segmentToPoint = point - segment0;

        if (point.y == segment0.y && point.y == segment1.y)
        {
            continue;
        }

        if (point.x == segment0.x && point.x == segment1.x)
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
}

bool isPointOutsideShape(PointMass &point, const ShapeBoundingBox &box, Range<PointMass> shape)
{
    // First check - is the point outside the bounding box of the other shape?
    // Then extend horizontal line from point to the right,  outside of bounding box.
    return point.pos.x < box.x1 ||
           point.pos.x > box.x2 ||
           point.pos.y < box.y1 ||
           point.pos.y > box.y2 ||
           // If the number of intersections is even, that means that the point is definitively outside
           // of our shape. If it is odd, then it is inside.
           countNumCollisions(shape, point, box.x2 + 10.0f) % 2 == 0;
}

int calculateCollisions(
    Range<PointMass> collisionShape,
    Range<PointMass> movingShape,
    const ShapeBoundingBox &collisionBox,
    const ShapeBoundingBox &movingBox,
    Range<int> &collisionCounterForPoints1,
    Range<int> &collisionCounterForPoints2)
{
    int numCollisions = 0;

    for (int i = 0; i < movingShape.size; i++)
    {
        PointMass &point = movingShape[i];
        Vector2 pointPos = point.pos;

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(point, collisionBox, collisionShape))
        {
            continue;
        }

        numCollisions++;
        collisionCounterForPoints1[i]++;
        collisionCounterForPoints2[i]++;

        int minIndex = -1;
        float minT = 0.0f;
        Vector2 minPoint = {0.0f, 0.0f};
        findClosestLineSegmentToPoint(collisionShape, pointPos, minIndex, minPoint, minT);

        PointMass &pm0 = collisionShape[minIndex];
        PointMass &pm1 = collisionShape[(minIndex + 1) % collisionShape.size];

        Vector2 segmentNormal = Vector2(-pm1.pos.y + pm0.pos.y, pm1.pos.x - pm0.pos.x).normalized();
        Vector2 impulse = calculateImpulse(pm0, pm1, segmentNormal, point, minPoint, minT);

        point.force += impulse;
        pm0.force -= (impulse * (1.0f - minT));
        pm1.force -= (impulse * minT);

        Vector2 reflection = point.velocity.reflect(segmentNormal).normalized();
        point.pos = minPoint + reflection * 0.01f;
    }

    return numCollisions;
}

int calculateStaticCollisions(
    Range<PointMass> staticShape,
    Range<PointMass> movingShape,
    const ShapeBoundingBox &staticBox,
    const ShapeBoundingBox &movingBox,
    Range<int> &collisionCounterForMovingShape)
{
    int numCollisions = 0;

    for (int i = 0; i < movingShape.size; i++)
    {
        PointMass &point = movingShape[i];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(point, staticBox, staticShape))
        {
            continue;
        }

        numCollisions++;
        collisionCounterForMovingShape[i]++;

        int minIndex = -1;
        float minT = 0.0f;
        Vector2 minPoint = {0.0f, 0.0f};
        findClosestLineSegmentToPoint(staticShape, point.pos, minIndex, minPoint, minT);

        PointMass &pm0 = staticShape[minIndex];
        PointMass &pm1 = staticShape[(minIndex + 1) % staticShape.size];
        Vector2 segmentDirection(pm1.pos.x - pm0.pos.x, pm1.pos.y - pm0.pos.y);
        Vector2 segmentNormal = Vector2(-segmentDirection.y, segmentDirection.x).normalized();

        Vector2 impulse = calculateImpulse(pm0, pm1, segmentNormal, point, minPoint, minT);
        Vector2 reflection = point.velocity.reflect(segmentNormal).normalized();
        point.force += impulse;
        point.pos = minPoint + reflection * 0.01f;
    }

    return numCollisions;
}

ShapeBoundingBox calculateShapeBoundingBox(const Shape &shape, int shapeIndex, const Array<PointMass> &points)
{
    const Range<PointMass> &range = points.range(shape);
    float minX = range[0].pos.x;
    float maxX = range[0].pos.x;
    float minY = range[0].pos.y;
    float maxY = range[0].pos.y;

    for (int j = 0; j < range.size; j++)
    {
        const PointMass &point = range[j];
        minX = min(minX, point.pos.x);
        maxX = max(maxX, point.pos.x);
        minY = min(minY, point.pos.y);
        maxY = max(maxY, point.pos.y);
    }

    return {shapeIndex, minX, minY, maxX, maxY};
}

void calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes, const Array<Shape> &shapes, const Array<PointMass> &points)
{
    boundingBoxes.clear();

    for (int i = 0; i < shapes.size(); i++)
    {
        boundingBoxes.push(calculateShapeBoundingBox(shapes[i], i, points));
    }
}

void applyGravity(Range<PointMass> &points)
{
    for (int i = 0; i < points.size; i++)
    {
        points[i].force.y += 0.05f * points[i].mass * 0.0001f;
    }
}

void applySprings(Range<PointMass> &points, Range<Spring> &springs, const Range<int> &counterForCollisions)
{
    for (int i = 0; i < springs.size; i++)
    {
        const Spring &spring = springs[i];
        PointMass &point1 = points[spring.pointA];
        PointMass &point2 = points[spring.pointB];

        Vector2 offset = (point2.pos - point1.pos);
        float delta = (offset.length() - spring.length);

        float springForce = delta * spring.stiffness;
        Vector2 offsetNormal = offset.normalized();

        float dampForce = offsetNormal.dot(point2.velocity - point1.velocity) * spring.damping;

        Vector2 force = offsetNormal * (springForce + dampForce) * 0.085f * 0.0001f;
        Vector2 diff = point2.velocity - point1.velocity;

        point1.force += force;
        point2.force -= force;
    }
}