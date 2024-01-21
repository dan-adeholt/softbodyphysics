#include "Physics.h"
#include "../containers/Range.h"
#include "../containers/Array.h"
#include "../utils/MinMax.h"
#include <stdio.h>
#include <math.h>

float coefficentOfRestitution = 0.65f;

Vector2 Vector2::zero()
{
    return {0.0f, 0.0f};
}

    Vector2 Vector2::one() {
        return {1.0f, 1.0f};
    }

    Vector2 Vector2::up() {
        return {0.0f, 1.0f};
    }

    Vector2 Vector2::down() {
        return {0.0f, -1.0f};
    }

    Vector2 Vector2::left() {
        return {-1.0f, 0.0f};
    }

    Vector2 Vector2::right() {
        return {1.0f, 0.0f};
    }

    Vector2 Vector2::fromAngle(float angle) {
        return {cosf(angle), sinf(angle)};
    }

    Vector2 Vector2::lerp(const Vector2& a, const Vector2& b, float t) {
        return a + (b - a) * t;
    }

    void Vector2::operator +=(const Vector2& other) {
        x += other.x;
        y += other.y;
    }

    void Vector2::operator -=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
    }    

    void Vector2::operator *=(float scalar) {
        x *= scalar;
        y *= scalar;
    }    

    void Vector2::operator /=(float scalar) {
        x /= scalar;
        y /= scalar;
    }    

    bool Vector2::operator ==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }    

    bool Vector2::operator !=(const Vector2& other) const {
        return x != other.x || y != other.y;
    }    

    float Vector2::length() const {
        return sqrtf(x * x + y * y);
    }    

    float Vector2::lengthSquared() const {
        return x * x + y * y;
    }

    float Vector2::distance(const Vector2& other) const {
        return (*this - other).length();
    }

    float Vector2::distanceSquared(const Vector2& other) const {
        return (*this - other).lengthSquared();
    }

    float Vector2::angle() const {
        return atan2f(y, x);
    }

    float Vector2::angle(const Vector2& other) const {
        return atan2f(y - other.y, x - other.x);
    }

    Vector2 Vector2::rotate(float angle) const {
        return {x * cosf(angle) - y * sinf(angle), x * sinf(angle) + y * cosf(angle)};
    }


Vector2 Vector2::reflect(const Vector2 &normal)
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

Vector2 Vector2::normalized()
{
    float length = sqrtf(x * x + y * y);
    return { x / length, y / length };
}

int countNumCollisions(Range<PointMass> collisionShape, PointMass point, float outX) {
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

void findClosestLineSegmentToPoint(Range<PointMass> collisionShape, PointMass point, int& minIndex, Vector2& minPoint, float& minT) {
    float minDistanceSquared = __FLT_MAX__;

    for (int i = 0; i < collisionShape.size; i++)
    {
        Vector2 segment0 = collisionShape[i].pos;
        Vector2 segment1 = collisionShape[(i + 1) % collisionShape.size].pos;

        // Vector from A to B
        Vector2 segment = segment1 - segment0;
        // Vector from A to P
        Vector2 segmentToPoint = point.pos - segment0;

        // The projection of point P onto the line defined by segment AB is given by:
        // v dot w / v dot v
        // Compute projection t
        float t = segmentToPoint.dot(segment) / segment.dot();
        t = clamp(t, 0.0f, 1.0f);

        Vector2 closestPoint = segment0 + segment * t;
        Vector2 pointToClosestPoint = closestPoint - point.pos;
        float dot = segmentToPoint.dot(point.velocity);
        if (dot >= 0.0f)
        {
            continue;
        }

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

bool isPointOutsideShape(PointMass& point, const ShapeBoundingBox& box, Range<PointMass> shape) {
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
    const ShapeBoundingBox& collisionBox,
    const ShapeBoundingBox& movingBox,
    Range<int> &collisionCounterForPoints1,
    Range<int> &collisionCounterForPoints2)
{
    int numCollisions = 0;

    for (int i = 0; i < movingShape.size; i++) {
        PointMass& point = movingShape[i];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(point, collisionBox, collisionShape)) {
            continue;
        }

        numCollisions++;
        collisionCounterForPoints1[i]++;
        collisionCounterForPoints2[i]++;
        
        int minIndex = -1;
        float minT = 0.0f;
        Vector2 minPoint = {0.0f, 0.0f};
        findClosestLineSegmentToPoint(collisionShape, point, minIndex, minPoint, minT);

        Vector2 p0 = collisionShape[minIndex].pos;
        Vector2 p1 = collisionShape[(minIndex + 1) % collisionShape.size].pos;

        Vector2 segmentDirection(p1.x - p0.x, p1.y - p0.y);
        Vector2 segmentNormal = Vector2(-segmentDirection.y, segmentDirection.x);
        Vector2 segmentNormalUnit = segmentNormal.normalized();

        float factor = 0.5f;

        float avgMass = (collisionShape[minIndex].mass + collisionShape[(minIndex + 1) % collisionShape.size].mass) / 2.0f;
        Vector2 avgVelocity(
            (collisionShape[minIndex].velocity.x + collisionShape[(minIndex + 1) % collisionShape.size].velocity.x) / 2.0f,
            (collisionShape[minIndex].velocity.y + collisionShape[(minIndex + 1) % collisionShape.size].velocity.y) / 2.0f
        );

        float massSum = point.mass + avgMass;
        float u1 = point.mass / (massSum);

        p0 -= Vector2((u1 * segmentNormal.x) * minT, (u1 * segmentNormal.y) * minT);
        p1 -= Vector2((u1 * segmentNormal.x) * (1.0f - minT), (u1 * segmentNormal.y) * (1.0f - minT));

        Vector2 newSegmentDirection = Vector2(p1.x - p0.x, p1.y - p0.y);
        Vector2 newSegmentNormal = Vector2(-newSegmentDirection.y, newSegmentDirection.x);
        Vector2 newMinPosition = p0 + newSegmentDirection * minT;
        
        point.pos = newMinPosition - segmentNormalUnit * 0.01f;

        Vector2 relativeVelocity(point.velocity.x - avgVelocity.x, point.velocity.y - avgVelocity.y);
        Vector2 collisionNormal = Vector2(point.pos.x - minPoint.x, point.pos.y - minPoint.y).normalized();
        
        float velocityAlongNormal = relativeVelocity.x * collisionNormal.x + relativeVelocity.y * collisionNormal.y;
        
        // Coefficient of restitution for elastic collision
        float impulseScalar = -(1.0f + coefficentOfRestitution) * velocityAlongNormal;
        impulseScalar /= (1.0f / avgMass + 1 / point.mass);
        
        Vector2 impulse = collisionNormal * impulseScalar;
        point.velocity += impulse * (1 / point.mass);

        collisionShape[minIndex].velocity -= impulse * (1 / avgMass);
        collisionShape[minIndex].pos = p0;
        collisionShape[(minIndex + 1) % collisionShape.size].velocity -= impulse * (1 / avgMass);
        collisionShape[(minIndex + 1) % collisionShape.size].pos = p1;
    }

    return numCollisions;
}

int calculateStaticCollisions(
    Range<PointMass> staticShape,
    Range<PointMass> movingShape,
    const ShapeBoundingBox& staticBox,
    const ShapeBoundingBox& movingBox,
    Range<int> &collisionCounterForMovingShape)
{
    int numCollisions = 0;

    for (int i = 0; i < movingShape.size; i++) {
        PointMass& point = movingShape[i];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(point, staticBox, staticShape)) {
            continue;
        }

        numCollisions++;
        collisionCounterForMovingShape[i]++;
        
        int minIndex = -1;
        float minT = 0.0f;
        Vector2 minPoint = {0.0f, 0.0f};
        findClosestLineSegmentToPoint(staticShape, point, minIndex, minPoint, minT);

        Vector2 p0 = staticShape[minIndex].pos;
        Vector2 p1 = staticShape[(minIndex + 1) % staticShape.size].pos;

        Vector2 segmentDirection(p1.x - p0.x, p1.y - p0.y);
        Vector2 segmentNormal = Vector2(-segmentDirection.y, segmentDirection.x);
        Vector2 segmentNormalUnit = segmentNormal.normalized();

        float factor = 0.5f;

        Vector2 newPosition = p0 + segmentDirection * minT;
        point.pos = newPosition - segmentNormalUnit * 0.01f;

        Vector2 collisionNormal = Vector2(point.pos.x - minPoint.x, point.pos.y - minPoint.y).normalized();
        float velocityAlongNormal = point.velocity.x * collisionNormal.x + point.velocity.y * collisionNormal.y;
        
        // Coefficient of restitution for elastic collision
        float impulseScalar = -(1.0f + coefficentOfRestitution) * velocityAlongNormal;
        
        Vector2 impulse = collisionNormal * impulseScalar;
        point.velocity += impulse * (1 / point.mass);
    }

    return numCollisions;
}

void calculateBoundingBoxes(Array<ShapeBoundingBox>& boundingBoxes, const Array<Shape>& shapes, const Array<PointMass>& points) {
    boundingBoxes.clear();

    for (int i = 0; i < shapes.size(); i++) {    
        const Range<PointMass>& range = points.range(shapes[i]);
        float minX = range[0].pos.x;
        float maxX = range[0].pos.x;
        float minY = range[0].pos.y;
        float maxY = range[0].pos.y;

        for (int j = 0; j < range.size; j++) {
            const PointMass& point = range[j];
            minX = min(minX, point.pos.x);
            maxX = max(maxX, point.pos.x);
            minY = min(minY, point.pos.y);
            maxY = max(maxY, point.pos.y);            
        }

        boundingBoxes.push({i, minX, minY, maxX, maxY });
    }
}

void applyGravity(Range<PointMass>& points) {
    for (int i = 0; i < points.size; i++)
    {
        points[i].force.y += 0.5f * points[i].mass;
    }
}

void applySprings(Range<PointMass>& points, Range<Spring>& springs, const Range<int>& counterForCollisions) {
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
        point1.force += offsetNormal * (springForce + dampForce);
        point2.force -= offsetNormal * (springForce + dampForce);

        // Vector2 correctionDirection = offset.normalized();
        // if (fabs(delta) > 0) {
        //     printf("Delta: %f\n", delta);
        // }
        // float adjustAmount = delta * elapsedTimeMilliseconds * 0.05;

        // if (counterForCollisions[spring.pointA] == 0) {
        //     if (counterForCollisions[spring.pointB] == 0) {
        //         point1.pos += correctionDirection * adjustAmount;
        //         point2.pos -= correctionDirection * adjustAmount;
        //     } else {
        //         point1.pos += correctionDirection * adjustAmount * 2;
        //     }
        // } else if (counterForCollisions[spring.pointB] == 0) {
        //     point2.pos -= correctionDirection * adjustAmount * 2;
        // } else {
        //     point1.pos += correctionDirection * adjustAmount;
        //     point2.pos -= correctionDirection * adjustAmount;
        // }

        // printf("***********\n");
        // printf("Delta: %f - %f = %f\n", offset.length(), spring.length, delta);
        // printf("Adjust amt: %f\n", adjustAmount);

        // // Calculate spring force
        // float springForce = (offset.length() - spring.length) * spring.stiffness;

        // // Calculate damping force based on relative velocity
        // Vector2 relativeVelocity = point2.velocity - point1.velocity;
        // float dampingForce = -damping * relativeVelocity.dot(correctionDirection);

        // // Combine spring and damping forces
        // float totalForce = springForce + dampingForce;

        // point1.velocity += correctionDirection * totalForce * 0.001;
        // point2.velocity -= correctionDirection * totalForce * 0.001;
        // point1.velocity *= (1.0f - damping);
        // point2.velocity *= (1.0f - damping);
    }
}