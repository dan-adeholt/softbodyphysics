#include "Physics.h"
#include "../containers/Range.h"
#include "../containers/Array.h"
#include "../utils/MinMax.h"
#include "../utils/Console.h"
#include "../tasks/Scheduler.h"
#include "../timer.h"
#include <stdio.h>
#include <math.h>

const float physicsStep = 1.0f;

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

float direction(const Vector2 &p1, const Vector2 &p2, const Vector2 &p3)
{
    return (p3.x - p1.x) * (p2.y - p1.y) - (p2.x - p1.x) * (p3.y - p1.y);
}

bool onSegment(const Vector2 &p1, const Vector2 &p2, const Vector2 &p)
{
    return (min(p1.x, p2.x) <= p.x && p.x <= max(p1.x, p2.x) &&
            min(p1.y, p2.y) <= p.y && p.y <= max(p1.y, p2.y));
}

bool lineSegmentsIntersect(const Vector2 &p1, const Vector2 &p2,
                           const Vector2 &p3, const Vector2 &p4)
{
    float d1 = direction(p3, p4, p1);
    float d2 = direction(p3, p4, p2);
    float d3 = direction(p1, p2, p3);
    float d4 = direction(p1, p2, p4);

    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
        return true;
    if (d1 == 0 && onSegment(p3, p4, p1))
        return true;
    if (d2 == 0 && onSegment(p3, p4, p2))
        return true;
    if (d3 == 0 && onSegment(p1, p2, p3))
        return true;
    if (d4 == 0 && onSegment(p1, p2, p4))
        return true;
    return false;
}

bool shapesOverlap(const PointMassesRange &poly1, const PointMassesRange &poly2)
{
    for (int i = 0; i < poly1.size(); ++i)
    {
        for (int j = 0; j < poly2.size(); ++j)
        {
            if (lineSegmentsIntersect(poly1.pos[i], poly1.pos[(i + 1) % poly1.size()],
                                      poly2.pos[j], poly2.pos[(j + 1) % poly2.size()]))
                return true;
        }
    }
    return false;
}

bool pointInShape(const Vector2 &point, const PointMassesRange &shape)
{
    bool inside = false;
    int j = shape.size() - 1;

    for (int i = 0; i < shape.size(); i++)
    {
        if ((shape.pos[i].y > point.y) != (shape.pos[j].y > point.y) &&
            point.x < (shape.pos[j].x - shape.pos[i].x) * (point.y - shape.pos[i].y) /
                              (shape.pos[j].y - shape.pos[i].y) +
                          shape.pos[i].x)
        {
            inside = !inside;
        }
        j = i;
    }

    return inside;
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
    PointMassesRange movingShapePrevPos,
    const ShapeBoundingBox &collisionBox,
    const ShapeBoundingBox &movingBox,
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

void shapeMatchAlignInit(PointMassesRange points, Shape &shape, Range<ShapeQuad> partialShapes)
{
    if (shape.subShapeSpan.isValid())
    {
        Range<ShapeQuad> subShapes = partialShapes.slice(shape.subShapeSpan);
        for (int i = 0; i < subShapes.size; i++)
        {
            ShapeQuad &subShape = subShapes[i];
            Vector2 center;

            for (int i = 0; i < subShape.size; i++)
            {
                center += subShape.originalPos[i];
            }

            center /= subShape.size;

            for (int i = 0; i < subShape.size; i++)
            {
                subShape.originalPos[i] -= center;
                subShape.shapePos[i] -= center;
            }
        }
    }
    else
    {
        Vector2 center;
        for (int j = shape.start; j < shape.end; j++)
        {
            center += points.shapeOriginalPos[j];
        }

        int numPoints = shape.end - shape.start;
        center /= numPoints;

        for (int j = shape.start; j < shape.end; j++)
        {
            points.shapeOriginalPos[j] -= center;
        }
    }
}

void shapeMatchAlign(PointMassesRange points, Array<Shape> &shapes, Range<ShapeQuad> partialShapes, int draggingShapeIndex)
{
    for (int i = 0; i < shapes.size(); i++)
    {
        Shape &shape = shapes[i];
        if (i == draggingShapeIndex)
        {
            continue;
        }

        if (shape.subShapeSpan.isValid())
        {
            Range<ShapeQuad> subShapes = partialShapes.slice(shape.subShapeSpan);
            for (int j = 0; j < subShapes.size; j++)
            {
                ShapeQuad &subShape = subShapes[j];
                Vector2 center;

                for (int k = 0; k < subShape.size; k++)
                {
                    center += points.pos[subShape.indices[k]];
                }

                center /= subShape.size;
                float avgDiffAngle = 0.0f;

                for (int k = 0; k < subShape.size; k++)
                {
                    Vector2 translatedPos = points.pos[subShape.indices[k]] - center;
                    avgDiffAngle += subShape.originalPos[k].angle(translatedPos);
                }

                avgDiffAngle /= subShape.size;

                for (int k = 0; k < subShape.size; k++)
                {
                    subShape.shapePos[k] = subShape.originalPos[k].rotate(avgDiffAngle) + center;
                }
            }
        }
        else
        {
            Vector2 center;

            for (int j = shape.start; j < shape.end; j++)
            {
                center += points.pos[j];
            }

            int numPoints = shape.end - shape.start;

            center /= numPoints;

            float avgDiffAngle = 0.0f;

            for (int j = shape.start; j < shape.end; j++)
            {
                Vector2 translatedPos = points.pos[j] - center;
                float angleDiff = points.shapeOriginalPos[j].angle(translatedPos);
                avgDiffAngle += angleDiff;
            }

            avgDiffAngle /= numPoints;

            for (int j = shape.start; j < shape.end; j++)
            {
                points.shapePos[j] = points.shapeOriginalPos[j].rotate(avgDiffAngle) + center;
            }
        }
    }
}

int calculateStaticCollisions(
    PointMassesRange staticShape,
    PointMassesRange movingShape,
    PointMassesRange movingShapePrevPos,
    const ShapeBoundingBox &staticBox,
    const ShapeBoundingBox &movingBox,
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

void applySpringDerivatives(Range<Shape> &shapes, PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives, Range<ShapeQuad> partialShapes, bool enableShapeMatching)
{
    for (int i = 0; i < springs.size; i++)
    {
        const Spring &spring = springs[i];

        Vector2 p0(points.pos[spring.pointA]);
        Vector2 p1(points.pos[spring.pointB]);
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

            PointDerivative &derivative1 = derivatives[spring.pointA];
            PointDerivative &derivative2 = derivatives[spring.pointB];

            derivative1.acceleration += force / p1Mass;
            derivative2.acceleration -= force / p2Mass;
        }
    }

    if (!enableShapeMatching)
    {
        return;
    }

    for (int i = 0; i < shapes.size; i++)
    {
        const Shape &shape = shapes[i];

        if (shape.subShapeSpan.isValid())
        {
            Range<ShapeQuad> subShapes = partialShapes.slice(shape.subShapeSpan);
            for (int j = 0; j < subShapes.size; j++)
            {
                const ShapeQuad &subShape = subShapes[j];
                for (int k = 0; k < subShape.size; k++)
                {
                    int index = subShape.indices[k];
                    PointDerivative &derivative = derivatives[index];
                    Vector2 force = (subShape.shapePos[k] - points.pos[index]) * 0.00004f;
                    derivative.acceleration += force / points.mass[index];
                }
            }
        }
        else
        {
            Vector2 avgVelocity;

            for (int j = shape.start; j < shape.end; j++)
            {
                avgVelocity += points.velocity[j];
            }

            avgVelocity /= (shape.end - shape.start);

            for (int j = shape.start; j < shape.end; j++)
            {
                PointDerivative &derivative = derivatives[j];
                float springStiffness = 0.1250f;
                float springDamping = 800.9f;

                Vector2 p0(points.shapePos[j]);
                Vector2 p1(points.pos[j]);
                Vector2 direction = p0 - p1;
                float offsetLength = direction.length();

                if (offsetLength > 0.001f)
                {
                    Vector2 directionNormalized = direction.normalized();

                    Vector2 velocityAlongSpringAxis = directionNormalized * (points.velocity[j] - avgVelocity).dot(directionNormalized);
                    Vector2 force = (p0 - p1) * 0.00005f;
                    Vector2 acceleration = force / points.mass[j];
                    // Console::log("Accel %f %f", acceleration.x, acceleration.y);

                    derivative.acceleration += acceleration;
                    derivative.acceleration += (velocityAlongSpringAxis * -0.0025f) / points.mass[j];
                    // derivative.acceleration -= (points.velocity[j] - avgVelocity) * 0.000025f;
                }
            }
        }
    }
}

void RK4Integrator::prepareRK4Step(PhysicsSpace &space, float dt, Array<PointDerivative> &derivatives, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo)
{
    if (rkTemp.pos.size() != space.points.size())
    {
        rkTemp.pos.fill(Vector2(), space.points.size());
        rkTemp.velocity.fill(Vector2(), space.points.size());
    }
    rkTemp.mass.replace(space.points.mass);
    rkTemp.shapePos.replace(space.points.shapePos);
    rkTemp.acceleration.fill(Vector2(), space.points.size());
    PointDerivative outDerivative;

    if (space.gravityEnabled)
    {
        outDerivative.acceleration = Vector2(0.0f, 0.00015f); // Gravity
    }
    outDerivatives.fill(outDerivative, space.points.size());

    Vector2 *posOut = &rkTemp.pos[0];
    Vector2 *velOut = &rkTemp.velocity[0];
    PointDerivative *outDerivativeOut = &outDerivatives[0];

    PointDerivative *inDerivative = &derivatives[0];
    Vector2 *posIn = &space.points.pos[0];
    Vector2 *posOutEnd = posOut + space.points.size();
    Vector2 *velIn = &space.points.velocity[0];

    while (posOut != posOutEnd)
    {
        PointDerivative derivative = *inDerivative++;
        Vector2 originalVelocity = *velIn++;
        Vector2 originalPos = *posIn++;
        outDerivativeOut->velocity = originalVelocity;
        outDerivativeOut++;
        *posOut++ = originalPos + derivative.velocity * dt;
        *velOut++ = originalVelocity + derivative.acceleration * dt;
    }
}

void RK4Integrator::updateRK4Springs(PhysicsSpace &space, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo)
{
    PointMassesRange pointsRange = rkTemp.range();
    Range<PointDerivative> derivativeRange = outDerivatives.range();

    if (!space.springsEnabled)
    {
        return;
    }

    performThreadedSpringDerivatives(space.shapes.range(), pointsRange, space.springs.range(), derivativeRange, space.partialShapes.range(), space.shapeMatchingEnabled, profileInfo);

    if (space.draggingShapeIndex != -1)
    {
        Shape &shape = space.shapes[space.draggingShapeIndex];

        for (int i = shape.start; i < shape.end; i++)
        {
            PointDerivative &derivative = outDerivatives[i];
            derivative.acceleration += space.points.velocity[i] * -0.008f;
        }
    }
}

struct SpringJobData
{
    Range<Shape> shapes;
    Range<Spring> springs;
    PointMassesRange points;
    Range<PointDerivative> derivatives;
    Range<ShapeQuad> partialShapes;
    bool enableShapeMatching;
};

void springJob(void *data)
{
    SpringJobData *jobData = (SpringJobData *)data;
    applySpringDerivatives(jobData->shapes, jobData->points, jobData->springs, jobData->derivatives, jobData->partialShapes, jobData->enableShapeMatching);
}

void RK4Integrator::performThreadedSpringDerivatives(Range<Shape> shapeRange, PointMassesRange points, Range<Spring> springs, Range<PointDerivative> derivatives, Range<ShapeQuad> partialShapes, bool enableShapeMatching, ConsoleProfileInfo &profileInfo)
{
    Timer springsTimer;
    SpringJobData springRanges[Scheduler::maxNumThreads];
    Task tasks[Scheduler::maxNumThreads];
    int batchSize = springs.size / Scheduler::numTasks;

    int curStart = 0;

    int numThreads = Scheduler::numTasks;

    // Ensure that no job chunks span across the same shape, because that would
    // cause point mass calculations from different threads to interfere with each other
    for (int i = 0; i < Scheduler::numTasks; i++)
    {
        int curEnd = min(curStart + batchSize, springs.size);
        int startShapeIndex = springs[curStart].shapeIndex;
        int curShapeIndex = springs[curEnd - 1].shapeIndex;

        while (curEnd < springs.size && springs[curEnd].shapeIndex == curShapeIndex)
        {
            curEnd++;
        }

        int endShapeIndex = springs[curEnd - 1].shapeIndex + 1;

        springRanges[i] = {
            shapeRange.slice(startShapeIndex, endShapeIndex),
            springs.slice(curStart, curEnd),
            points,
            derivatives,
            partialShapes,
            enableShapeMatching};

        tasks[i].function = springJob;
        tasks[i].data = &springRanges[i];

        curStart = curEnd;
        if (curStart == springs.size)
        {
            numThreads = i + 1;
            break;
        }
    }

    Scheduler::instance->schedule(tasks, numThreads);

    profileInfo.springsTimeMillis += springsTimer.elapsedMillis();
}

void RK4Integrator::performRK4Integration(PhysicsSpace &space, ConsoleProfileInfo &profileInfo)
{
    if (rkEmptyDerivatives.size() != space.points.size())
    {
        rkEmptyDerivatives.fill(PointDerivative(), space.points.size());
    }

    prepareRK4Step(space, 0.0, rkEmptyDerivatives, rk1, profileInfo);
    updateRK4Springs(space, rk1, profileInfo);
    prepareRK4Step(space, physicsStep * 0.5, rk1, rk2, profileInfo);
    updateRK4Springs(space, rk2, profileInfo);
    prepareRK4Step(space, physicsStep * 0.5, rk2, rk3, profileInfo);
    updateRK4Springs(space, rk3, profileInfo);
    prepareRK4Step(space, physicsStep, rk3, rk4, profileInfo);
    updateRK4Springs(space, rk4, profileInfo);

    float factor = (1.0f / 6.0f) * physicsStep;

    for (int i = 0; i < space.points.size(); i++)
    {
        PointDerivative rk1d = rk1[i];
        PointDerivative rk2d = rk2[i];
        PointDerivative rk3d = rk3[i];
        PointDerivative rk4d = rk4[i];

        Vector2 deltaVelocity = (rk1d.velocity + (rk2d.velocity + rk3d.velocity) * 2.0f + rk4d.velocity) * factor;
        Vector2 deltaAcceleration = (rk1d.acceleration + (rk2d.acceleration + rk3d.acceleration) * 2.0f + rk4d.acceleration) * factor;
        space.points.pos[i] += deltaVelocity;
        space.points.velocity[i] += deltaAcceleration;

        float velocityAmplitude = space.points.velocity[i].length();

        if (velocityAmplitude > 2.0f)
        {
            space.points.velocity[i] *= 2.0f / velocityAmplitude;
        }
    }
}

void RK4Integrator::testRK4Performance(int iterations, PhysicsSpace &space)
{
    rkEmptyDerivatives.fill(PointDerivative(), space.points.size());
    ConsoleProfileInfo profileInfo;

    for (int i = 0; i < iterations; i++)
    {
        performRK4Integration(space, profileInfo);
    }
}

void RK4Integrator::testRK4PreparePerformance(int iterations, PhysicsSpace &space)
{
    ConsoleProfileInfo profileInfo;
    rkEmptyDerivatives.fill(PointDerivative(), space.points.size());

    for (int i = 0; i < iterations; i++)
    {
        prepareRK4Step(space, 0.0, rkEmptyDerivatives, rk1, profileInfo);
    }
}

void RK4Integrator::testSpringPerformance(int iterations, PhysicsSpace &space)
{
    ConsoleProfileInfo profileInfo;
    rkEmptyDerivatives.fill(PointDerivative(), space.points.size());

    prepareRK4Step(space, 0.0, rkEmptyDerivatives, rk1, profileInfo);
    auto derivativeRange = rk1.range();
    for (int i = 0; i < iterations; i++)
    {
        performThreadedSpringDerivatives(space.shapes.range(), space.points.range(), space.springs.range(), derivativeRange, space.partialShapes.range(), space.shapeMatchingEnabled, profileInfo);
    }
}

#define NUM_SHAPES 40
#define NUM_POINTS 1024

void sortBoundingBoxes(Array<ShapeBoundingBox> &sortedBoundingBoxes)
{
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    for (int i = 1; i < sortedBoundingBoxes.size(); i++)
    {
        ShapeBoundingBox item = sortedBoundingBoxes[i];
        int j = i - 1;
        while (j >= 0 && sortedBoundingBoxes[j].x1 > item.x1)
        {
            sortedBoundingBoxes[j + 1] = sortedBoundingBoxes[j];
            j--;
        }
        sortedBoundingBoxes[j + 1] = item;
    }
}

void updateSortedBoundingBoxes(Array<ShapeBoundingBox> &sortedBoundingBoxes, Array<ShapeBoundingBox> &boundingBoxes)
{
    for (int i = 0; i < sortedBoundingBoxes.size(); i++)
    {
        ShapeBoundingBox &sortedBox = sortedBoundingBoxes[i];
        const ShapeBoundingBox &box = boundingBoxes[sortedBox.shapeIndex];
        sortedBox.x1 = box.x1;
        sortedBox.y1 = box.y1;
        sortedBox.x2 = box.x2;
        sortedBox.y2 = box.y2;
    }
}

PhysicsSpace::PhysicsSpace() : gravityEnabled(true), collisionsEnabled(true), shapeMatchingEnabled(true), springsEnabled(true), draggingShapeIndex(-1), draggingSubShapeIndex(-1)
{
    shapes.reserve(NUM_SHAPES);
    partialShapes.reserve(NUM_SHAPES);
    points.reserve(NUM_POINTS);
    staticShapes.reserve(NUM_SHAPES);
    staticPoints.reserve(NUM_POINTS);
    staticJoints.reserve(NUM_POINTS / 2);
    springs.reserve(NUM_POINTS / 2);
    mouseJoint.pointIndex = -1;
    mouseJoint.position = Vector2::zero();
}

void PhysicsSpace::assign(PhysicsSpace &other)
{
    shapes.replace(other.shapes);
    partialShapes.replace(other.partialShapes);
    staticShapes.replace(other.staticShapes);
    points.replace(other.points);
    staticPoints.replace(other.staticPoints);
    springs.replace(other.springs);
    staticJoints.replace(other.staticJoints);
}

int PhysicsSpace::nextShapeIndex() const
{
    return shapes.size();
}

int PhysicsSpace::nextStaticShapeIndex() const
{
    return staticShapes.size();
}

void PhysicsSpace::clear()
{
    shapes.clear();
    partialShapes.clear();
    points.clear();
    staticShapes.clear();
    staticPoints.clear();
    springs.clear();
    staticJoints.clear();
    mouseJoint.pointIndex = -1;
    gravityEnabled = true;
    shapeMatchingEnabled = true;
}

PhysicsCollisionSolver::PhysicsCollisionSolver() : collisionMap(0)
{
}

void PhysicsCollisionSolver::clear()
{
    collisionMap.clear();
    resolvedCollisionPairs.clear();
    boundingBoxes.clear();
    staticBoundingBoxes.clear();
    sortedBoundingBoxes.clear();
    sortedStaticBoundingBoxes.clear();
}

void PhysicsCollisionSolver::handleCollisions(PhysicsSpace &space, PhysicsSpace &prevSpace, float step, ConsoleProfileInfo &profileInfo)
{
    int numElementsWithStatic = space.shapes.size() + space.staticShapes.size();

    for (int i = 0; i < resolvedCollisionPairs.size(); i++)
    {
        CollisionPair &pair = resolvedCollisionPairs[i];
        const Shape &shape1 = space.shapes[pair.shape1Index];

        PointMassesRange range1 = space.points.range(shape1);
        PointMassesRange range2;
        if (pair.shape2Index >= space.shapes.size())
        {
            range2 = space.staticPoints.range(space.staticShapes[pair.shape2Index - space.shapes.size()]);
        }
        else
        {
            const Shape &shape2 = space.shapes[pair.shape2Index];
            range2 = space.points.range(shape2);
        }

        if (!shapesOverlap(range1, range2))
        {
            collisionMap.resetCollision(pair.shape1Index, pair.shape2Index);
        }
    }

    resolvedCollisionPairs.clear();

    if (collisionMap.numElements != numElementsWithStatic)
    {
        collisionMap.resize(numElementsWithStatic);
    }

    if (boundingBoxes.capacity() == 0)
    {
        boundingBoxes.reserve(space.shapes.size());
        staticBoundingBoxes.reserve(space.staticShapes.size());
        sortedBoundingBoxes.reserve(space.shapes.size());
        sortedStaticBoundingBoxes.reserve(space.staticShapes.size());
    }

    for (int i = 0; i < space.points.size(); i++)
    {
        space.points.acceleration[i] = Vector2();
    }

    Timer collisionsTimer;

    // For a broad phase collision detection, sort using insertion sort along a single axis

    {
        Timer boundingBoxTimer;
        calculateBoundingBoxes(boundingBoxes, space.shapes, space.points);
        calculateBoundingBoxes(staticBoundingBoxes, space.staticShapes, space.staticPoints);

        if (sortedBoundingBoxes.size() == 0)
        {
            for (int i = 0; i < boundingBoxes.size(); i++)
            {
                sortedBoundingBoxes.push(boundingBoxes[i]);
            }
        }
        else
        {
            updateSortedBoundingBoxes(sortedBoundingBoxes, boundingBoxes);
        }

        if (sortedStaticBoundingBoxes.size() == 0)
        {
            for (int i = 0; i < staticBoundingBoxes.size(); i++)
            {
                sortedStaticBoundingBoxes.push(staticBoundingBoxes[i]);
            }
        }

        else
        {
            updateSortedBoundingBoxes(sortedStaticBoundingBoxes, staticBoundingBoxes);
        }

        sortBoundingBoxes(sortedBoundingBoxes);
        sortBoundingBoxes(sortedStaticBoundingBoxes);
        profileInfo.boundingBoxTimeMillis = boundingBoxTimer.elapsedMillis();
    }

    profileInfo.numBboxes = sortedBoundingBoxes.size();
    profileInfo.numBbboxChecks = 0;
    int staticIndex = 0;
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < sortedBoundingBoxes.size(); i++)
    {
        const ShapeBoundingBox &box = sortedBoundingBoxes[i];
        const Shape &shape1 = space.shapes[box.shapeIndex];

        for (int j = i + 1; j < sortedBoundingBoxes.size(); j++)
        {
            const ShapeBoundingBox &otherBox = sortedBoundingBoxes[j];
            profileInfo.numBbboxChecks++;

            if (otherBox.x1 < box.x2)
            {
                // Due to the sorted nature of the bounding boxes, we already know that
                // otherBox.x1 >= box.x1, so we only need to check the other axis

                if (otherBox.y1 <= box.y2 && otherBox.y2 >= box.y1)
                {
                    // Check for collision
                    int numCollisions = collisionMap.getCollisionCount(box.shapeIndex, otherBox.shapeIndex);
                    const Shape &shape2 = space.shapes[otherBox.shapeIndex];

                    if (numCollisions < 128)
                    {
                        calculateCollisions(space.points.range(shape1), space.points.range(shape2), prevSpace.points.range(shape2), box, otherBox, step);
                        calculateCollisions(space.points.range(shape2), space.points.range(shape1), prevSpace.points.range(shape1), otherBox, box, step);

                        if (shapesOverlap(space.points.range(shape1), space.points.range(shape2)))
                        {
                            collisionMap.incrementCollision(box.shapeIndex, otherBox.shapeIndex);
                        }
                    }
                    else if (shapesOverlap(space.points.range(shape1), space.points.range(shape2)))
                    {
                        resolvedCollisionPairs.push({box.shapeIndex, otherBox.shapeIndex});
                        PointMassesRange range1 = space.points.range(shape1);
                        PointMassesRange range2 = space.points.range(shape2);
                        if (box.shapeIndex < otherBox.shapeIndex)
                        {
                            swap(range1, range2);
                        }

                        for (int i = 0; i < range1.size(); i++)
                        {
                            range1.acceleration[i] += Vector2(0.0009f, 0.0009f);
                        }

                        for (int i = 0; i < range2.size(); i++)
                        {
                            range2.acceleration[i] -= Vector2(0.0009f, 0.0009f);
                        }
                    }
                    else
                    {
                        collisionMap.resetCollision(box.shapeIndex, otherBox.shapeIndex);
                    }
                }
            }
            else
            {
                break;
            }
        }

        while (staticIndex < sortedStaticBoundingBoxes.size() && sortedStaticBoundingBoxes[staticIndex].x2 < box.x1)
        {
            staticIndex++;
        }

        for (int j = staticIndex; j < sortedStaticBoundingBoxes.size(); j++)
        {
            const ShapeBoundingBox &staticBox = sortedStaticBoundingBoxes[j];
            int staticShapeIndex = staticBox.shapeIndex + space.shapes.size();

            if (staticBox.x1 < box.x2)
            {
                // Due to the sorted nature of the bounding boxes, we already know that
                // staticBox.x1 >= box.x1, so we only need to check the other axis
                if (staticBox.y1 > box.y2 || staticBox.y2 < box.y1)
                {
                    continue;
                }

                const Shape &staticShape = space.staticShapes[staticBox.shapeIndex];

                int collisionCount = collisionMap.getCollisionCount(box.shapeIndex, staticShapeIndex);
                if (collisionCount < 64)
                {
                    calculateStaticCollisions(space.staticPoints.range(staticShape), space.points.range(shape1), prevSpace.points.range(shape1), staticBox, box, step);
                    if (shapesOverlap(space.points.range(shape1), space.staticPoints.range(staticShape)))
                    {
                        collisionMap.incrementCollision(box.shapeIndex, staticShapeIndex);
                    }
                    else
                    {
                        PointMassesRange range1 = space.staticPoints.range(staticShape);
                        PointMassesRange range2 = space.points.range(shape1);
                        collisionMap.resetCollision(box.shapeIndex, staticShapeIndex);
                    }
                }
                else
                {
                    if (shapesOverlap(space.points.range(shape1), space.staticPoints.range(staticShape)))
                    {
                        resolvedCollisionPairs.push({box.shapeIndex, staticShapeIndex});
                        PointMassesRange range1 = space.points.range(shape1);
                        PointMassesRange range2 = space.staticPoints.range(staticShape);
                        Vector2 centerOutsideMovingShape;
                        int numOutside = 0;

                        for (int i = 0; i < range1.size(); i++)
                        {
                            if (!pointInShape(range1.pos[i], range2))
                            {
                                centerOutsideMovingShape += range1.pos[i];
                                numOutside++;
                            }
                        }

                        centerOutsideMovingShape /= numOutside;

                        for (int i = 0; i < range1.size(); i++)
                        {
                            if (pointInShape(range1.pos[i], range2))
                            {
                                Vector2 dirToOutside = centerOutsideMovingShape - range1.pos[i];
                                range1.acceleration[i] += dirToOutside * 0.001f;
                            }
                        }
                    }
                    else
                    {
                        collisionMap.resetCollision(box.shapeIndex, staticShapeIndex);
                    }
                }
            }
            else
            {
                break;
            }
        }
    }

    profileInfo.collisionTimeMillis = collisionsTimer.elapsedMillis();

    for (int i = 0; i < space.points.size(); i++)
    {
        space.points.velocity[i] += (space.points.acceleration[i]) * physicsStep;
        space.points.pos[i] += space.points.velocity[i] * physicsStep;
    }
}

void PhysicsIntegrator::performIntegration(PhysicsSpace &space, ConsoleProfileInfo &profileInfo)
{
    shapeMatchAlign(space.points.range(), space.shapes, space.partialShapes.range(), space.draggingShapeIndex);
    rk4Integrator.performRK4Integration(space, profileInfo);

    for (int i = 0; i < space.staticJoints.size(); i++)
    {
        StaticJoint &joint = space.staticJoints[i];
        space.points.pos[joint.pointIndex] = joint.position;
        space.points.velocity[joint.pointIndex] = Vector2();
    }

    if (space.mouseJoint.pointIndex != -1)
    {
        space.points.pos[space.mouseJoint.pointIndex] = space.mouseJoint.position;
        space.points.velocity[space.mouseJoint.pointIndex] = Vector2();
        space.points.acceleration[space.mouseJoint.pointIndex] = Vector2();
    }
}
