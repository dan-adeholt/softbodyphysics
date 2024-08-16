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

void shapeMatchAlignInit(PointMassesRange points, Shape &shape)
{

    if (shape.subShapes.isValid())
    {
        for (int i = 0; i < shape.subShapes.size; i++)
        {
            ShapeQuad &subShape = shape.subShapes[i];
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

void shapeMatchAlign(PointMassesRange points, Array<Shape> &shapes)
{
    for (int i = 0; i < shapes.size(); i++)
    {
        Shape &shape = shapes[i];

        if (shape.subShapes.isValid())
        {
            for (int j = 0; j < shape.subShapes.size; j++)
            {
                ShapeQuad &subShape = shape.subShapes[j];
                Vector2 center;

                for (int k = 0; k < subShape.size; k++)
                {
                    center += points.pos[subShape.indices[k]];
                }

                center /= subShape.size;
                float avgDiffAngle = 0.0f;

                for (int k = 0; k < subShape.size; k++)
                {
                    int index = subShape.indices[k];
                    Vector2 translatedPos = points.pos[index] - center;
                    float angleDiff = subShape.originalPos[k].angle(translatedPos);
                    avgDiffAngle += angleDiff;
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

void applySpringDerivatives(Range<Shape> &shapes, PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives, bool enableShapeMatching)
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

        if (shape.subShapes.isValid())
        {
            for (int j = 0; j < shape.subShapes.size; j++)
            {
                const ShapeQuad &subShape = shape.subShapes[j];
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
            for (int j = shape.start; j < shape.end; j++)
            {
                PointDerivative &derivative = derivatives[j];
                Vector2 force = (points.shapePos[j] - points.pos[j]) * 0.00004f;
                derivative.acceleration += force / points.mass[j];
            }
        }
    }
}

void RK4Integrator::prepareRK4Step(PointMassesRange &initialState, Range<Spring> &springs, float dt, Array<PointDerivative> &derivatives, Array<PointDerivative> &outDerivatives, bool gravityEnabled, ConsoleProfileInfo &profileInfo)
{
    if (rkTemp.pos.size() != initialState.size())
    {
        rkTemp.pos.fill(Vector2(), initialState.size());
        rkTemp.velocity.fill(Vector2(), initialState.size());
    }
    rkTemp.mass.replace(initialState.mass);
    rkTemp.shapePos.replace(initialState.shapePos);
    rkTemp.acceleration.fill(Vector2(), initialState.size());
    PointDerivative outDerivative;

    if (gravityEnabled)
    {
        outDerivative.acceleration = Vector2(0.0f, 0.00015f); // Gravity
    }
    outDerivatives.fill(outDerivative, initialState.size());

    Vector2 *posOut = &rkTemp.pos[0];
    Vector2 *velOut = &rkTemp.velocity[0];
    PointDerivative *outDerivativeOut = &outDerivatives[0];

    Task tasks[Scheduler::numTasks];
    int batchSize = initialState.size() / Scheduler::numTasks;
    int curStart = 0;

    PointDerivative *inDerivative = &derivatives[0];
    Vector2 *posIn = &initialState.pos[0];
    Vector2 *posOutEnd = posOut + initialState.size();
    Vector2 *velIn = &initialState.velocity[0];

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

void RK4Integrator::updateRK4Springs(Range<Shape> shapeRange, Range<Spring> &springs, Array<PointDerivative> &outDerivatives, bool enableShapeMatching, ConsoleProfileInfo &profileInfo)
{
    PointMassesRange pointsRange = rkTemp.range();
    Range<PointDerivative> derivativeRange = outDerivatives.range();

    performThreadedSpringDerivatives(shapeRange, pointsRange, springs, derivativeRange, enableShapeMatching, profileInfo);
}

struct SpringJobData
{
    Range<Shape> shapes;
    Range<Spring> springs;
    PointMassesRange points;
    Range<PointDerivative> derivatives;
    bool enableShapeMatching;
};

void springJob(void *data)
{
    SpringJobData *jobData = (SpringJobData *)data;
    applySpringDerivatives(jobData->shapes, jobData->points, jobData->springs, jobData->derivatives, jobData->enableShapeMatching);
}

void RK4Integrator::performThreadedSpringDerivatives(Range<Shape> shapeRange, PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives, bool enableShapeMatching, ConsoleProfileInfo &profileInfo)
{
    Timer springsTimer;
    SpringJobData springRanges[Scheduler::numTasks];
    Task tasks[Scheduler::numTasks];
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

        springRanges[i] = {
            shapeRange.slice(startShapeIndex, curShapeIndex - startShapeIndex + 1),
            springs.slice(curStart, curEnd),
            points,
            derivatives,
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

void RK4Integrator::performRK4Integration(Range<Shape> shapeRange, PointMassesRange &points, Range<Spring> &springs, Range<int> &collisionCounterForPoints, bool gravityEnabled, bool updateCollisions, bool enableShapeMatching, ConsoleProfileInfo &profileInfo)
{
    if (rkEmptyDerivatives.size() != points.size())
    {
        rkEmptyDerivatives.fill(PointDerivative(), points.size());
    }

    prepareRK4Step(points, springs, 0.0, rkEmptyDerivatives, rk1, gravityEnabled, profileInfo);
    updateRK4Springs(shapeRange, springs, rk1, enableShapeMatching, profileInfo);
    prepareRK4Step(points, springs, physicsStep * 0.5, rk1, rk2, gravityEnabled, profileInfo);
    updateRK4Springs(shapeRange, springs, rk2, enableShapeMatching, profileInfo);
    prepareRK4Step(points, springs, physicsStep * 0.5, rk2, rk3, gravityEnabled, profileInfo);
    updateRK4Springs(shapeRange, springs, rk3, enableShapeMatching, profileInfo);
    prepareRK4Step(points, springs, physicsStep, rk3, rk4, gravityEnabled, profileInfo);
    updateRK4Springs(shapeRange, springs, rk4, enableShapeMatching, profileInfo);

    float factor = (1.0f / 6.0f) * physicsStep;

    for (int i = 0; i < points.size(); i++)
    {
        PointDerivative rk1d = rk1[i];
        PointDerivative rk2d = rk2[i];
        PointDerivative rk3d = rk3[i];
        PointDerivative rk4d = rk4[i];

        Vector2 deltaVelocity = (rk1d.velocity + (rk2d.velocity + rk3d.velocity) * 2.0f + rk4d.velocity) * factor;
        Vector2 deltaAcceleration = (rk1d.acceleration + (rk2d.acceleration + rk3d.acceleration) * 2.0f + rk4d.acceleration) * factor;
        points.pos[i] += deltaVelocity;
        points.velocity[i] += deltaAcceleration;
    }
}

void RK4Integrator::testRK4Performance(int iterations, Range<Shape> shapes, PointMassesRange points, Range<Spring> springs)
{
    rkEmptyDerivatives.fill(PointDerivative(), points.size());
    ConsoleProfileInfo profileInfo;
    Array<int> collisionCounterForPoints;
    collisionCounterForPoints.fill(0, points.size());
    Range<int> collisionCounterForPointsRange = collisionCounterForPoints.range();

    for (int i = 0; i < iterations; i++)
    {
        performRK4Integration(shapes, points, springs, collisionCounterForPointsRange, true, false, true, profileInfo);
    }
}

void RK4Integrator::testRK4PreparePerformance(int iterations, PointMassesRange points, Range<Spring> springs)
{
    ConsoleProfileInfo profileInfo;
    rkEmptyDerivatives.fill(PointDerivative(), points.size());

    for (int i = 0; i < iterations; i++)
    {
        prepareRK4Step(points, springs, 0.0, rkEmptyDerivatives, rk1, true, profileInfo);
    }
}

void RK4Integrator::testSpringPerformance(int iterations, Range<Shape> shapes, PointMassesRange points, Range<Spring> springs)
{
    ConsoleProfileInfo profileInfo;
    rkEmptyDerivatives.fill(PointDerivative(), points.size());

    prepareRK4Step(points, springs, 0.0, rkEmptyDerivatives, rk1, true, profileInfo);
    auto derivativeRange = rk1.range();
    for (int i = 0; i < iterations; i++)
    {
        performThreadedSpringDerivatives(shapes, points, springs, derivativeRange, true, profileInfo);
    }
}
