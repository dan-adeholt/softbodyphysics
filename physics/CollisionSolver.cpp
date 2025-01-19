#include "CollisionSolver.h"
#include "CollisionMap.h"
#include "CollisionSet.h"
#include "../utils/MinMax.h"
#include "../utils/Console.h"
#include "../timer.h"
#include "./PhysicsSpace.h"
#include <math.h>
#include <cstdio>

float coefficentOfRestitution = 0.65f;

// EdgeStrategy intersectionStrategy = EdgeStrategy::ClosestSegment;

bool logBox = true;

ShapeBoundingBox CollisionSolver::calculateShapeBoundingBox(int shapeIndex, const PointMassesRange &range)
{
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
            ShapeBoundingBox box = CollisionSolver::calculateShapeBoundingBox(i, points.range(shape));
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

bool lineSegmentIntersection(
    const Vector2 &p1, const Vector2 &p2,
    const Vector2 &q1, const Vector2 &q2,
    float &t_p, float &t_q)
{
    Vector2 r = p2 - p1;
    Vector2 s = q2 - q1;
    float denominator = r.cross(s);

    if (fabs(denominator) < 0.001f)
    {
        // Lines are parallel
        return false;
    }

    Vector2 qp = q1 - p1;
    t_p = qp.cross(s) / denominator;
    t_q = qp.cross(r) / denominator;

    return (t_p >= 0.0f && t_p <= 1.0f) && (t_q >= 0.0f && t_q <= 1.0f);
}

ClosestSegmentResult CollisionSolver::findEntryEdgeClosestSegment(PointMassesRange collisionShape,
                                                                  const Vector2 &currentPoint,
                                                                  bool log)
{
    float minDistanceSquared = __FLT_MAX__;
    float minDistanceSquared2 = __FLT_MAX__;

    int collisionShapeSize = collisionShape.pos.size;
    ClosestSegmentResult result = {-1, Vector2(), Vector2(), 0.0f, -1, Vector2(), Vector2(), 0.0f};

    for (int i = 0; i < collisionShapeSize; i++)
    {
        Vector2 segment0 = collisionShape.pos[i];
        Vector2 segment1 = collisionShape.pos[(i + 1) % collisionShapeSize];

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
            minDistanceSquared2 = minDistanceSquared;
            result.entryEdgeIndex1 = result.entryEdgeIndex0;
            result.closestPoint1 = result.closestPoint0;
            result.entryTime1 = result.entryTime0;
            result.pointOutside1 = result.pointOutside0;

            minDistanceSquared = distanceToClosestPointSquared;
            result.entryEdgeIndex0 = i;
            result.closestPoint0 = closestPoint;
            result.entryTime0 = t;
            result.pointOutside0 = pointOutside;
        }

        if (distanceToClosestPointSquared < minDistanceSquared2 && (distanceToClosestPointSquared > minDistanceSquared || (distanceToClosestPointSquared == minDistanceSquared && i != result.entryEdgeIndex0)))
        {
            minDistanceSquared2 = distanceToClosestPointSquared;
            result.entryEdgeIndex1 = i;
            result.closestPoint1 = closestPoint;
            result.entryTime1 = t;
            result.pointOutside1 = pointOutside;
        }
    }

    return result;
}

// void CollisionSolver::findEntryEdge(int index,
//                                     int collisionShapeIndex,
//                                     int movingShapeIndex,
//                                     PointMassesRange collisionShape,
//                                     const Vector2 &currentPoint,
//                                     const Vector2 &velocity,
//                                     int &vertexHitIndex,
//                                     int &entryEdgeIndex,
//                                     Vector2 &entryPoint,
//                                     float &entryTime)
// {
//     float minDistanceSquared = __FLT_MAX__;
//     int collisionShapeSize = collisionShape.pos.size;

//     float minTime = __FLT_MAX__;
//     bool collisionFound = false;

//     // Movement vector of the point

//     Vector2 previousPointFarBack = currentPoint - velocity * 200.0f;
//     Vector2 currentPointExtendedSlighly = currentPoint + velocity * 30.0f;
//     Vector2 movement = currentPointExtendedSlighly - previousPointFarBack;
//     float movementLengthSquared = movement.lengthSquared();

//     for (int i = 0; i < collisionShapeSize; i++)
//     {
//         Vector2 segmentStart = collisionShape.pos[i];
//         Vector2 segmentEnd = collisionShape.pos[(i + 1) % collisionShapeSize];

//         // Check for intersection between point's movement and edge
//         float t_point, t_edge;

//         float t = (segmentStart - previousPointFarBack).dot(movement) / movementLengthSquared;
//         t = clamp(t, 0.0f, 1.0f);
//         Vector2 closestPoint = previousPointFarBack + movement * t;

//         // Compute squared distance from vertex to closest point
//         float distanceSquared = (segmentStart - closestPoint).lengthSquared();

//         // Define threshold for vertex proximity (adjust as needed)
//         float threshold = 5.0f;
//         float thresholdSquared = threshold * threshold;

//         float distanceToVertexSquared = (segmentStart - currentPoint).lengthSquared();

//         // if (distanceSquared < thresholdSquared)
//         // {
//         //     vertexHitIndex = i;
//         //     entryEdgeIndex = -1; // Indicate point-point collision
//         //     return;
//         // }

//         if (lineSegmentIntersection(previousPointFarBack, currentPointExtendedSlighly, segmentStart, segmentEnd, t_point, t_edge))
//         {
//             if (t_point < minTime && t_point >= 0.0f && t_point <= 1.0f)
//             {
//                 minTime = t_point;
//                 entryEdgeIndex = i;
//                 entryPoint = previousPointFarBack + movement * t_point;
//                 entryTime = t_edge;
//                 collisionFound = true;
//             }
//         }
//         else
//         {
//             // if (index == 0 && movingShapeIndex == 1 && collisionShapeIndex == 0)
//             // {
//             // Console::log("[%d] - No intersection between point and edge %d-%d", i, (i + 1) % collisionShapeSize);
//             // }
//         }
//     }

//     if (!collisionFound)
//     {
//         Console::log("ERROR: No entry edge found [%d-%d]! %.2f %.2f => %.2f %.f", movingShapeIndex, index, previousPointFarBack.x, previousPointFarBack.y, currentPoint.x, currentPoint.y);
//         for (int i = 0; i < collisionShapeSize; i++)
//         {
//             Console::log("Segment [%d-%d]: %.2f %.2f => %.2f %.2f", collisionShapeIndex, i, collisionShape.pos[i].x, collisionShape.pos[i].y, collisionShape.pos[(i + 1) % collisionShapeSize].x, collisionShape.pos[(i + 1) % collisionShapeSize].y);
//         }

//         // Console::setDebugger(true);
//     }
// }

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

int CollisionSolver::calculateCollisions(
    PointMassesRange collisionShape,
    PointMassesRange movingShape,
    PointMassesRange prevCollisionShape,
    PointMassesRange prevMovingShape,
    const ShapeBoundingBox &collisionBox,
    const ShapeBoundingBox &movingBox,
    bool isStaticCollisionShape,
    bool isStaticMovingShape,
    int numIterationsTouching)
{
    if (isStaticMovingShape && isStaticCollisionShape)
    {
        return 0;
    }

    int numCollisions = 0;

    struct CollisionInfo
    {
        int index;
        bool pointIsInsideShape;
        Vector2 pointOutsideShape;
        ClosestSegmentResult result = {};
    };

    static Array<CollisionInfo> collisionInfo;
    collisionInfo.clear();

    for (int i = 0; i < movingShape.pos.size; i++)
    {
        Vector2 pointPos = movingShape.pos[i];
        Vector2 prevPos = movingShape.pos[i == 0 ? movingShape.pos.size - 1 : i - 1];
        Vector2 nextPos = movingShape.pos[(i + 1) % movingShape.pos.size];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(pointPos.x, pointPos.y, collisionBox, collisionShape))
        {
            CollisionInfo info = {i, false, pointPos};
            collisionInfo.push(info);

            continue;
        }

        numCollisions++;

        // Console::log("** Find entry edge for point %d in %d **", i, movingBox.shapeIndex);

        ClosestSegmentResult result = findEntryEdgeClosestSegment(collisionShape, pointPos, !isStaticCollisionShape && !isStaticMovingShape);

        CollisionInfo info = {
            i,
            true,
            result.pointOutside0,
            result};

        collisionInfo.push(info);
    }

    for (int i = 0; i < collisionInfo.size(); i++)
    {
        CollisionInfo &info = collisionInfo[i];
        if (!info.pointIsInsideShape)
        {
            continue;
        }

        Vector2 prevPointOutside = collisionInfo[i == 0 ? collisionInfo.size() - 1 : i - 1].pointOutsideShape;
        Vector2 nextPointOutside = collisionInfo[(i + 1) % collisionInfo.size()].pointOutsideShape;
        Vector2 segmentStart = collisionShape.pos[info.result.entryEdgeIndex0];
        Vector2 segmentEnd = collisionShape.pos[(info.result.entryEdgeIndex0 + 1) % collisionShape.size()];

        float t_point, t_edge;

        if (!isStaticCollisionShape && (lineSegmentIntersection(prevPointOutside, info.pointOutsideShape, segmentStart, segmentEnd, t_point, t_edge) ||
                                        lineSegmentIntersection(info.pointOutsideShape, nextPointOutside, segmentStart, segmentEnd, t_point, t_edge)))
        {
            Vector2 diff = info.result.closestPoint0 - info.result.closestPoint1;

            info.result.closestPoint0 = info.result.closestPoint1;
            info.result.entryEdgeIndex0 = info.result.entryEdgeIndex1;
            info.result.entryTime0 = info.result.entryTime1;
            info.result.pointOutside0 = info.result.pointOutside1;
        }

        Vector2 pointVelocity = movingShape.velocity[i];
        ClosestSegmentResult result = info.result;

        if (result.entryEdgeIndex0 != -1)
        {
            float pointMass = movingShape.mass[i];
            Vector2 pm0Pos = collisionShape.pos[result.entryEdgeIndex0];
            Vector2 pm1Pos = collisionShape.pos[(result.entryEdgeIndex0 + 1) % collisionShape.pos.size];
            Vector2 segmentNormal = Vector2(-pm1Pos.y + pm0Pos.y, pm1Pos.x - pm0Pos.x).normalized();

            if (isStaticCollisionShape)
            {
                Vector2 impulse = calculateImpulseStatic(pointVelocity.x, pointVelocity.y, pointMass, segmentNormal);

                Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();
                movingShape.velocity[i] += impulse / pointMass;
                movingShape.pos[i] = result.closestPoint0 + reflection * 0.1f;
            }
            else
            {
                float pm0Mass = collisionShape.mass[result.entryEdgeIndex0];
                Vector2 pm0Vel = collisionShape.velocity[result.entryEdgeIndex0];
                float pm1Mass = collisionShape.mass[(result.entryEdgeIndex0 + 1) % collisionShape.pos.size];
                Vector2 pm1Vel = collisionShape.velocity[(result.entryEdgeIndex0 + 1) % collisionShape.pos.size];

                Vector2 offset = movingShape.pos[i] - result.closestPoint0;

                Vector2 impulse = calculateImpulse(pm0Vel.x, pm0Vel.y, pm0Mass, pm1Vel.x, pm1Vel.y, pm1Mass, segmentNormal, pointVelocity.x, pointVelocity.y, pointMass, result.entryTime0);

                Vector2 pm0VelocityDiff = (impulse * (1.0f - result.entryTime0)) / pm0Mass;
                collisionShape.velocity[result.entryEdgeIndex0] -= pm0VelocityDiff;
                Vector2 pm1VelocityDiff = (impulse * result.entryTime0) / pm1Mass;
                collisionShape.velocity[(result.entryEdgeIndex0 + 1) % collisionShape.pos.size] -= pm1VelocityDiff;

                if (!isStaticMovingShape)
                {
                    float totalMass = pm0Mass + pm1Mass + pointMass;

                    float weightSeg1 = ((totalMass - pm0Mass) / totalMass) * (1.0f - result.entryTime0);
                    float weightSeg2 = ((totalMass - pm1Mass) / totalMass) * result.entryTime0;

                    Vector2 newPm0Pos = pm0Pos; // + offset * weightSeg1;
                    Vector2 newPm1Pos = pm1Pos; // + offset * weightSeg2;
                    Vector2 newMinPoint = newPm0Pos + (newPm1Pos - newPm0Pos) * result.entryTime0;
                    Vector2 newSegmentNormal = Vector2(-newPm1Pos.y + newPm0Pos.y, newPm1Pos.x - newPm0Pos.x).normalized();

                    Vector2 reflection = pointVelocity.reflect(newSegmentNormal).normalized();

                    collisionShape.pos[result.entryEdgeIndex0] = newPm0Pos;
                    collisionShape.pos[(result.entryEdgeIndex0 + 1) % collisionShape.pos.size] = newPm1Pos;
                    movingShape.pos[i] = newMinPoint + reflection * 0.1f;
                    movingShape.velocity[i] += impulse / pointMass;
                }
                else
                {
                    Vector2 offsetExtended = offset * 1.01f;
                    collisionShape.pos[result.entryEdgeIndex0] += offsetExtended;
                    collisionShape.pos[(result.entryEdgeIndex0 + 1) % collisionShape.pos.size] += offsetExtended;
                }

                // Console::logCollisionImpulse((impulse / pointMass) * 100.0f, movingShape.pos[i]);
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
        boundingBoxes.push(calculateShapeBoundingBox(i, points.range(shapes[i])));
    }
}

Array<ShapeBoundingBox> &CollisionSolver::boundingBoxes()
{
    return m->boundingBoxes; // TODO: insert return statement here
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
    for (int i = 0; i < m->resolvedCollisionPairs.size(); i++)
    {
        CollisionPair &pair = m->resolvedCollisionPairs[i];
        const Shape &shape1 = space.shapes[pair.shape1Index];

        PointMassesRange range1 = space.points.range(shape1);
        PointMassesRange range2;

        const Shape &shape2 = space.shapes[pair.shape2Index];
        range2 = space.points.range(shape2);

        if (!shapesOverlap(range1, range2))
        {
            m->collisionMap.resetCollision(pair.shape1Index, pair.shape2Index);
        }
    }

    m->resolvedCollisionPairs.clear();

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

void CollisionSolver::handleCollisions(PhysicsSpace &space, PhysicsSpace &prevSpace, ConsoleProfileInfo &profileInfo)
{
    Timer collisionsTimer;
    updateBoundingBoxes(space, profileInfo);

    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
    {
        const ShapeBoundingBox &box = m->sortedBoundingBoxes[i];
        const Shape &shape1 = space.shapes[box.shapeIndex];

        for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
        {
            const ShapeBoundingBox &otherBox = m->sortedBoundingBoxes[j];
            profileInfo.numBbboxChecks++;

            if (otherBox.x1 <= box.x2)
            {
                // Due to the sorted nature of the bounding boxes, we already know that
                // otherBox.x1 >= box.x1, so we only need to check the other axis

                if (otherBox.y1 <= box.y2 && otherBox.y2 >= box.y1)
                {

                    // Check for collision
                    int numCollisions = m->collisionMap.getCollisionCount(box.shapeIndex, otherBox.shapeIndex);

                    const Shape &shape2 = space.shapes[otherBox.shapeIndex];

                    if (numCollisions < 8)
                    {
                        calculateCollisions(
                            space.points.range(shape1), space.points.range(shape2),
                            prevSpace.points.range(shape1), prevSpace.points.range(shape2),
                            box, otherBox, shape1.isStatic, shape2.isStatic, numCollisions);
                        calculateCollisions(space.points.range(shape2), space.points.range(shape1),
                                            prevSpace.points.range(shape2), prevSpace.points.range(shape1),
                                            otherBox, box, shape2.isStatic, shape1.isStatic, numCollisions);

                        if (shapesOverlap(space.points.range(shape1), space.points.range(shape2)))
                        {
                            m->collisionMap.incrementCollision(box.shapeIndex, otherBox.shapeIndex);
                        }
                    }
                    else if (shapesOverlap(space.points.range(shape1), space.points.range(shape2)))
                    {
                        m->resolvedCollisionPairs.push({box.shapeIndex, otherBox.shapeIndex});
                        PointMassesRange range1 = space.points.range(shape1);
                        PointMassesRange range2 = space.points.range(shape2);

                        if (!shape1.isStatic && !shape2.isStatic)
                        {
                            ShapeBoundingBox movingBoxContracted = calculateShapeBoundingBox(1, range1);
                            ShapeBoundingBox collisionBoxContracted = calculateShapeBoundingBox(1, range2);
                            reduceOverlap(collisionBoxContracted, movingBoxContracted);

                            for (int i = 0; i < range1.size(); i++)
                            {
                                float newX = clamp(range1.pos[i].x, movingBoxContracted.x1, movingBoxContracted.x2);
                                float newY = clamp(range1.pos[i].y, movingBoxContracted.y1, movingBoxContracted.y2);

                                float deltaX = fabs(range1.pos[i].x - newX);
                                float deltaY = fabs(range1.pos[i].y - newY);

                                if (deltaX > 0.0f)
                                {
                                    range1.velocity[i].x *= 0.5f;
                                }

                                if (deltaY > 0.0f)
                                {
                                    range1.velocity[i].y *= 0.5f;
                                }

                                range1.pos[i].x = newX;
                                range1.pos[i].y = newY;
                            }

                            for (int i = 0; i < range2.size(); i++)
                            {
                                float newX = clamp(range2.pos[i].x, collisionBoxContracted.x1, collisionBoxContracted.x2);
                                float newY = clamp(range2.pos[i].y, collisionBoxContracted.y1, collisionBoxContracted.y2);

                                float deltaX = fabs(range2.pos[i].x - newX);
                                float deltaY = fabs(range2.pos[i].y - newY);

                                if (deltaX > 0.0f)
                                {
                                    range2.velocity[i].x *= 0.5f;
                                }

                                if (deltaY > 0.0f)
                                {
                                    range2.velocity[i].y *= 0.5f;
                                }

                                range2.pos[i].x = newX;
                                range2.pos[i].y = newY;
                            }
                        }
                        else if ((shape1.isStatic && !shape2.isStatic) || (shape2.isStatic && !shape1.isStatic))
                        {
                            PointMassesRange staticRange = shape1.isStatic ? range1 : range2;
                            PointMassesRange movingRange = shape1.isStatic ? range2 : range1;

                            ShapeBoundingBox movingBoxContracted = calculateShapeBoundingBox(1, movingRange);
                            ShapeBoundingBox staticBoxContracted = calculateShapeBoundingBox(1, staticRange);
                            reduceOverlapStatic(movingBoxContracted, staticBoxContracted);

                            for (int i = 0; i < movingRange.size(); i++)
                            {
                                movingRange.pos[i].x = clamp(movingRange.pos[i].x, movingBoxContracted.x1, movingBoxContracted.x2);
                                movingRange.pos[i].y = clamp(movingRange.pos[i].y, movingBoxContracted.y1, movingBoxContracted.y2);
                            }
                        }
                    }
                    else
                    {
                        m->collisionMap.resetCollision(box.shapeIndex, otherBox.shapeIndex);
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

void CollisionSolver::assign(CollisionSolver &other)
{
    m->collisionMap.assign(other.m->collisionMap);
    m->resolvedCollisionPairs.replace(other.m->resolvedCollisionPairs);
    m->boundingBoxes.replace(other.m->boundingBoxes);
    m->sortedBoundingBoxes.replace(other.m->sortedBoundingBoxes);
}
