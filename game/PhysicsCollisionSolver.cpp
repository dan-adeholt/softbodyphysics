#include "PhysicsCollisionSolver.h"
#include "../utils/MinMax.h"
#include "../utils/Console.h"
#include "../timer.h"
#include "./PhysicsSpace.h"
#include <math.h>

float coefficentOfRestitution = 0.65f;

inline Vector2 calculateImpulseStatic(float pointVelX, float pointVelY, float pointMass, Vector2 segmentNormal)
{
    // Velocities of static points are zero
    float velocityLineSegmentX = 0.0f;
    float velocityLineSegmentY = 0.0f;

    // Calculate relative velocity
    float relativeVelocityX = pointVelX - velocityLineSegmentX;
    float relativeVelocityY = pointVelY - velocityLineSegmentY;

    // Inverse mass of static points is zero
    float inverseMass = (1.0f / pointMass);

    // Dot product of relative velocity and normal
    float dotProduct = Vector2::vec2dot(relativeVelocityX, relativeVelocityY, segmentNormal.x, segmentNormal.y);

    // Calculate impulse magnitude
    float impulseMagnitude = (-(1.0f + coefficentOfRestitution) * dotProduct) / inverseMass;

    // Return the impulse vector
    return segmentNormal * impulseMagnitude;
}

inline Vector2 calculateImpulse(float pm0VelX, float pm0VelY, float pm0Mass, float pm1VelX, float pm1VelY, float pm1Mass, Vector2 segmentNormal, float pointVelX, float pointVelY, float pointMass, float minT)
{
    float velocityLineSegmentX = pm0VelX + (pm1VelX - pm0VelX) * minT;
    float velocityLineSegmentY = pm0VelY + (pm1VelY - pm0VelY) * minT;
    float relativeVelocityX = pointVelX - velocityLineSegmentX;
    float relativeVelocityY = pointVelY - velocityLineSegmentY;

    float inverseMass = (1.0f / pointMass) + (2.0f / (pm0Mass)) + (2.0f / (pm1Mass));
    float dotProduct = Vector2::vec2dot(relativeVelocityX, relativeVelocityY, segmentNormal.x, segmentNormal.y);

    float impulseMagnitude = (-(1.0f + coefficentOfRestitution) * dotProduct) / inverseMass;

    // impulseMagnitude = clamp(impulseMagnitude, -0.001f, 0.001f);
    return segmentNormal * impulseMagnitude;
}

struct IntersectionResult
{
    Vector2 point;
    float t;
    int segmentIndex;
    bool found;
};

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

float lastMaxAmplitude = 0.0f;
float lastMaxAmplitudeStatic = 0.0f;

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

        // if (fabs(segmentNormal.dot(velocity)) < 0.000001f)
        // {
        //     continue;
        // }

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
        Vector2 impulse = calculateImpulseStatic(velocity.x, velocity.y, pointMass, segmentNormal);

        // float impulseLength = impulse.length();
        // if (impulseLength > lastMaxAmplitudeStatic)
        // {
        //     lastMaxAmplitudeStatic = impulseLength;
        //     Console::log("Last max static impulse: %f", impulseLength);
        // }

        Vector2 reflection = velocity.reflect(segmentNormal).normalized();
        movingShape.velocity[i] += impulse / pointMass;
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

int calculateCollisions(
    PointMassesRange collisionShape,
    PointMassesRange movingShape,
    PointMassesRange movingShapePrevPos,
    const ShapeBoundingBox &collisionBox,
    const ShapeBoundingBox &movingBox,
    int shapeIndex1,
    int shapeIndex2,
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
        Vector2 impulse = calculateImpulse(pm0Vel.x, pm0Vel.y, pm0Mass, pm1Vel.x, pm1Vel.y, pm1Mass, segmentNormal, pointVelocity.x, pointVelocity.y, pointMass, minT);

        movingShape.velocity[i] += impulse / pointMass;

        Vector2 pm0Acceleration = (impulse * (1.0f - minT)) / pm0Mass;
        collisionShape.velocity[minIndex] -= pm0Acceleration;
        Vector2 pm1Acceleration = (impulse * minT) / pm1Mass;
        collisionShape.velocity[(minIndex + 1) % collisionShape.pos.size] -= pm1Acceleration;

        Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();
        Vector2 oldPos = movingShape.pos[i];
        movingShape.pos[i] = minPoint + reflection * 0.1f;
    }

    return numCollisions;
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
void PhysicsCollisionSolver::updateBoundingBoxes(PhysicsSpace &space, ConsoleProfileInfo &profileInfo)
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
}

void PhysicsCollisionSolver::handleCollisions(PhysicsSpace &space, PhysicsSpace &prevSpace, float step, ConsoleProfileInfo &profileInfo)
{
    Timer collisionsTimer;
    updateBoundingBoxes(space, profileInfo);

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
                        calculateCollisions(space.points.range(shape1), space.points.range(shape2), prevSpace.points.range(shape2), box, otherBox, box.shapeIndex, otherBox.shapeIndex, step);
                        calculateCollisions(space.points.range(shape2), space.points.range(shape1), prevSpace.points.range(shape1), otherBox, box, otherBox.shapeIndex, box.shapeIndex, step);

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
                            range1.velocity[i] += Vector2(0.0009f, 0.0009f);
                        }

                        for (int i = 0; i < range2.size(); i++)
                        {
                            range2.velocity[i] -= Vector2(0.0009f, 0.0009f);
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
                                range1.velocity[i] += dirToOutside * 0.001f;
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
}

void PhysicsCollisionSolver::assign(PhysicsCollisionSolver &other)
{
    collisionMap.assign(other.collisionMap);
    resolvedCollisionPairs.replace(other.resolvedCollisionPairs);
    boundingBoxes.replace(other.boundingBoxes);
    staticBoundingBoxes.replace(other.staticBoundingBoxes);
    sortedBoundingBoxes.replace(other.sortedBoundingBoxes);
    sortedStaticBoundingBoxes.replace(other.sortedStaticBoundingBoxes);
}
