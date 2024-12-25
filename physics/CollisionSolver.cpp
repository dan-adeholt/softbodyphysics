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

EdgeStrategy intersectionStrategy = EdgeStrategy::IntersectionPrevAndCurrent;

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

void CollisionSolver::findEntryEdge(
    EdgeStrategy strategy,
    PointMassesRange collisionShape,
    PointMassesRange prevCollisionShape,
    const Vector2 &currentPoint,
    const Vector2 &prevPoint,
    const Vector2 &velocity,
    int &vertexHitIndex,
    int &entryEdgeIndex,
    Vector2 &entryPoint,
    float &entryTime)
{
    switch (strategy)
    {
    default:
    case EdgeStrategy::ClosestSegment:
        findEntryEdgeClosestSegment(collisionShape, prevCollisionShape, currentPoint, prevPoint, velocity, vertexHitIndex, entryEdgeIndex, entryPoint, entryTime);
        break;
    case EdgeStrategy::RelativeVelocitySegment:
        findEntryEdgeRelativeVelocitySegment(collisionShape, prevCollisionShape, currentPoint, prevPoint, velocity, vertexHitIndex, entryEdgeIndex, entryPoint, entryTime);
    case EdgeStrategy::IntersectionPrevAndCurrent:
        findEntryEdgeIntersectionPrevAndCurrentSegment(collisionShape, prevCollisionShape, currentPoint, prevPoint, velocity, vertexHitIndex, entryEdgeIndex, entryPoint, entryTime);
        break;
    }
}

void CollisionSolver::findEntryEdgeRelativeVelocitySegment(PointMassesRange collisionShape,
                                                           PointMassesRange prevCollisionShape,
                                                           const Vector2 &currentPoint,
                                                           const Vector2 &prevPoint,
                                                           const Vector2 &velocity,
                                                           int &vertexHitIndex,
                                                           int &entryEdgeIndex,
                                                           Vector2 &entryPoint,
                                                           float &entryTime)
{
    float minDistanceSquared = __FLT_MAX__;
    int collisionShapeSize = collisionShape.pos.size;
    float closestFacing = 0.0f;

    struct Candidate
    {
        int index;
        float distanceSquared;
        float t;
        float facing;
        Vector2 closestPoint;
    };

    Candidate candidates[2] = {{0, __FLT_MAX__, 0.0f, 0.0f, Vector2()}, {0, __FLT_MAX__, 0.0f, 0.0f, Vector2()}};

    float minPointX = min(currentPoint.x, prevPoint.x);
    float minPointY = min(currentPoint.y, prevPoint.y);
    float maxPointX = max(currentPoint.x, prevPoint.x);
    float maxPointY = max(currentPoint.y, prevPoint.y);

    ShapeBoundingBox pointBox = {0, minPointX, minPointY, maxPointX, maxPointY};

    for (int i = 0; i < collisionShapeSize; i++)
    {
        int nextIndex = (i + 1) % collisionShapeSize;
        Vector2 segment0 = collisionShape.pos[i];
        Vector2 segment1 = collisionShape.pos[nextIndex];

        Vector2 prevSegment0 = prevCollisionShape.pos[i];
        Vector2 prevSegment1 = prevCollisionShape.pos[nextIndex];

        float minCornerX = min(segment0.x, prevSegment0.x);
        float minCornerY = min(segment0.y, prevSegment0.y);
        float maxCornerX = max(segment0.x, prevSegment0.x);
        float maxCornerY = max(segment0.y, prevSegment0.y);

        float distanceToVertex = (segment0 - currentPoint).length();

        ShapeBoundingBox cornerBox = {0, minCornerX, minCornerY, maxCornerX, maxCornerY};

        if (pointBox.overlaps(cornerBox) || distanceToVertex < 3.0f)
        {
            Console::log("Edge collision");
            vertexHitIndex = i;
            entryEdgeIndex = -1; // Indicate point-point collision
            return;
        }

        // Vector from A to B
        Vector2 segment = segment1 - segment0;
        // Vector from A to P
        Vector2 segmentToPoint = currentPoint - segment0;
        Vector2 segmentNormal = (segment0 - segment1).normalVector().normalized();

        // Console::addDebugVector(segment0 + segment * 0.5f, segmentNormal * 15.0f, 0x00FF00);

        Vector2 segmentAvgVelocity = (collisionShape.velocity[i] + collisionShape.velocity[nextIndex]) * 0.5f;

        // Console::addDebugVector(segment0 + segment * 0.5f, segmentAvgVelocity * 10.0f, 0x00FFFF);
        Vector2 relativeVelocity = velocity - segmentAvgVelocity;

        // Console::addDebugVector(segment0 + segment * 0.5f, relativeVelocity, 0xFF00FF);

        float facing = fabs(relativeVelocity.normalized().dot(segmentNormal));

        // // Check if the relative velocity is facing away from the segment normal
        // if (relativeVelocity.dot(segmentNormal) > 0.0f)
        // {
        //     continue;
        // }

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
        Vector2 pointToClosestPoint = closestPoint - currentPoint;

        float distanceToClosestPointActual = (pointToClosestPoint).lengthSquared();
        float distanceToClosestPointSquared = (pointToClosestPoint).lengthSquared() / facing;

        // if (logBox)
        // {
        //     Console::addDebugPoint(closestPoint, 0xFFFF00);
        //     Console::logFrame(closestPoint.x, closestPoint.y, "%.2f-%.2f-%2.f", facing, sqrtf(distanceToClosestPointActual), sqrtf(distanceToClosestPointSquared));
        // }

        if (distanceToClosestPointSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceToClosestPointSquared;
            entryEdgeIndex = i;
            entryPoint = closestPoint;
            closestFacing = facing;
            entryTime = t;
        }

        if (distanceToClosestPointSquared < candidates[0].distanceSquared)
        {
            candidates[0].distanceSquared = distanceToClosestPointSquared;
            candidates[0].index = i;
            candidates[0].t = t;
            candidates[0].facing = facing;
            candidates[0].closestPoint = closestPoint;
        }
        else if (distanceToClosestPointSquared < candidates[1].distanceSquared)
        {
            candidates[1].distanceSquared = distanceToClosestPointSquared;
            candidates[1].index = i;
            candidates[1].t = t;
            candidates[1].facing = facing;
            candidates[1].closestPoint = closestPoint;
        }
    }

    if (minDistanceSquared == __FLT_MAX__)
    {
        Console::log("ERROR: Failed to find line segment!");
    }
    else
    {
        if (!logBox)
        {
            return;
        }

        Console::addDebugVector(currentPoint, velocity, 0xFF00FF);

        int numCandidates = 0;
        for (int i = 0; i < 2; i++)
        {
            // Candidate candidate = candidates[i];

            // if (candidate.distanceSquared == __FLT_MAX__)
            // {
            //     continue;
            // }
            // numCandidates++;
            // unsigned int color = i == 0 ? 0x00FF00 : 0x0000FF;
            // Console::logFrame(candidate.closestPoint.x, candidate.closestPoint.y, "%.2f-%.2f", candidate.facing, sqrtf(candidate.distanceSquared));
            // Console::addDebugPoint(candidate.closestPoint, color);
        }
    }
}

void CollisionSolver::findEntryEdgeIntersectionPrevAndCurrentSegment(PointMassesRange collisionShape,
                                                                     PointMassesRange prevCollisionShape,
                                                                     const Vector2 &currentPoint,
                                                                     const Vector2 &prevPoint,
                                                                     const Vector2 &velocity,
                                                                     int &vertexHitIndex,
                                                                     int &entryEdgeIndex,
                                                                     Vector2 &entryPoint,
                                                                     float &entryTime)
{
    float minDistanceSquared = __FLT_MAX__;
    int collisionShapeSize = collisionShape.pos.size;

    if (logBox)
    {
        Console::addDebugPoint(currentPoint, 0xFF00FF);
        // Console::addDebugPoint(prevPoint, 0x0000FF);
        // Console::addDebugSegment(currentPoint, prevPoint, 0xFF0000);
    }

    float minPointX = min(currentPoint.x, prevPoint.x);
    float minPointY = min(currentPoint.y, prevPoint.y);
    float maxPointX = max(currentPoint.x, prevPoint.x);
    float maxPointY = max(currentPoint.y, prevPoint.y);

    ShapeBoundingBox pointBox = {0, minPointX, minPointY, maxPointX, maxPointY};

    for (int i = 0; i < collisionShapeSize; i++)
    {
        Vector2 segment0 = collisionShape.pos[i];
        Vector2 prevSegment0 = prevCollisionShape.pos[i];

        float minCornerX = min(segment0.x, prevSegment0.x);
        float minCornerY = min(segment0.y, prevSegment0.y);
        float maxCornerX = max(segment0.x, prevSegment0.x);
        float maxCornerY = max(segment0.y, prevSegment0.y);

        float cornerExtendSize = 2.0f;
        ShapeBoundingBox cornerBox = {0, minCornerX - cornerExtendSize, minCornerY - cornerExtendSize, maxCornerX + cornerExtendSize, maxCornerY + cornerExtendSize};

        // Console::addDebugSegment(Vector2(minCornerX, minCornerY), Vector2(minCornerX, maxCornerY), 0x00FF00);
        // Console::addDebugSegment(Vector2(minCornerX, maxCornerY), Vector2(maxCornerX, maxCornerY), 0x00FF00);
        // Console::addDebugSegment(Vector2(maxCornerX, maxCornerY), Vector2(maxCornerX, minCornerY), 0x00FF00);
        // Console::addDebugSegment(Vector2(maxCornerX, minCornerY), Vector2(minCornerX, minCornerY), 0x00FF00);

        if (pointBox.overlaps(cornerBox))
        {
            vertexHitIndex = i;
            entryEdgeIndex = -1; // Indicate point-point collision
            return;
        }
    }

    for (int i = 0; i < collisionShapeSize; i++)
    {
        int nextIndex = (i + 1) % collisionShapeSize;
        Vector2 segment0 = collisionShape.pos[i];
        Vector2 segment1 = collisionShape.pos[nextIndex];

        Vector2 prevSegment0 = prevCollisionShape.pos[i];
        Vector2 prevSegment1 = prevCollisionShape.pos[nextIndex];

        float t_point, t_edge;
        Vector2 movement = currentPoint - prevPoint;

        if (lineSegmentIntersection(segment0, segment1, prevPoint, currentPoint, t_point, t_edge) || lineSegmentIntersection(prevSegment0, prevSegment1, prevPoint, currentPoint, t_point, t_edge))
        {

            // Console::addDebugSegment(segment0, segment1, 0x00FF00);
            // Console::addDebugSegment(prevSegment0, prevSegment1, 0x0000FF);
            // Console::addDebugSegment(segment0, prevSegment0, 0x00FF00);
            // Console::addDebugSegment(prevSegment1, segment1, 0x0000FF);

            // Console::addDebugSegment(prevPoint, currentPoint, 0xFF0000);

            Vector2 segment = segment1 - segment0;
            Vector2 segmentToPoint = currentPoint - segment0;
            Vector2 segmentNormal = segment.normalVector().normalized();

            float t = segmentToPoint.dot(segment) / segment.dot();
            t = clamp(t, 0.0f, 1.0f);

            Vector2 closestPoint = segment0 + segment * t;
            Vector2 pointToClosestPoint = closestPoint - currentPoint;

            entryEdgeIndex = i;
            entryPoint = closestPoint;
            entryTime = t;

            // Console::addDebugPoint(entryPoint, 0xFF00FF);
            // Console::logFrame(entryPoint.x, entryPoint.y, "2 - %.2f", t_edge);

            return;
        }
    }

    return findEntryEdgeClosestSegment(collisionShape, prevCollisionShape, currentPoint, prevPoint, velocity, vertexHitIndex, entryEdgeIndex, entryPoint, entryTime);
    Console::log("Failed to find intersection");
}

void CollisionSolver::findEntryEdgeClosestSegment(PointMassesRange collisionShape,
                                                  PointMassesRange prevCollisionShape,
                                                  const Vector2 &currentPoint,
                                                  const Vector2 &prevPoint,
                                                  const Vector2 &velocity,
                                                  int &vertexHitIndex,
                                                  int &entryEdgeIndex,
                                                  Vector2 &entryPoint,
                                                  float &entryTime)
{
    float minDistanceSquared = __FLT_MAX__;
    int collisionShapeSize = collisionShape.pos.size;
    for (int i = 0; i < collisionShapeSize; i++)
    {
        Vector2 segment0 = collisionShape.pos[i];
        Vector2 segment1 = collisionShape.pos[(i + 1) % collisionShapeSize];

        float distanceToVertex = (segment0 - currentPoint).length();

        if (distanceToVertex < 3.0f)
        {
            vertexHitIndex = i;
            entryEdgeIndex = -1; // Indicate point-point collision
            return;
        }

        // Vector from A to B
        Vector2 segment = segment1 - segment0;
        // Vector from A to P
        Vector2 segmentToPoint = currentPoint - segment0;
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
        Vector2 pointToClosestPoint = closestPoint - currentPoint;

        float distanceToClosestPointSquared = (pointToClosestPoint).lengthSquared();

        if (distanceToClosestPointSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceToClosestPointSquared;
            entryEdgeIndex = i;
            entryPoint = closestPoint;
            entryTime = t;
        }
    }

    if (minDistanceSquared == __FLT_MAX__)
    {
        Console::log("ERROR: Failed to find line segment!");
    }
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

int CollisionSolver::calculateCollisions(
    PointMassesRange collisionShape,
    PointMassesRange movingShape,
    PointMassesRange prevCollisionShape,
    PointMassesRange prevMovingShape,
    const ShapeBoundingBox &collisionBox,
    const ShapeBoundingBox &movingBox,
    bool isStaticCollisionShape,
    bool isStaticMovingShape,
    EdgeStrategy strategy)
{
    if (isStaticMovingShape)
    {
        return 0;
    }

    int numCollisions = 0;

    // if (prevCollisionShape.size() == 4)
    // {
    //     Console::addDebugQuad(prevCollisionShape.pos[0], prevCollisionShape.pos[1], prevCollisionShape.pos[2], prevCollisionShape.pos[3], 0x00FF00);
    // }

    // if (prevMovingShape.size() == 4)
    // {
    //     Console::addDebugQuad(prevMovingShape.pos[0], prevMovingShape.pos[1], prevMovingShape.pos[2], prevMovingShape.pos[3], 0xFF0000);
    // }

    for (int i = 0; i < movingShape.pos.size; i++)
    {
        Vector2 pointPos = movingShape.pos[i];
        Vector2 prevPointPos = prevMovingShape.pos[i];
        Vector2 pointVelocity = movingShape.velocity[i];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (isPointOutsideShape(pointPos.x, pointPos.y, collisionBox, collisionShape))
        {

            continue;
        }

        numCollisions++;

        int vertexHitIndex = -1;
        int minIndex = -1;
        float minT = 0.0f;
        Vector2 minPoint = {0.0f, 0.0f};

        logBox = movingBox.shapeIndex == 0 && i == 2;

        // Console::log("** Find entry edge for point %d in %d **", i, movingBox.shapeIndex);

        findEntryEdge(strategy, collisionShape, prevCollisionShape, pointPos, prevPointPos, pointVelocity, vertexHitIndex, minIndex, minPoint, minT);

        // Console::log("[%d] Collision point %s: %.2f %.2f", logBox, vertexHitIndex == -1 ? "Segment" : "Corner", minPoint.x, minPoint.y);

        if (vertexHitIndex != -1)
        {
            float pointMass = movingShape.mass[i];
            Vector2 collisionPoint = collisionShape.pos[vertexHitIndex];
            Vector2 collisionOffset = pointPos - collisionPoint;
            Vector2 collisionNormal = collisionOffset.normalized();

            // Prevent division by zero
            if (collisionOffset.isZero())
            {
                collisionNormal = (-collisionShape.velocity[vertexHitIndex]).normalized();

                if (collisionNormal.isZero())
                {
                    collisionNormal = Vector2(1.0f, 0.0f);
                }
            }

            // Console::addDebugPoint(pointPos, 0xFF00FF);
            // Console::addDebugVector(pointPos, collisionPoint - pointPos, 0xFF00FF);
            // Console::addDebugPoint(collisionPoint, 0xFF00FF);

            if (isStaticCollisionShape)
            {
                Vector2 impulse = calculateImpulseStatic(pointVelocity.x, pointVelocity.y, pointMass, collisionNormal);

                Vector2 reflection = pointVelocity.reflect(collisionNormal).normalized();
                movingShape.velocity[i] += impulse / pointMass;
                movingShape.pos[i] = collisionPoint + reflection * 0.1f;
            }
            else
            {
                float pm0Mass = collisionShape.mass[vertexHitIndex];
                Vector2 impulse = calculateImpulsePointToPoint(
                    collisionShape.velocity[vertexHitIndex].x,
                    collisionShape.velocity[vertexHitIndex].y,
                    pm0Mass,
                    collisionNormal,
                    pointVelocity.x,
                    pointVelocity.y, pointMass);

                Vector2 pm0Acceleration = (impulse) / pm0Mass;
                collisionShape.velocity[vertexHitIndex] -= pm0Acceleration;

                Vector2 reflection = pointVelocity.reflect(collisionNormal).normalized();
                Vector2 oldPos = movingShape.pos[i];

                bool wasNan = isnan(movingShape.velocity[i].x);
                movingShape.velocity[i] += impulse / pointMass;

                // if (!wasNan && isnan(movingShape.velocity[i].x))
                // {
                //     Console::log("NAN velocity after collision %f", impulse);
                // }

                movingShape.pos[i] = collisionPoint + reflection * 0.1f;
            }
        }
        else if (minIndex != -1)
        {
            // Console::addDebugPoint(pointPos, 0xFF00FF);
            // Console::addDebugVector(pointPos, minPoint - pointPos, 0xFF00FF);
            // Console::addDebugPoint(minPoint, 0xFF00FF);
            float pointMass = movingShape.mass[i];
            Vector2 pm0Pos = collisionShape.pos[minIndex];
            Vector2 pm1Pos = collisionShape.pos[(minIndex + 1) % collisionShape.pos.size];

            // if (!isStaticCollisionShape)
            // {
            //     Console::log("Collision min segment: %d-%d on shape %d [%d-%d] mvel: %.2f %.2f", minIndex, (minIndex + 1) % collisionShape.pos.size, collisionBox.shapeIndex, movingBox.shapeIndex, i, movingShape.velocity[i].x, movingShape.velocity[i].y);
            // }

            Vector2 segmentNormal = Vector2(-pm1Pos.y + pm0Pos.y, pm1Pos.x - pm0Pos.x).normalized();

            if (isStaticCollisionShape)
            {
                // Since this is an impulse and not a continuously applied force, we need to divide by the step
                // so that when the velocity and position are updated, the impulse is applied correctly.
                Vector2 impulse = calculateImpulseStatic(pointVelocity.x, pointVelocity.y, pointMass, segmentNormal);

                Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();
                movingShape.velocity[i] += impulse / pointMass;
                movingShape.pos[i] = minPoint + reflection * 0.1f;
                // Console::logCollisionImpulse((impulse / pointMass) * 100.0f, movingShape.pos[i]);
            }
            else
            {
                float pm0Mass = collisionShape.mass[minIndex];
                Vector2 pm0Vel = collisionShape.velocity[minIndex];
                float pm1Mass = collisionShape.mass[(minIndex + 1) % collisionShape.pos.size];
                Vector2 pm1Vel = collisionShape.velocity[(minIndex + 1) % collisionShape.pos.size];

                Vector2 impulse = calculateImpulse(pm0Vel.x, pm0Vel.y, pm0Mass, pm1Vel.x, pm1Vel.y, pm1Mass, segmentNormal, pointVelocity.x, pointVelocity.y, pointMass, minT);

                Vector2 pm0Acceleration = (impulse * (1.0f - minT)) / pm0Mass;
                collisionShape.velocity[minIndex] -= pm0Acceleration;
                Vector2 pm1Acceleration = (impulse * minT) / pm1Mass;
                collisionShape.velocity[(minIndex + 1) % collisionShape.pos.size] -= pm1Acceleration;

                Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();
                Vector2 oldPos = movingShape.pos[i];

                movingShape.velocity[i] += impulse / pointMass;
                movingShape.pos[i] = minPoint + reflection * 0.1f;

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

                    if (numCollisions < 2048)
                    {
                        calculateCollisions(
                            space.points.range(shape1), space.points.range(shape2),
                            prevSpace.points.range(shape1), prevSpace.points.range(shape2),
                            box, otherBox, shape1.isStatic, shape2.isStatic, intersectionStrategy);
                        calculateCollisions(space.points.range(shape2), space.points.range(shape1),
                                            prevSpace.points.range(shape2), prevSpace.points.range(shape1),
                                            otherBox, box, shape2.isStatic, shape1.isStatic, intersectionStrategy);

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

const char *edgeStrategyToString(EdgeStrategy strategy)
{
    switch (strategy)
    {
    case EdgeStrategy::ClosestSegment:
        return "ClosestSegment";
    case EdgeStrategy::RelativeVelocitySegment:
        return "RelativeVelocitySegment";
    case EdgeStrategy::IntersectionPrevAndCurrent:
        return "IntersectionPrevAndCurrent";
    default:
        return "Unknown";
    }
}
