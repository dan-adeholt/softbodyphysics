#include "CollisionSolver.h"
#include "CollisionMap.h"
#include "CollisionSet.h"
#include "ShapeAxisSeparator.h"
#include "../utils/MinMax.h"
#include "../utils/Console.h"
#include "../timer.h"
#include "./PhysicsSpace.h"
#include <math.h>
#include <cstdio>

struct ShapeIndexedRange
{
    int start = 0;
    int end = 0;
    int indices[4] = {0, 0, 0, 0};
    bool indexed = false;

    ShapeIndexedRange(const Shape &shape)
    {
        indexed = shape.hasIndices();
        start = indexed ? 0 : shape.start;
        end = indexed ? 4 : shape.end;

        if (indexed)
        {
            for (int i = 0; i < 4; i++)
            {
                indices[i] = shape.start + shape.indices[i];
            }
        }
    }

    int operator[](int i) const
    {
        return indexed ? indices[i] : start + i;
    }

    int size() const
    {
        return end - start;
    }
};

float coefficentOfRestitution = 0.65f;

// EdgeStrategy intersectionStrategy = EdgeStrategy::ClosestSegment;

bool logBox = true;

ShapeBoundingBox CollisionSolver::calculateShapeBoundingBox(const PointMassesRange &points, int shapeIndex, const Shape &shape)
{
    ShapeIndexedRange range(shape);

    Vector2 pos = points.pos[range[0]];
    float minX = pos.x;
    float maxX = pos.x;
    float minY = pos.y;
    float maxY = pos.y;

    for (int j = 1; j < range.size(); j++)
    {
        Vector2 otherPos = points.pos[range[j]];

        minX = min(minX, otherPos.x);
        maxX = max(maxX, otherPos.x);
        minY = min(minY, otherPos.y);
        maxY = max(maxY, otherPos.y);
    }

    return {shapeIndex, minX, minY, maxX, maxY};
}
struct CollisionCell
{
    int boxIndex;
    bool isStatic;
    int next;
};

struct CollisionGrid
{
    Array<ShapeBoundingBox> boundingBoxes;

    CollisionGrid(int width, int height, int cellSize) : width(width), height(height), cellSize(cellSize)
    {
        cells.fill(-1, width * height);
    }

    void reset()
    {
        cells.fill(-1, width * height);
        cellStorage.clear();
        boundingBoxes.clear();
    }

    void updateShapes(const Array<Shape> &shapes, const PointMasses points)
    {
        reset();

        for (int i = 0; i < shapes.size(); i++)
        {
            Shape shape = shapes[i];
            ShapeBoundingBox box = CollisionSolver::calculateShapeBoundingBox(points.range(), i, shape);
            addShapeBoundingBox(box, shape.isStatic);
        }
    }

    template <typename Callback>
    void calculateCollisions(Callback &&onCollision, ConsoleProfileInfo &profileInfo)
    {
        collisionSet.init(boundingBoxes.size());

        for (int i = 0; i < cells.size(); i++)
        {
            int cellIndex = cells[i];
            while (cellIndex != -1)
            {
                CollisionCell cell = cellStorage[cellIndex];
                ShapeBoundingBox box = boundingBoxes[cell.boxIndex];
                int next = cell.next;
                while (next != -1)
                {
                    ShapeBoundingBox otherBox = boundingBoxes[cellStorage[next].boxIndex];
                    profileInfo.numBbboxChecks++;

                    if (box.overlaps(otherBox) && !collisionSet.hasCollision(cell.boxIndex, cellStorage[next].boxIndex))
                    {
                        profileInfo.numBboxOverlaps++;
                        // Handle collision
                        collisionSet.setCollision(cell.boxIndex, cellStorage[next].boxIndex);
                        onCollision(box, otherBox, cell.boxIndex, cellStorage[next].boxIndex, cell.isStatic, cellStorage[next].isStatic);
                    }
                    next = cellStorage[next].next;
                }
                cellIndex = cell.next;
            }
        }
    }

private:
    CollisionSet collisionSet;
    Array<CollisionCell> cellStorage;
    Array<int> cells;

    int width;
    int height;
    int cellSize;

    void addShapeBoundingBox(ShapeBoundingBox box, bool isStatic)
    {
        boundingBoxes.push(box);
        int x1 = (int)floor(box.x1 / cellSize);
        int y1 = (int)floor(box.y1 / cellSize);
        int x2 = (int)floor(box.x2 / cellSize);
        int y2 = (int)floor(box.y2 / cellSize);

        for (int y = y1; y <= y2; y++)
        {
            for (int x = x1; x <= x2; x++)
            {
                if (x >= 0 && x < width && y >= 0 && y < height)
                {
                    int index = y * width + x;
                    int cellIndex = cells[index];

                    CollisionCell cell = {
                        .boxIndex = boundingBoxes.size() - 1,
                        .isStatic = isStatic,
                        .next = cellIndex};
                    cellStorage.push(cell);
                    cells[index] = cellStorage.size() - 1;
                }
            }
        }
    }
};

struct CollisionPair
{
    int shape1Index;
    int shape2Index;
};

struct CollisionSolver::Impl
{
    Impl() : collisionMap(0),
             resolvedCollisionPairs(0),
             boundingBoxes(0),
             sortedBoundingBoxes(0),
             grid(100, 100, 30)
    {
    }

    CollisionMap collisionMap;
    Array<CollisionPair> resolvedCollisionPairs;
    Array<ShapeBoundingBox> boundingBoxes;
    Array<ShapeBoundingBox> sortedBoundingBoxes;
    CollisionGrid grid;
};

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

inline Vector2 calculateImpulsePointToPoint(float pm0VelX, float pm0VelY, float pm0Mass, Vector2 segmentNormal, float pointVelX, float pointVelY, float pointMass)
{

    float relativeVelocityX = pointVelX - pm0VelX;
    float relativeVelocityY = pointVelY - pm0VelY;

    float inverseMass = (1.0f / pointMass) + (1.0f / (pm0Mass));
    float dotProduct = Vector2::vec2dot(relativeVelocityX, relativeVelocityY, segmentNormal.x, segmentNormal.y);

    float impulseMagnitude = (-(1.0f + coefficentOfRestitution) * dotProduct) / inverseMass;

    // if (!isnan(pm0VelX) && !isnan(pm0VelY) && !isnan(pm0Mass) && !isnan(segmentNormal.x) && !isnan(segmentNormal.y) && !isnan(pointVelX) && !isnan(pointVelY) && !isnan(pointMass))
    // {
    if (isnan(impulseMagnitude))
    {
        Console::log("impulseMagnitude is nan %.f %.f %.f %.f %.f %.f [%.1f %.1f]", pm0VelX, pm0VelY, pm0Mass, pointVelX, pointVelY, segmentNormal.x, segmentNormal.y);
    }
    // }

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

bool shapesOverlap(const PointMassesRange &points, const Shape &shape1, const Shape &shape2)
{
    ShapeIndexedRange poly1(shape1);
    ShapeIndexedRange poly2(shape2);

    for (int i = 0; i < poly1.size(); ++i)
    {
        for (int j = 0; j < poly2.size(); ++j)
        {
            if (lineSegmentsIntersect(points.pos[poly1[i]], points.pos[poly1[(i + 1) % poly1.size()]],
                                      points.pos[poly2[j]], points.pos[poly2[(j + 1) % poly2.size()]]))
                return true;
        }
    }
    return false;
}

float lastMaxAmplitude = 0.0f;
float lastMaxAmplitudeStatic = 0.0f;

int countNumCollisions(int pointIndex, const PointMassesRange &points, const Shape &collisionShape, float pointX, float pointY, float outX)
{
    int numIntersections = 0;
    const ShapeIndexedRange collisionShapeRange(collisionShape);

    // Check intersections between (point.pos.x, point.pos.y) -> (outX, point.pos.y) and each line segment in shape1
    for (int i = 0; i < collisionShapeRange.size(); i++)
    {
        int collisionIndex0 = collisionShapeRange[i];
        int collisionIndex1 = collisionShapeRange[(i + 1) % collisionShapeRange.size()];

        // Return early here, we currently don't support self-collisions between adjacent subshapes
        if (pointIndex == collisionIndex0 || pointIndex == collisionIndex1)
        {
            return 0;
        }

        Vector2 p0 = points.pos[collisionIndex0];
        Vector2 p1 = points.pos[collisionIndex1];

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

ClosestSegmentResult CollisionSolver::findEntryEdgeClosestSegment(PointMassesRange points,
                                                                  const Shape &collisionShape,
                                                                  const Vector2 &currentPoint)
{
    float minDistanceSquared = __FLT_MAX__;

    ClosestSegmentResult result = {-1, Vector2(), Vector2(), 0.0f};
    const ShapeIndexedRange collisionShapeRange(collisionShape);

    for (int i = 0; i < collisionShapeRange.size(); i++)
    {
        Vector2 segment0 = points.pos[collisionShapeRange[i]];
        Vector2 segment1 = points.pos[collisionShapeRange[(i + 1) % collisionShapeRange.size()]];

        float distanceToVertex = (segment0 - currentPoint).length();

        // Vector from A to B
        Vector2 segment = segment1 - segment0;
        // Vector from A to P
        Vector2 segmentToPoint = currentPoint - segment0;
        Vector2 segmentNormal = segment.normalVector().normalized();
        Vector2 pointOutside = (segment0 + segment * 0.5f) - segmentNormal * 2.0f;

        // float t_point, t_edge;

        // Console::drawPoint(pointOutside, 0x00FF00);

        // The projection of point P onto the line defined by segment AB is given by:
        // v dot w / v dot v
        // Compute projection t
        float t = segmentToPoint.dot(segment) / segment.dot();
        t = clamp(t, 0.0f, 1.0f);

        Vector2 closestPoint = segment0 + segment * t;
        Vector2 pointToClosestPoint = closestPoint - currentPoint;

        float distanceToClosestPointSquared = (pointToClosestPoint).lengthSquared();

        if (distanceToClosestPointSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceToClosestPointSquared;
            result.entryEdgeIndex0 = i;
            result.closestPoint0 = closestPoint;
            result.entryTime0 = t;
            result.pointOutside0 = pointOutside;
        }
    }

    return result;
}

// PointMassesRange collisionShape, float pointX, float pointY, float outX
bool CollisionSolver::isPointOutsideShape(int pointIndex, float pointX, float pointY, const ShapeBoundingBox &box, const PointMassesRange &points, const Shape &collisionShape)
{
    // First check - is the point outside the bounding box of the other shape?
    // Then extend horizontal line from point to the right,  outside of bounding box.
    return pointX < box.x1 ||
           pointX > box.x2 ||
           pointY < box.y1 ||
           pointY > box.y2 ||
           // If the number of intersections is even, that means that the point is definitively outside
           // of our shape. If it is odd, then it is inside.
           countNumCollisions(pointIndex, points, collisionShape, pointX, pointY, box.x2 + 10.0f) % 2 == 0;
}

void reduceOverlap(ShapeBoundingBox &box1, ShapeBoundingBox &box2)
{
    if (!box1.overlaps(box2))
    {
        return; // Nothing to do if boxes don't overlap
    }

    if (box1.encloses(box2))
    {
        return;
    }
    else if (box2.encloses(box1))
    {
        return;
    }

    // Calculate overlap amounts in each direction
    float overlapX = min(box1.x2, box2.x2) - max(box1.x1, box2.x1);
    float overlapY = min(box1.y2, box2.y2) - max(box1.y1, box2.y1);
    float limit = 5.0f;

    if (overlapX >= 0 && overlapX < overlapY)
    {
        float splitOverlap = overlapX * 0.5f + 0.25f;
        if ((box1.width() - splitOverlap) < limit || (box2.width() - splitOverlap) < limit)
        {
            return;
        }

        if (box1.x1 < box2.x1)
        {
            box1.x2 -= splitOverlap;
            box2.x1 += splitOverlap;
        }
        else
        {
            box1.x1 += splitOverlap;
            box2.x2 -= splitOverlap;
        }
    }
    else
    {
        float splitOverlap = overlapY * 0.5f + 0.25f;

        if ((box1.height() - splitOverlap) < limit || (box2.height() - splitOverlap) < limit)
        {
            return;
        }

        if (box1.y1 < box2.y1)
        {
            box1.y2 -= splitOverlap;
            box2.y1 += splitOverlap;
        }
        else
        {
            box1.y1 += splitOverlap;
            box2.y2 -= splitOverlap;
        }
    }
}

void reduceOverlapStatic(ShapeBoundingBox &box1, ShapeBoundingBox &staticBox)
{
    if (!box1.overlaps(staticBox))
    {
        return; // Nothing to do if boxes don't overlap
    }

    // Calculate overlap amounts in each direction
    float overlapX = min(box1.x2, staticBox.x2) - max(box1.x1, staticBox.x1);
    float overlapY = min(box1.y2, staticBox.y2) - max(box1.y1, staticBox.y1);
    float limit = 2.0f;

    if (overlapX >= 0 && overlapX < overlapY)
    {
        if (box1.width() < limit || staticBox.width() < limit)
        {
            return;
        }

        float overlapXExtended = overlapX + 0.25f;
        if (box1.x1 < staticBox.x1)
        {
            box1.x2 -= overlapXExtended;
        }
        else
        {
            box1.x1 += overlapXExtended;
        }
    }
    else
    {
        if (box1.height() < limit || staticBox.height() < limit)
        {
            return;
        }

        float overlapExtended = overlapY + 0.25f;
        if (box1.y1 < staticBox.y1)
        {
            box1.y2 -= overlapExtended;
        }
        else
        {
            box1.y1 += overlapExtended;
        }
    }
}

int CollisionSolver::calculateCollisionsMidPoint(
    PointMassesRange points,
    const Shape &collisionShape,
    const Shape &movingShape,
    const ShapeBoundingBox &collisionBox,
    const ShapeBoundingBox &movingBox)
{
    if (movingShape.isStatic && collisionShape.isStatic)
    {
        return 0;
    }

    int numCollisions = 0;

    const ShapeIndexedRange movingRange(movingShape);
    const ShapeIndexedRange collisionRange(collisionShape);

    for (int i = 0; i < movingRange.size(); i++)
    {
        int nextIndex = (i + 1) % movingRange.size();
        int pointIndex = movingRange[i];
        int nextPointIndex = movingRange[nextIndex];

        Vector2 prevPos = points.pos[movingRange[i == 0 ? movingRange.size() - 1 : i - 1]];
        Vector2 nextPos = points.pos[movingRange[nextIndex]];

        Vector2 pointToNext = nextPos - points.pos[pointIndex];
        Vector2 pointPos = points.pos[pointIndex] + pointToNext * 0.5f;

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(pointIndex, pointPos.x, pointPos.y, collisionBox, points, collisionShape))
        {
            continue;
        }

        Console::drawPoint(pointPos, 0xFF0000);

        numCollisions++;

        // Console::log("** Find entry edge for point %d in %d **", i, movingBox.shapeIndex);

        ClosestSegmentResult result = findEntryEdgeClosestSegment(points, collisionShape, pointPos);
        Vector2 pointVelocity = points.velocity[pointIndex];

        int collisionIndex0 = collisionRange[result.entryEdgeIndex0];
        int collisionIndex1 = collisionRange[(result.entryEdgeIndex0 + 1) % collisionRange.size()];

        float pointMass = (points.mass[pointIndex] + points.mass[pointIndex]) * 0.5f;

        Vector2 pm0Pos = points.pos[collisionIndex0];
        Vector2 pm1Pos = points.pos[collisionIndex1];
        Vector2 segmentNormal = Vector2(-pm1Pos.y + pm0Pos.y, pm1Pos.x - pm0Pos.x).normalized();

        Console::drawPoint(result.closestPoint0, 0x00FF00);

        if (collisionShape.isStatic)
        {
            Vector2 impulse = calculateImpulseStatic(pointVelocity.x, pointVelocity.y, pointMass, segmentNormal);
            Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();

            Vector2 splitOffset = result.closestPoint0 - pointPos;
            points.pos[pointIndex] += splitOffset + reflection * 0.1f;
            points.pos[nextPointIndex] += splitOffset + reflection * 0.1f;

            points.velocity[pointIndex] += (impulse / pointMass) * 0.5f;
            points.velocity[nextPointIndex] += (impulse / pointMass) * 0.5f;
        }
        else
        {
            float pm0Mass = points.mass[collisionIndex0];
            Vector2 pm0Vel = points.velocity[collisionIndex0];
            float pm1Mass = points.mass[collisionIndex1];
            Vector2 pm1Vel = points.velocity[collisionIndex1];

            Vector2 offset = points.pos[pointIndex] - result.closestPoint0;

            Vector2 impulse = calculateImpulse(pm0Vel.x, pm0Vel.y, pm0Mass, pm1Vel.x, pm1Vel.y, pm1Mass, segmentNormal, pointVelocity.x, pointVelocity.y, pointMass, result.entryTime0);

            Vector2 pm0VelocityDiff = (impulse * (1.0f - result.entryTime0)) / pm0Mass;
            points.velocity[collisionIndex0] -= pm0VelocityDiff;
            Vector2 pm1VelocityDiff = (impulse * result.entryTime0) / pm1Mass;
            points.velocity[collisionIndex1] -= pm1VelocityDiff;

            if (!movingShape.isStatic)
            {
                float totalMass = pm0Mass + pm1Mass + pointMass;
                Vector2 splitOffset = result.closestPoint0 - pointPos;

                Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();

                Vector2 avgDir = (pm1Pos - pm0Pos).normalized();

                Vector2 A = result.closestPoint0 - avgDir * 100.0f;
                Vector2 B = result.closestPoint0 + avgDir * 100.0f;

                points.pos[collisionIndex0] = closestPointToAxis(A, B, pm0Pos);
                points.pos[collisionIndex1] = closestPointToAxis(A, B, pm1Pos);
                points.pos[pointIndex] = closestPointToAxis(A, B, points.pos[pointIndex]) + reflection * 0.2f;
                points.pos[nextPointIndex] = closestPointToAxis(A, B, points.pos[nextPointIndex]) + reflection * 0.2f;
                points.velocity[pointIndex] += (impulse / pointMass) * 0.5f;
                points.velocity[nextPointIndex] += (impulse / pointMass) * 0.5f;
            }
            else
            {
                Vector2 offsetExtended = offset * 1.01f;
                points.pos[collisionIndex0] -= offsetExtended;
                points.pos[collisionIndex1] -= offsetExtended;
            }
        }
    }

    return numCollisions;
}

int CollisionSolver::calculateCollisions(
    PointMassesRange points,
    const Shape &collisionShape,
    const Shape &movingShape,
    const ShapeBoundingBox &collisionBox,
    const ShapeBoundingBox &movingBox)
{
    if (collisionShape.isStatic && movingShape.isStatic)
    {
        return 0;
    }

    int numCollisions = 0;

    ShapeIndexedRange movingRange(movingShape);
    ShapeIndexedRange collisionRange(collisionShape);

    for (int i = 0; i < movingRange.size(); i++)
    {
        int pointIndex = movingRange[i];
        Vector2 pointPos = points.pos[pointIndex];
        Vector2 prevPos = points.pos[movingRange[i == 0 ? movingRange.size() - 1 : i - 1]];
        Vector2 nextPos = points.pos[movingRange[(i + 1) % movingRange.size()]];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(pointIndex, pointPos.x, pointPos.y, collisionBox, points, collisionShape))
        {
            continue;
        }

        numCollisions++;

        ClosestSegmentResult result = findEntryEdgeClosestSegment(points, collisionShape, pointPos);

        Vector2 pointVelocity = points.velocity[pointIndex];

        float pointMass = points.mass[pointIndex];
        int collisionIndex0 = collisionRange[result.entryEdgeIndex0];
        int collisionIndex1 = collisionRange[(result.entryEdgeIndex0 + 1) % collisionRange.size()];

        Vector2 pm0Pos = points.pos[collisionIndex0];
        Vector2 pm1Pos = points.pos[collisionIndex1];
        Vector2 segmentNormal = Vector2(-pm1Pos.y + pm0Pos.y, pm1Pos.x - pm0Pos.x).normalized();

        if (collisionShape.isStatic)
        {
            Vector2 impulse = calculateImpulseStatic(pointVelocity.x, pointVelocity.y, pointMass, segmentNormal);

            Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();
            points.velocity[pointIndex] += impulse / pointMass;
            Vector2 diff = points.pos[pointIndex] - result.closestPoint0;
            points.pos[pointIndex] = result.closestPoint0 + reflection * 0.1f;
        }
        else
        {
            float pm0Mass = points.mass[collisionIndex0];
            Vector2 pm0Vel = points.velocity[collisionIndex0];
            float pm1Mass = points.mass[collisionIndex1];
            Vector2 pm1Vel = points.velocity[collisionIndex1];

            Vector2 offset = points.pos[pointIndex] - result.closestPoint0;

            Vector2 impulse = calculateImpulse(pm0Vel.x, pm0Vel.y, pm0Mass, pm1Vel.x, pm1Vel.y, pm1Mass, segmentNormal, pointVelocity.x, pointVelocity.y, pointMass, result.entryTime0);

            Vector2 pm0VelocityDiff = (impulse * (1.0f - result.entryTime0)) / pm0Mass;
            points.velocity[collisionIndex0] -= pm0VelocityDiff;
            Vector2 pm1VelocityDiff = (impulse * result.entryTime0) / pm1Mass;
            points.velocity[collisionIndex1] -= pm1VelocityDiff;

            if (!movingShape.isStatic)
            {
                float totalMass = pm0Mass + pm1Mass + pointMass;
                Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();

                points.pos[collisionIndex0] = pm0Pos;
                points.pos[collisionIndex1] = pm1Pos;
                points.pos[pointIndex] = result.closestPoint0 + reflection * 0.1f;
                points.velocity[pointIndex] += impulse / pointMass;
            }
            else
            {
                Vector2 offsetExtended = offset * 1.01f;
                points.pos[collisionIndex0] += offsetExtended;
                points.pos[collisionIndex1] += offsetExtended;
            }
        }
    }

    return numCollisions;
}

void CollisionSolver::calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes, const Array<Shape> &shapes, const PointMasses &points)
{
    boundingBoxes.clear();

    for (int i = 0; i < shapes.size(); i++)
    {
        boundingBoxes.push(calculateShapeBoundingBox(points.range(), i, shapes[i]));
    }
}

Array<ShapeBoundingBox> &CollisionSolver::boundingBoxes()
{
    return m->boundingBoxes;
}

CollisionSolver::CollisionSolver() : m(new Impl())
{
}

CollisionSolver::~CollisionSolver()
{
    delete m;
}

void CollisionSolver::clear()
{
    m->collisionMap.clear();
    m->resolvedCollisionPairs.clear();
    m->boundingBoxes.clear();
    m->sortedBoundingBoxes.clear();
}

void CollisionSolver::updateBoundingBoxes(PhysicsSpace &space, ConsoleProfileInfo &profileInfo)
{
    // m->grid.updateShapes(space.shapes, space.points);
    // for (int i = 0; i < m->resolvedCollisionPairs.size(); i++)
    // {
    //     CollisionPair &pair = m->resolvedCollisionPairs[i];
    //     const Shape &shape1 = space.shapes[pair.shape1Index];

    //     PointMassesRange range1 = space.points.range(shape1);
    //     PointMassesRange range2;

    //     const Shape &shape2 = space.shapes[pair.shape2Index];
    //     range2 = space.points.range(shape2);

    //     if (!shapesOverlap(range1, range2))
    //     {
    //         m->collisionMap.resetCollision(pair.shape1Index, pair.shape2Index);
    //     }
    // }

    // m->resolvedCollisionPairs.clear();

    if (m->collisionMap.numElements != space.shapes.size())
    {
        m->collisionMap.resize(space.shapes.size());
    }

    if (m->boundingBoxes.capacity() == 0)
    {
        m->boundingBoxes.reserve(space.shapes.size());
        m->sortedBoundingBoxes.reserve(space.shapes.size());
    }

    // For a broad phase collision detection, sort using insertion sort along a single axis

    {
        Timer boundingBoxTimer;
        calculateBoundingBoxes(m->boundingBoxes, space.shapes, space.points);
        if (m->sortedBoundingBoxes.size() == 0)
        {
            for (int i = 0; i < m->boundingBoxes.size(); i++)
            {
                m->sortedBoundingBoxes.push(m->boundingBoxes[i]);
            }
        }
        else
        {
            for (int i = m->sortedBoundingBoxes.size(); i < m->boundingBoxes.size(); i++)
            {
                m->sortedBoundingBoxes.push(m->boundingBoxes[i]);
            }

            updateSortedBoundingBoxes(m->sortedBoundingBoxes, m->boundingBoxes);
        }

        sortBoundingBoxes(m->sortedBoundingBoxes);
        profileInfo.boundingBoxTimeMillis = boundingBoxTimer.elapsedMillis();
    }

    profileInfo.numBboxes = m->sortedBoundingBoxes.size();
    profileInfo.numBbboxChecks = 0;
    profileInfo.numBboxOverlaps = 0;
    profileInfo.numCollisions = 0;
}

void CollisionSolver::boxSeparateDynamicAndStaticShapes(PointMassesRange points, const Shape &movingShape, const Shape &staticShape)
{
    ShapeBoundingBox movingBoxContracted = calculateShapeBoundingBox(points, 1, movingShape);
    ShapeBoundingBox staticBoxContracted = calculateShapeBoundingBox(points, 1, staticShape);
    reduceOverlapStatic(movingBoxContracted, staticBoxContracted);

    ShapeIndexedRange movingRange(movingShape);

    for (int i = 0; i < movingRange.size(); i++)
    {
        int pointIndex = movingRange[i];
        points.pos[pointIndex].x = clamp(points.pos[pointIndex].x, movingBoxContracted.x1, movingBoxContracted.x2);
        points.pos[pointIndex].y = clamp(points.pos[pointIndex].y, movingBoxContracted.y1, movingBoxContracted.y2);
        // movingRange.velocity[i] *= 0.5f;
    }
}

void CollisionSolver::handleCollisions(PhysicsSpace &space, ConsoleProfileInfo &profileInfo)
{
    Timer collisionsTimer;
    updateBoundingBoxes(space, profileInfo);

    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
    {
        const ShapeBoundingBox &box = m->sortedBoundingBoxes[i];
        const Shape &shape1 = space.shapes[box.shapeIndex];

        if (shape1.disableShapeMatching)
        {
            continue;
        }
        for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
        {
            const ShapeBoundingBox &otherBox = m->sortedBoundingBoxes[j];
            profileInfo.numBbboxChecks++;

            if (otherBox.x1 > box.x2)
            {
                break;
            }
            // Due to the sorted nature of the bounding boxes, we already know that
            // otherBox.x1 >= box.x1, so we only need to check the other axis
            else if (otherBox.y1 > box.y2 || otherBox.y2 < box.y1)
            {
                continue;
            }

            const Shape &shape2 = space.shapes[otherBox.shapeIndex];

            if (shape1.parentIndex != -1 && shape1.parentIndex == shape2.parentIndex)
            {
                continue;
            }

            if (shape2.disableShapeMatching)
            {
                continue;
            }

            calculateCollisions(space.points.range(), shape1, shape2, box, otherBox);
            calculateCollisions(space.points.range(), shape2, shape1, otherBox, box);

            if (shapesOverlap(space.points.range(), shape1, shape2))
            {
                m->collisionMap.incrementCollision(box.shapeIndex, otherBox.shapeIndex);
                int numCollisions = m->collisionMap.getCollisionCount(box.shapeIndex, otherBox.shapeIndex);
                calculateCollisionsMidPoint(space.points.range(), shape1, shape2, box, otherBox);
                calculateCollisionsMidPoint(space.points.range(), shape2, shape1, otherBox, box);

                // Console::log("Num collisions: %d", numCollisions);
                if (numCollisions > 32 && shapesOverlap(space.points.range(), shape1, shape2))
                {
                    PointMassesRange range1 = space.points.range(shape1);
                    PointMassesRange range2 = space.points.range(shape2);

                    if (!shape1.isStatic && !shape2.isStatic)
                    {
                        // Console::log("Case A");
                        ShapeAxisSeparator::separateShapesFromIntersectionAxis(range1, range2);
                    }
                    else if ((shape1.isStatic && !shape2.isStatic) || (shape2.isStatic && !shape1.isStatic))
                    {
                        // Console::log("Case B");
                        const Shape &staticShape(shape1.isStatic ? shape1 : shape2);
                        const Shape &movingShape(shape1.isStatic ? shape2 : shape1);
                        boxSeparateDynamicAndStaticShapes(space.points.range(), movingShape, staticShape);
                    }
                }

                if (!shapesOverlap(space.points.range(), shape1, shape2))
                {
                    m->collisionMap.resetCollision(box.shapeIndex, otherBox.shapeIndex);
                }
            }
            else
            {
                m->collisionMap.resetCollision(box.shapeIndex, otherBox.shapeIndex);
            }
        }
    }

    profileInfo.collisionTimeMillis = collisionsTimer.elapsedMillis();
}

void CollisionSolver::assign(CollisionSolver &other)
{
    m->collisionMap.assign(other.m->collisionMap);
    m->resolvedCollisionPairs.replace(other.m->resolvedCollisionPairs);
    m->boundingBoxes.replace(other.m->boundingBoxes);
    m->sortedBoundingBoxes.replace(other.m->sortedBoundingBoxes);
}
