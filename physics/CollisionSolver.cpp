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
#include "ShapeUtils.h"

float coefficentOfRestitution = 0.65f;

// EdgeStrategy intersectionStrategy = EdgeStrategy::ClosestSegment;

bool logBox = true;

static const Vector2 K_DOP_AXES[4] = {
    {1.0f, 0.0f},       // X axis
    {0.0f, 1.0f},       // Y axis
    {0.7071f, 0.7071f}, // Diagonal (1,1) normalized
    {0.7071f, -0.7071f} // Diagonal (1,-1) normalized
};

KDOPProjection computeKDOPProjection(const PointMassesRange &points, int shapeIndex, const Shape &shape)
{
    KDOPProjection proj;
    ShapeIndexedRange range(shape);
    int count = range.size();

    for (int i = 0; i < 4; ++i)
    {
        float minProj = __FLT_MAX__;
        float maxProj = -__FLT_MAX__;
        const Vector2 &axis = K_DOP_AXES[i];

        for (int j = 0; j < count; ++j)
        {
            Vector2 point = points.pos[range[j]];
            float p = point.dot(axis);
            if (p < minProj)
                minProj = p;
            if (p > maxProj)
                maxProj = p;
        }

        proj.min[i] = minProj;
        proj.max[i] = maxProj;
    }
    return proj;
}

bool kdopOverlap(const KDOPProjection &a, const KDOPProjection &b)
{
    for (int i = 0; i < 4; ++i)
    {
        if (a.max[i] < b.min[i] || b.max[i] < a.min[i])
            return false; // Separating axis found
    }
    return true;
}

OrientedBoundingBox CollisionSolver::calculateOrientedBoundingBox(const PointMassesRange &points, int shapeIndex, const Shape &shape)
{

    ShapeIndexedRange range(shape);
    int count = range.size();

    Vector2 longest = points.pos[range[1]] - points.pos[range[0]];
    float maxLenSq = longest.lengthSquared();

    for (int j = 1; j < range.size(); j++)
    {
        int nextIndex = (j + 1) % count;
        Vector2 edge = points.pos[range[nextIndex]] - points.pos[range[j]];
        float lenSq = edge.lengthSquared();
        if (lenSq > maxLenSq)
        {
            maxLenSq = lenSq;
            longest = edge;
        }
    }

    Vector2 axisX = longest.normalized();
    Vector2 axisY = axisX.normalVector();

    float minX = __FLT_MAX__, maxX = -__FLT_MAX__;
    float minY = __FLT_MAX__, maxY = -__FLT_MAX__;

    for (int i = 0; i < count; ++i)
    {
        const Vector2 &p = points.pos[range[i]];

        float projX = p.dot(axisX);
        float projY = p.dot(axisY);

        if (projX < minX)
            minX = projX;
        if (projX > maxX)
            maxX = projX;
        if (projY < minY)
            minY = projY;
        if (projY > maxY)
            maxY = projY;
    }

    float centerProjX = (minX + maxX) * 0.5f;
    float centerProjY = (minY + maxY) * 0.5f;
    Vector2 center = axisX * centerProjX + axisY * centerProjY;

    OrientedBoundingBox box;
    box.center = center;
    box.axisX = axisX;
    box.axisY = axisY;
    box.halfX = (maxX - minX) * 0.5f;
    box.halfY = (maxY - minY) * 0.5f;
    return box;
}

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

    ShapeBoundingBox box;
    box.x1 = minX;
    box.y1 = minY;
    box.x2 = maxX;
    box.y2 = maxY;
    box.shapeIndex = shapeIndex;

    return box;
}
struct CollisionCell
{
    int boxIndex;
    bool isStatic;
    int next;
};

struct CollisionPair
{
    int shape1Index;
    int shape2Index;
};
struct CollisionSolver::Impl
{
    Impl() : collisionMap(0),
             gridCollisionMap(0),
             boundingBoxes(0),
             sortedBoundingBoxes(0),
             sortedBoundingBoxShapeIndices(0)
    {
    }

    CollisionMap collisionMap;
    CollisionMap gridCollisionMap;
    Array<ShapeBoundingBox> boundingBoxes;
    Array<OrientedBoundingBox> orientedBoundingBoxes;
    Array<KDOPProjection> kdopProjections;
    Array<BoundingBox> sortedBoundingBoxes;
    Array<int> sortedBoundingBoxShapeIndices;
    CollisionGridSimple collisionGridSimple;
    bool useCollisionGrid = false;
};

float frictionCoefficient = 0.9f;

static WheelMotor *findWheelMotor(PhysicsSpace *space, int shapeIndex)
{
    if (space == nullptr)
    {
        return nullptr;
    }

    for (int i = 0; i < space->wheelMotors.size(); i++)
    {
        WheelMotor &wheelMotor = space->wheelMotors[i];
        if (wheelMotor.shapeIndex == shapeIndex)
        {
            return &wheelMotor;
        }
    }

    return nullptr;
}

static void resetWheelMotorState(PhysicsSpace *space)
{
    if (space == nullptr)
    {
        return;
    }

    for (int i = 0; i < space->wheelMotors.size(); i++)
    {
        WheelMotor &wheelMotor = space->wheelMotors[i];
        wheelMotor.groundedThisStep = false;
        wheelMotor.groundedContactCount = 0;
        wheelMotor.groundedTangentSum = Vector2();
        wheelMotor.groundedGroundVelocitySum = Vector2();
        wheelMotor.lastSurfaceSpeed = 0.0f;
        wheelMotor.lastSurfaceSpeedError = 0.0f;
        wheelMotor.lastAppliedImpulse = 0.0f;
        wheelMotor.lastParentForwardSpeed = 0.0f;
        wheelMotor.lastGroundSpeed = 0.0f;
        wheelMotor.lastRelativeForwardSpeed = 0.0f;
        wheelMotor.lastCommandSpaceSpeed = 0.0f;
        wheelMotor.lastAuthorityClamp = 0.0f;
        wheelMotor.lastHandoverBand = 0.0f;
        wheelMotor.lastMode = WheelMotorMode::Coast;
    }
}

static void recordWheelMotorContact(WheelMotor &wheelMotor, Vector2 tangent, Vector2 groundVelocity)
{
    wheelMotor.groundedThisStep = true;
    wheelMotor.groundedContactCount++;
    wheelMotor.groundedTangentSum += tangent;
    wheelMotor.groundedGroundVelocitySum += groundVelocity;
}

static void applyWheelMotorImpulseStatic(PhysicsSpace *space,
                                         const Shape &movingShape,
                                         const Vector2 &contactPoint,
                                         int pointIndex0,
                                         int pointIndex1,
                                         float pointMass,
                                         Vector2 segmentNormal,
                                         PointMassesRange points)
{
    (void)contactPoint;
    (void)pointIndex0;
    (void)pointIndex1;
    (void)pointMass;
    (void)points;
    WheelMotor *wheelMotor = findWheelMotor(space, movingShape.index);
    if (wheelMotor == nullptr)
    {
        return;
    }

    recordWheelMotorContact(*wheelMotor, segmentNormal.normalVector(), Vector2());
}

static void applyWheelMotorImpulseDynamic(PhysicsSpace *space,
                                          const Shape &movingShape,
                                          const Vector2 &contactPoint,
                                          int pointIndex0,
                                          int pointIndex1,
                                          float pointMass,
                                          int collisionIndex0,
                                          int collisionIndex1,
                                          float pm0Mass,
                                          const Vector2 &pm0Velocity,
                                          float pm1Mass,
                                          const Vector2 &pm1Velocity,
                                          float contactT,
                                          Vector2 segmentNormal,
                                          PointMassesRange points)
{
    (void)contactPoint;
    (void)pointIndex0;
    (void)pointIndex1;
    (void)pointMass;
    (void)collisionIndex0;
    (void)collisionIndex1;
    (void)pm0Mass;
    (void)pm1Mass;
    (void)points;
    WheelMotor *wheelMotor = findWheelMotor(space, movingShape.index);
    if (wheelMotor == nullptr)
    {
        return;
    }

    Vector2 groundVelocity = pm0Velocity + (pm1Velocity - pm0Velocity) * contactT;
    recordWheelMotorContact(*wheelMotor, segmentNormal.normalVector(), groundVelocity);
}

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

    // Normal impulse vector
    Vector2 normalImpulse = segmentNormal * impulseMagnitude;

    // -------- Friction impulse --------

    // Tangent = perp(normal)
    Vector2 tangent = segmentNormal.normalVector();

    float tangentVelocity = Vector2::vec2dot(relativeVelocityX, relativeVelocityY, tangent.x, tangent.y);
    float tangentImpulseMag = -tangentVelocity / inverseMass;

    // Clamp friction impulse using Coulomb friction
    float maxFriction = fabs(impulseMagnitude) * frictionCoefficient;
    tangentImpulseMag = clamp(tangentImpulseMag, -maxFriction, maxFriction);

    Vector2 frictionImpulse = tangent * tangentImpulseMag;

    return normalImpulse + frictionImpulse;
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

    Vector2 normalImpulse = segmentNormal * impulseMagnitude;

    // -------- Friction impulse --------
    Vector2 relativeVelocity(relativeVelocityX, relativeVelocityY);

    Vector2 tangent(segmentNormal.normalVector());
    float tangentVelocity = Vector2::vec2dot(relativeVelocityX, relativeVelocityY, tangent.x, tangent.y);
    float tangentImpulseMag = -tangentVelocity / inverseMass;

    float maxFriction = fabs(impulseMagnitude) * frictionCoefficient;
    tangentImpulseMag = clamp(tangentImpulseMag, -maxFriction, maxFriction);

    Vector2 frictionImpulse = tangent * tangentImpulseMag;

    return normalImpulse + frictionImpulse;
}

void sortBoundingBoxes(Array<BoundingBox> &sortedBoundingBoxes, Array<int> &sortedBoundingBoxShapeIndices)
{
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    for (int i = 1; i < sortedBoundingBoxes.size(); i++)
    {
        BoundingBox item = sortedBoundingBoxes[i];
        int shapeIndex = sortedBoundingBoxShapeIndices[i];
        int j = i - 1;
        while (j >= 0 && sortedBoundingBoxes[j].x1 > item.x1)
        {
            sortedBoundingBoxes[j + 1] = sortedBoundingBoxes[j];
            sortedBoundingBoxShapeIndices[j + 1] = sortedBoundingBoxShapeIndices[j];
            j--;
        }
        sortedBoundingBoxes[j + 1] = item;
        sortedBoundingBoxShapeIndices[j + 1] = shapeIndex;
    }
}

void updateSortedBoundingBoxes(Array<BoundingBox> &sortedBoundingBoxes, Array<ShapeBoundingBox> &boundingBoxes, Array<int> &sortedBoundingBoxShapeIndices)
{
    for (int i = 0; i < sortedBoundingBoxes.size(); i++)
    {
        BoundingBox &sortedBox = sortedBoundingBoxes[i];
        const ShapeBoundingBox &box = boundingBoxes[sortedBoundingBoxShapeIndices[i]];
        sortedBox.x1 = box.x1;
        sortedBox.y1 = box.y1;
        sortedBox.x2 = box.x2;
        sortedBox.y2 = box.y2;
    }
}

// Function to calculate the intersection point of two line segments
inline IntersectionResult lineIntersection(const Vector2 &s1, const Vector2 &s2, const Vector2 &p1, const Vector2 &p2)
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

bool isPointInPolygon(int index, const Vector2 &pt, const PointMassesRange &points, const Shape &shape)
{
    bool inside = false;
    ShapeIndexedRange polygon(shape);
    int n = polygon.size();

    // Loop over each edge of the polygon
    for (int i = 0, j = n - 1; i < n; j = i++)
    {
        if (index == polygon[i])
        {
            return false;
        }

        // Check if point's y is between the y-coordinates of the edge endpoints
        if (((points.pos[polygon[i]].y > pt.y) != (points.pos[polygon[j]].y > pt.y)))
        {
            // Compute the x coordinate where the line through the edge intersects the horizontal line at pt.y
            float intersectX = (points.pos[polygon[j]].x - points.pos[polygon[i]].x) * (pt.y - points.pos[polygon[i]].y) /
                                   (points.pos[polygon[j]].y - points.pos[polygon[i]].y) +
                               points.pos[polygon[i]].x;
            // If the intersection point is to the right of pt, toggle the inside flag
            if (pt.x < intersectX)
                inside = !inside;
        }
    }

    return inside;
}

bool shapesOverlapSimple(const PointMassesRange &points, const Shape &shape1, const Shape &shape2)
{
    ShapeIndexedRange poly1(shape1);
    ShapeIndexedRange poly2(shape2);

    for (int i = 0; i < poly1.size(); ++i)
    {
        int p1 = poly1[i];
        Vector2 point(points.pos[p1]);
        if (isPointInPolygon(p1, point, points, shape2))
        {
            return true;
        }
    }

    for (int i = 0; i < poly2.size(); ++i)
    {
        int p1 = poly2[i];
        Vector2 point(points.pos[p1]);
        if (isPointInPolygon(p1, point, points, shape1))
        {
            return true;
        }
    }

    return false;
}

bool shapesOverlap(const PointMassesRange &points, const Shape &shape1, const Shape &shape2)
{
    ShapeIndexedRange poly1(shape1);
    ShapeIndexedRange poly2(shape2);

    bool hasInteriorEdge = false;

    for (int i = 0; i < poly1.size(); ++i)
    {
        int p1 = poly1[i];
        const Vector2 &point1(points.pos[p1]);
        int p2 = poly1[(i + 1) % poly1.size()];

        for (int j = 0; j < poly2.size(); ++j)
        {
            int p3 = poly2[j];
            int p4 = poly2[(j + 1) % poly2.size()];

            bool found = lineIntersection(point1, points.pos[p2], points.pos[p3], points.pos[p4]).found;

            bool hasSharedEdge = p1 == p3 || p1 == p4 || p2 == p3 || p2 == p4;

            if (found && !hasSharedEdge)
            {
                return true;
            }
        }
    }

    for (int i = 0; i < poly1.size(); ++i)
    {
        int p1 = poly1[i];
        const Vector2 &point1(points.pos[p1]);

        if (isPointInPolygon(p1, point1, points, shape2))
        {
            return true;
        }
    }

    for (int i = 0; i < poly2.size(); ++i)
    {
        int p1 = poly2[i];
        Vector2 point(points.pos[p1]);
        if (isPointInPolygon(p1, point, points, shape1))
        {
            return true;
        }
    }

    return false;
}

ClosestSegmentResult CollisionSolver::findEntryEdgeClosestSegment(PointMassesRange points,
                                                                  const Shape &collisionShape,
                                                                  const Vector2 &currentPoint)
{
    float minDistanceSquared = __FLT_MAX__;

    ClosestSegmentResult result = {-1, Vector2(), 0.0f};
    const ShapeIndexedRange collisionShapeRange(collisionShape);

    for (int i = 0; i < collisionShapeRange.size(); i++)
    {
        Vector2 segment0 = points.pos[collisionShapeRange[i]];
        Vector2 segment1 = points.pos[collisionShapeRange[(i + 1) % collisionShapeRange.size()]];

        if (collisionShapeRange.hasInteriorEdge(i))
        {
            continue;
        }

        float distanceToVertex = (segment0 - currentPoint).length();

        // Vector from A to B
        Vector2 segment = segment1 - segment0;
        // Vector from A to P
        Vector2 segmentToPoint = currentPoint - segment0;
        Vector2 segmentNormal = segment.normalVector().normalized();
        Vector2 pointOutside = (segment0 + segment * 0.5f) - segmentNormal * 2.0f;

        // float t_point, t_edge;

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
        }
    }

    return result;
}

// PointMassesRange collisionShape, float pointX, float pointY, float outX
bool CollisionSolver::isPointOutsideShape(int pointIndex, float pointX, float pointY, const BoundingBox &box, const PointMassesRange &points, const Shape &collisionShape)
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

static int calculateCollisionsMidPointInternal(
    PhysicsSpace *space,
    PointMassesRange points,
    const Shape &collisionShape,
    const Shape &movingShape,
    const BoundingBox &collisionBox,
    const BoundingBox &movingBox)
{
    if (movingShape.isStatic && collisionShape.isStatic)
    {
        return 0;
    }

    int numCollisions = 0;

    const ShapeIndexedRange movingRange(movingShape);
    const ShapeIndexedRange collisionRange(collisionShape);
    int commonEdge = collisionShape.commonEdge(movingShape);

    for (int i = 0; i < movingRange.size(); i++)
    {
        int prevIndex = (i == 0 ? movingRange.size() - 1 : i - 1);
        int nextIndex = (i + 1) % movingRange.size();
        int pointIndex = movingRange[i];
        int nextPointIndex = movingRange[nextIndex];

        Vector2 prevPos = points.pos[movingRange[prevIndex]];
        Vector2 nextPos = points.pos[movingRange[nextIndex]];

        Vector2 pointToNext = nextPos - points.pos[pointIndex];
        Vector2 pointPos = points.pos[pointIndex] + pointToNext * 0.5f;

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (CollisionSolver::isPointOutsideShape(pointIndex, pointPos.x, pointPos.y, collisionBox, points, collisionShape))
        {
            continue;
        }

        numCollisions++;

        // Console::log("** Find entry edge for point %d in %d **", i, movingBox.shapeIndex);

        ClosestSegmentResult result = {-1, Vector2(), 0.0f};

        if (commonEdge != -1)
        {
            Vector2 segment0 = points.pos[collisionRange[commonEdge]];
            Vector2 segment1 = points.pos[collisionRange[(commonEdge + 1) % collisionRange.size()]];

            float distanceToVertex = (segment0 - pointPos).length();

            // Vector from A to B
            Vector2 segment = segment1 - segment0;
            // Vector from A to P
            Vector2 segmentToPoint = pointPos - segment0;
            // Compute projection t
            float t = segmentToPoint.dot(segment) / segment.dot();
            t = clamp(t, 0.0f, 1.0f);
            Vector2 closestPoint = segment0 + segment * t;
            result.closestPoint0 = closestPoint;
            result.entryEdgeIndex0 = commonEdge;
            result.entryTime0 = t;
        }
        else
        {
            result = CollisionSolver::findEntryEdgeClosestSegment(points, collisionShape, pointPos);
        }

        if (result.entryEdgeIndex0 == -1)
        {
            continue;
        }

        Vector2 pointVelocity = (points.velocity[pointIndex] + points.velocity[nextPointIndex]) * 0.5f;

        int collisionIndex0 = collisionRange[result.entryEdgeIndex0];
        int collisionIndex1 = collisionRange[(result.entryEdgeIndex0 + 1) % collisionRange.size()];

        float pointMass = (points.mass[pointIndex] + points.mass[nextPointIndex]) * 0.5f;

        Vector2 pm0Pos = points.pos[collisionIndex0];
        Vector2 pm1Pos = points.pos[collisionIndex1];
        Vector2 segmentNormal = Vector2(-pm1Pos.y + pm0Pos.y, pm1Pos.x - pm0Pos.x).normalized();

        if (collisionShape.isStatic)
        {
            Vector2 impulse = calculateImpulseStatic(pointVelocity.x, pointVelocity.y, pointMass, segmentNormal);
            Vector2 reflection = pointVelocity.reflect(segmentNormal).normalized();

            Vector2 splitOffset = result.closestPoint0 - pointPos;
            points.pos[pointIndex] += splitOffset + reflection * 0.1f;
            points.pos[nextPointIndex] += splitOffset + reflection * 0.1f;

            points.velocity[pointIndex] += (impulse / pointMass) * 0.5f;
            points.velocity[nextPointIndex] += (impulse / pointMass) * 0.5f;
            applyWheelMotorImpulseStatic(space, movingShape, result.closestPoint0, pointIndex, nextPointIndex, pointMass, segmentNormal, points);
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
                applyWheelMotorImpulseDynamic(space, movingShape, result.closestPoint0, pointIndex, nextPointIndex, pointMass, collisionIndex0, collisionIndex1, pm0Mass, pm0Vel, pm1Mass, pm1Vel, result.entryTime0, segmentNormal, points);
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

int CollisionSolver::calculateCollisionsMidPoint(
    PointMassesRange points,
    const Shape &collisionShape,
    const Shape &movingShape,
    const BoundingBox &collisionBox,
    const BoundingBox &movingBox)
{
    return calculateCollisionsMidPointInternal(nullptr, points, collisionShape, movingShape, collisionBox, movingBox);
}

static int calculateCollisionsInternal(
    PhysicsSpace *space,
    PointMassesRange points,
    const Shape &collisionShape,
    const Shape &movingShape,
    const BoundingBox &collisionBox,
    const BoundingBox &movingBox)
{
    if (collisionShape.isStatic && movingShape.isStatic)
    {
        return 0;
    }

    int numCollisions = 0;

    ShapeIndexedRange movingRange(movingShape);
    ShapeIndexedRange collisionRange(collisionShape);
    int commonEdge = collisionShape.commonEdge(movingShape);

    for (int i = 0; i < movingRange.size(); i++)
    {
        int pointIndex = movingRange[i];
        Vector2 pointPos = points.pos[pointIndex];
        Vector2 prevPos = points.pos[movingRange[i == 0 ? movingRange.size() - 1 : i - 1]];
        Vector2 nextPos = points.pos[movingRange[(i + 1) % movingRange.size()]];

        // First check - is the point outside the bounding box of the other shape?
        // Then extend horizontal line from point to the right,  outside of bounding box.
        if (CollisionSolver::isPointOutsideShape(pointIndex, pointPos.x, pointPos.y, collisionBox, points, collisionShape))
        {
            continue;
        }

        numCollisions++;

        ClosestSegmentResult result = {-1, Vector2(), 0.0f};

        if (commonEdge != -1)
        {
            Vector2 segment0 = points.pos[collisionRange[commonEdge]];
            Vector2 segment1 = points.pos[collisionRange[(commonEdge + 1) % collisionRange.size()]];

            float distanceToVertex = (segment0 - pointPos).length();

            // Vector from A to B
            Vector2 segment = segment1 - segment0;
            // Vector from A to P
            Vector2 segmentToPoint = pointPos - segment0;
            // Compute projection t
            float t = segmentToPoint.dot(segment) / segment.dot();
            t = clamp(t, 0.0f, 1.0f);
            Vector2 closestPoint = segment0 + segment * t;
            result.closestPoint0 = closestPoint;
            result.entryEdgeIndex0 = commonEdge;
            result.entryTime0 = t;
        }
        else
        {
            result = CollisionSolver::findEntryEdgeClosestSegment(points, collisionShape, pointPos);
        }

        if (result.entryEdgeIndex0 == -1)
        {
            continue;
        }

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
            Vector2 oldPos = points.pos[pointIndex];
            points.pos[pointIndex] = result.closestPoint0 + reflection * 0.1f;
            applyWheelMotorImpulseStatic(space, movingShape, result.closestPoint0, pointIndex, -1, pointMass, segmentNormal, points);
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
                Vector2 oldPos = points.pos[pointIndex];
                points.pos[pointIndex] = result.closestPoint0 + reflection * 0.1f;
                float distance = (points.pos[pointIndex] - oldPos).length();

                points.velocity[pointIndex] += impulse / pointMass;
                applyWheelMotorImpulseDynamic(space, movingShape, result.closestPoint0, pointIndex, -1, pointMass, collisionIndex0, collisionIndex1, pm0Mass, pm0Vel, pm1Mass, pm1Vel, result.entryTime0, segmentNormal, points);
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

int CollisionSolver::calculateCollisions(
    PointMassesRange points,
    const Shape &collisionShape,
    const Shape &movingShape,
    const BoundingBox &collisionBox,
    const BoundingBox &movingBox)
{
    return calculateCollisionsInternal(nullptr, points, collisionShape, movingShape, collisionBox, movingBox);
}

void CollisionSolver::calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes,
                                             Array<OrientedBoundingBox> &orientedBoundingBoxes,
                                             Array<KDOPProjection> &projections,
                                             const Array<Shape> &shapes, const PointMasses &points)
{
    boundingBoxes.clear();
    orientedBoundingBoxes.clear();
    projections.clear();

    for (int i = 0; i < shapes.size(); i++)
    {
        boundingBoxes.push(calculateShapeBoundingBox(points.range(), i, shapes[i]));
        orientedBoundingBoxes.push(calculateOrientedBoundingBox(points.range(), i, shapes[i]));
        projections.push(computeKDOPProjection(points.range(), i, shapes[i]));
    }
}

Array<ShapeBoundingBox> &CollisionSolver::boundingBoxes()
{
    return m->boundingBoxes;
}

Array<OrientedBoundingBox> &CollisionSolver::orientedBoundingBoxes()
{
    return m->orientedBoundingBoxes;
}

CollisionGridSimple &CollisionSolver::grid()
{
    return m->collisionGridSimple;
}

void CollisionSolver::toggleCollisionGrid()
{
    m->useCollisionGrid = !m->useCollisionGrid;
}

void CollisionSolver::getCollisionCandidates(int shapeIndex, Array<int> &candidates, PhysicsSpace &space)
{
    candidates.clear();
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
    {
        const BoundingBox &box = m->sortedBoundingBoxes[i];

        for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
        {
            const BoundingBox &otherBox = m->sortedBoundingBoxes[j];

            if (otherBox.x1 >= box.x2)
            {
                break;
            }
            // Due to the sorted nature of the bounding boxes, we already know that
            // otherBox.x1 >= box.x1, so we only need to check the other axis
            else if (otherBox.y1 >= box.y2 || otherBox.y2 <= box.y1)
            {
                continue;
            }
            Timer calculateCollisionsTimer;

            int shapeIndex1 = m->sortedBoundingBoxShapeIndices[i];
            int shapeIndex2 = m->sortedBoundingBoxShapeIndices[j];

            if (shapeIndex1 != shapeIndex && shapeIndex2 != shapeIndex)
            {
                continue; // Skip if the shape is the same as the one we are checking
            }
            const Shape &shape1 = space.shapes[shapeIndex1];
            const Shape &shape2 = space.shapes[shapeIndex2];

            const OrientedBoundingBox &orientedBox1 = m->orientedBoundingBoxes[shapeIndex1];
            const OrientedBoundingBox &orientedBox2 = m->orientedBoundingBoxes[shapeIndex2];

            if (!orientedBox1.overlaps(orientedBox2))
            {
                // Oriented bounding boxes overlap, skip
                continue; // No overlap, skip
            }

            const KDOPProjection &projection1 = m->kdopProjections[shapeIndex1];
            const KDOPProjection &projection2 = m->kdopProjections[shapeIndex2];

            if (!kdopOverlap(projection1, projection2))
            {
                // KDOP projections do not overlap, skip
                continue; // No overlap, skip
            }

            bool isSameShape = shape1.parentId != -1 && shape1.parentId == shape2.parentId;

            if (!shape1.selfIntersecting && isSameShape)
            {
                continue;
            }

            if (shapeIndex1 == shapeIndex)
            {
                candidates.push(shapeIndex2);
            }
            else if (shapeIndex2 == shapeIndex)
            {
                candidates.push(shapeIndex1);
            }
        }
    }
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
    m->boundingBoxes.clear();
    m->sortedBoundingBoxes.clear();
    m->sortedBoundingBoxShapeIndices.clear();
}

void CollisionSolver::updateBoundingBoxes(PhysicsSpace &space, ConsoleProfileInfo &profileInfo)
{
    if (m->collisionMap.numElements != space.shapes.size())
    {
        m->collisionMap.resize(space.shapes.size());
    }

    if (m->boundingBoxes.capacity() == 0)
    {
        m->boundingBoxes.reserve(space.shapes.size());
        m->sortedBoundingBoxes.reserve(space.shapes.size());
        m->sortedBoundingBoxShapeIndices.reserve(space.shapes.size());
    }

    // For a broad phase collision detection, sort using insertion sort along a single axis

    {
        Timer boundingBoxTimer;
        calculateBoundingBoxes(m->boundingBoxes, m->orientedBoundingBoxes, m->kdopProjections, space.shapes, space.points);
        if (m->sortedBoundingBoxes.size() == 0)
        {
            for (int i = 0; i < m->boundingBoxes.size(); i++)
            {
                m->sortedBoundingBoxes.push(m->boundingBoxes[i]);
                m->sortedBoundingBoxShapeIndices.push(m->boundingBoxes[i].shapeIndex);
            }
        }
        else
        {
            for (int i = m->sortedBoundingBoxes.size(); i < m->boundingBoxes.size(); i++)
            {
                m->sortedBoundingBoxes.push(m->boundingBoxes[i]);
                m->sortedBoundingBoxShapeIndices.push(m->boundingBoxes[i].shapeIndex);
            }

            updateSortedBoundingBoxes(m->sortedBoundingBoxes, m->boundingBoxes, m->sortedBoundingBoxShapeIndices);
        }

        sortBoundingBoxes(m->sortedBoundingBoxes, m->sortedBoundingBoxShapeIndices);
        profileInfo.boundingBoxTimeMillis = boundingBoxTimer.elapsedMillis();
    }

    profileInfo.numBboxes = m->sortedBoundingBoxes.size();
    profileInfo.numBbboxChecks = 0;
    profileInfo.numBboxOverlaps = 0;
    profileInfo.numCircleRejections = 0;
    profileInfo.numIntersections = 0;
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
    resetWheelMotorState(&space);
    updateBoundingBoxes(space, profileInfo);

    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
    {
        const BoundingBox &box = m->sortedBoundingBoxes[i];

        for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
        {
            const BoundingBox &otherBox = m->sortedBoundingBoxes[j];
            profileInfo.numBbboxChecks++;

            if (otherBox.x1 >= box.x2)
            {
                break;
            }
            // Due to the sorted nature of the bounding boxes, we already know that
            // otherBox.x1 >= box.x1, so we only need to check the other axis
            else if (otherBox.y1 >= box.y2 || otherBox.y2 <= box.y1)
            {
                continue;
            }
            Timer calculateCollisionsTimer;

            int shapeIndex1 = m->sortedBoundingBoxShapeIndices[i];
            int shapeIndex2 = m->sortedBoundingBoxShapeIndices[j];
            const Shape &shape1 = space.shapes[shapeIndex1];
            const Shape &shape2 = space.shapes[shapeIndex2];

            const OrientedBoundingBox &orientedBox1 = m->orientedBoundingBoxes[shapeIndex1];
            const OrientedBoundingBox &orientedBox2 = m->orientedBoundingBoxes[shapeIndex2];

            if (!orientedBox1.overlaps(orientedBox2))
            {
                // Oriented bounding boxes overlap, skip
                profileInfo.numCircleRejections++;
                continue; // No overlap, skip
            }

            const KDOPProjection &projection1 = m->kdopProjections[shapeIndex1];
            const KDOPProjection &projection2 = m->kdopProjections[shapeIndex2];

            if (!kdopOverlap(projection1, projection2))
            {
                // KDOP projections do not overlap, skip
                profileInfo.numCircleRejections++;
                continue; // No overlap, skip
            }

            bool isSameShape = shape1.parentId != -1 && shape1.parentId == shape2.parentId;

            if (!shape1.selfIntersecting && isSameShape)
            {
                continue;
            }

            profileInfo.numBboxOverlaps++;
            profileInfo.numIntersections++;
            int numCollisions = m->collisionMap.getCollisionCount(shapeIndex1, shapeIndex2);

            if (numCollisions < 32)
            {
                if (numCollisions % 2 == 0)
                {
                    calculateCollisionsInternal(&space, space.points.range(), shape1, shape2, box, otherBox);
                    calculateCollisionsInternal(&space, space.points.range(), shape2, shape1, otherBox, box);
                }
                else
                {
                    calculateCollisionsMidPointInternal(&space, space.points.range(), shape1, shape2, box, otherBox);
                    calculateCollisionsMidPointInternal(&space, space.points.range(), shape2, shape1, otherBox, box);
                }
            }
            else if (!shape1.isStatic && !shape2.isStatic)
            {
                PointMassesRange range(space.points.range());
                ShapeAxisSeparator::separateShapesFromIntersectionAxis(range, shape1, shape2);
            }
            else if ((shape1.isStatic && !shape2.isStatic) || (shape2.isStatic && !shape1.isStatic))
            {
                const Shape &staticShape(shape1.isStatic ? shape1 : shape2);
                const Shape &movingShape(shape1.isStatic ? shape2 : shape1);
                boxSeparateDynamicAndStaticShapes(space.points.range(), movingShape, staticShape);
            }

            bool stillOverlapping = shapesOverlap(space.points.range(), shape1, shape2);

            if (stillOverlapping)
            {
                m->collisionMap.incrementCollision(shapeIndex1, shapeIndex2);
            }
            else
            {
                m->collisionMap.resetCollision(shapeIndex1, shapeIndex2);
            }

            profileInfo.collisionHandlingTimeMillis += calculateCollisionsTimer.elapsedMillis();
        }
    }

    profileInfo.collisionTimeMillis += collisionsTimer.elapsedMillis();
}

void CollisionSolver::assign(CollisionSolver &other)
{
    m->collisionMap.assign(other.m->collisionMap);
    m->boundingBoxes.replace(other.m->boundingBoxes);
    m->sortedBoundingBoxes.replace(other.m->sortedBoundingBoxes);
}

void CollisionGridSimple::init(Array<BoundingBox> &boundingBoxes)
{

    for (int i = 0; i < width * height; i++)
    {
        cells[i].numIndices = 0;
    }

    for (int i = 0; i < boundingBoxes.size(); i++)
    {
        BoundingBox &box = boundingBoxes[i];
        int x1 = (int)floorf(box.x1 / (float)cellSize);
        int y1 = (int)floorf(box.y1 / (float)cellSize);
        int x2 = (int)ceilf(box.x2 / (float)cellSize);
        int y2 = (int)ceilf(box.y2 / (float)cellSize);

        for (int y = y1; y <= y2; y++)
        {
            for (int x = x1; x <= x2; x++)
            {
                if (x >= 0 && x < width && y >= 0 && y < height)
                {
                    int index = y * width + x;
                    CollisionGridCell &cell = cells[index];
                    if (cell.numIndices < CollisionGridCell::maxNumIndices)
                    {
                        cell.indices[cell.numIndices++] = i;
                    }
                    else
                    {
                        Console::log("Collision grid cell overflow at (%d, %d) for shape %d", x, y, i);
                    }
                }
            }
        }
    }
}

int CollisionSolver::testBoundingBoxesPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations)
{
    updateBoundingBoxes(space, profileInfo);

    int numRejects = 0;
    for (int i = 0; i < numIterations; i++)
    {
        // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
        // TODO: Pre-sort using something better the first time it is run
        for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
        {
            const BoundingBox &box = m->sortedBoundingBoxes[i];

            for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
            {
                const BoundingBox &otherBox = m->sortedBoundingBoxes[j];
                profileInfo.numBbboxChecks++;

                if (otherBox.x1 >= box.x2)
                {
                    numRejects++;
                    break;
                }
                // Due to the sorted nature of the bounding boxes, we already know that
                // otherBox.x1 >= box.x1, so we only need to check the other axis
                else if (otherBox.y1 >= box.y2 || otherBox.y2 <= box.y1)
                {
                    numRejects++;
                    continue;
                }
            }
        }
    }

    return numRejects / numIterations;
}

int CollisionSolver::testAlignedBoundingBoxesPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations)
{
    struct OrientedBoundingBoxPair
    {
        OrientedBoundingBox box1;
        OrientedBoundingBox box2;
    };

    updateBoundingBoxes(space, profileInfo);

    Array<OrientedBoundingBoxPair> pairs;
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
    {
        const BoundingBox &box = m->sortedBoundingBoxes[i];

        for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
        {
            const BoundingBox &otherBox = m->sortedBoundingBoxes[j];
            profileInfo.numBbboxChecks++;

            if (otherBox.x1 >= box.x2)
            {
                break;
            }
            // Due to the sorted nature of the bounding boxes, we already know that
            // otherBox.x1 >= box.x1, so we only need to check the other axis
            else if (otherBox.y1 >= box.y2 || otherBox.y2 <= box.y1)
            {
                continue;
            }

            int shapeIndex1 = m->sortedBoundingBoxShapeIndices[i];
            int shapeIndex2 = m->sortedBoundingBoxShapeIndices[j];
            const Shape &shape1 = space.shapes[shapeIndex1];
            const Shape &shape2 = space.shapes[shapeIndex2];

            const OrientedBoundingBox &box1 = m->orientedBoundingBoxes[shapeIndex1];
            const OrientedBoundingBox &box2 = m->orientedBoundingBoxes[shapeIndex2];
            OrientedBoundingBoxPair pair = {box1, box2};
            pairs.push(pair);
        }
    }

    int numRejects = 0;

    for (int i = 0; i < numIterations; i++)
    {
        for (int j = 0; j < pairs.size(); j++)
        {
            OrientedBoundingBoxPair &pair(pairs[j]);
            if (!pairs[j].box1.overlaps(pairs[j].box2))
            {
                numRejects++;
            }
        }
    }

    return numRejects / numIterations;
}

int CollisionSolver::testKdopPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations)
{
    struct KdopProjectionPair
    {
        KDOPProjection box1;
        KDOPProjection box2;
    };

    updateBoundingBoxes(space, profileInfo);

    Array<KdopProjectionPair> pairs;
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
    {
        const BoundingBox &box = m->sortedBoundingBoxes[i];

        for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
        {
            const BoundingBox &otherBox = m->sortedBoundingBoxes[j];
            profileInfo.numBbboxChecks++;

            if (otherBox.x1 >= box.x2)
            {
                break;
            }
            // Due to the sorted nature of the bounding boxes, we already know that
            // otherBox.x1 >= box.x1, so we only need to check the other axis
            else if (otherBox.y1 >= box.y2 || otherBox.y2 <= box.y1)
            {
                continue;
            }

            int shapeIndex1 = m->sortedBoundingBoxShapeIndices[i];
            int shapeIndex2 = m->sortedBoundingBoxShapeIndices[j];
            const Shape &shape1 = space.shapes[shapeIndex1];
            const Shape &shape2 = space.shapes[shapeIndex2];

            const KDOPProjection &box1 = m->kdopProjections[shapeIndex1];
            const KDOPProjection &box2 = m->kdopProjections[shapeIndex2];
            KdopProjectionPair pair = {box1, box2};
            pairs.push(pair);
        }
    }

    int numRejects = 0;

    for (int i = 0; i < numIterations; i++)
    {
        for (int j = 0; j < pairs.size(); j++)
        {
            KdopProjectionPair &pair(pairs[j]);
            if (!kdopOverlap(pair.box1, pair.box2))
            {
                numRejects++;
            }
        }
    }

    return numRejects / numIterations;
}

inline float sign(const Vector2 &p1, const Vector2 &p2, const Vector2 &p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) -
           (p2.x - p3.x) * (p1.y - p3.y);
}

inline bool pointInTriangleFast(const Vector2 &pt, const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
    float d1 = sign(pt, a, b);
    float d2 = sign(pt, b, c);
    float d3 = sign(pt, c, a);
    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(has_neg && has_pos);
}

inline bool edgeEdgeIntersect(const Vector2 &a1, const Vector2 &a2, const Vector2 &b1, const Vector2 &b2)
{
    float d = (a2.x - a1.x) * (b2.y - b1.y) - (a2.y - a1.y) * (b2.x - b1.x);
    if (d == 0.0f)
        return false;

    float s = ((b1.x - a1.x) * (b2.y - b1.y) - (b1.y - a1.y) * (b2.x - b1.x)) / d;
    float t = ((b1.x - a1.x) * (a2.y - a1.y) - (b1.y - a1.y) * (a2.x - a1.x)) / d;

    return s >= 0.0f && s <= 1.0f && t >= 0.0f && t <= 1.0f;
}

int CollisionSolver::testSimpleOverlapPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations)
{
    struct OverlapPair
    {
        Shape shape1;
        Shape shape2;
    };

    updateBoundingBoxes(space, profileInfo);

    Array<OverlapPair> pairs;
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
    {
        const BoundingBox &box = m->sortedBoundingBoxes[i];

        for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
        {
            const BoundingBox &otherBox = m->sortedBoundingBoxes[j];
            profileInfo.numBbboxChecks++;

            if (otherBox.x1 >= box.x2)
            {
                break;
            }
            // Due to the sorted nature of the bounding boxes, we already know that
            // otherBox.x1 >= box.x1, so we only need to check the other axis
            else if (otherBox.y1 >= box.y2 || otherBox.y2 <= box.y1)
            {
                continue;
            }

            int shapeIndex1 = m->sortedBoundingBoxShapeIndices[i];
            int shapeIndex2 = m->sortedBoundingBoxShapeIndices[j];
            const Shape &shape1 = space.shapes[shapeIndex1];
            const Shape &shape2 = space.shapes[shapeIndex2];

            OverlapPair pair = {shape1, shape2};
            pairs.push(pair);
        }
    }
    int numRejects = 0;

    for (int i = 0; i < numIterations; i++)
    {
        for (int j = 0; j < pairs.size(); j++)
        {
            OverlapPair &pair(pairs[j]);
            if (!shapesOverlapSimple(space.points.range(), pair.shape1, pair.shape2))
            {
                numRejects++;
            }
        }
    }

    return numRejects / numIterations;
}

int CollisionSolver::testOverlapPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations)
{
    struct OverlapPair
    {
        Shape shape1;
        Shape shape2;
    };

    updateBoundingBoxes(space, profileInfo);

    Array<OverlapPair> pairs;
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
    {
        const BoundingBox &box = m->sortedBoundingBoxes[i];

        for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
        {
            const BoundingBox &otherBox = m->sortedBoundingBoxes[j];
            profileInfo.numBbboxChecks++;

            if (otherBox.x1 >= box.x2)
            {
                break;
            }
            // Due to the sorted nature of the bounding boxes, we already know that
            // otherBox.x1 >= box.x1, so we only need to check the other axis
            else if (otherBox.y1 >= box.y2 || otherBox.y2 <= box.y1)
            {
                continue;
            }

            int shapeIndex1 = m->sortedBoundingBoxShapeIndices[i];
            int shapeIndex2 = m->sortedBoundingBoxShapeIndices[j];
            const Shape &shape1 = space.shapes[shapeIndex1];
            const Shape &shape2 = space.shapes[shapeIndex2];

            OverlapPair pair = {shape1, shape2};
            pairs.push(pair);
        }
    }

    int numRejects = 0;

    for (int i = 0; i < numIterations; i++)
    {
        for (int j = 0; j < pairs.size(); j++)
        {
            OverlapPair &pair(pairs[j]);
            if (!shapesOverlap(space.points.range(), pair.shape1, pair.shape2))
            {
                numRejects++;
            }
        }
    }

    return numRejects / numIterations;
}

int CollisionSolver::testCollisionPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations, bool testMidPoint)
{
    struct OverlapPair
    {
        Shape shape1;
        Shape shape2;
    };

    updateBoundingBoxes(space, profileInfo);

    Array<OverlapPair> pairs;
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
    {
        const BoundingBox &box = m->sortedBoundingBoxes[i];

        for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
        {
            const BoundingBox &otherBox = m->sortedBoundingBoxes[j];
            profileInfo.numBbboxChecks++;

            if (otherBox.x1 >= box.x2)
            {
                break;
            }
            // Due to the sorted nature of the bounding boxes, we already know that
            // otherBox.x1 >= box.x1, so we only need to check the other axis
            else if (otherBox.y1 >= box.y2 || otherBox.y2 <= box.y1)
            {
                continue;
            }

            int shapeIndex1 = m->sortedBoundingBoxShapeIndices[i];
            int shapeIndex2 = m->sortedBoundingBoxShapeIndices[j];
            const Shape &shape1 = space.shapes[shapeIndex1];
            const Shape &shape2 = space.shapes[shapeIndex2];

            const OrientedBoundingBox &orientedBox1 = m->orientedBoundingBoxes[shapeIndex1];
            const OrientedBoundingBox &orientedBox2 = m->orientedBoundingBoxes[shapeIndex2];

            if (!orientedBox1.overlaps(orientedBox2))
            {
                continue;
            }

            OverlapPair pair = {shape1, shape2};
            pairs.push(pair);
        }
    }

    PhysicsSpace copy;
    for (int i = 0; i < numIterations; i++)
    {
        copy.assign(space);

        if (testMidPoint)
        {
            for (int j = 0; j < pairs.size(); j++)
            {
                OverlapPair &pair(pairs[j]);

                calculateCollisionsMidPoint(copy.points.range(), pair.shape1, pair.shape2, m->boundingBoxes[pair.shape1.index], m->boundingBoxes[pair.shape2.index]);
                calculateCollisionsMidPoint(copy.points.range(), pair.shape2, pair.shape1, m->boundingBoxes[pair.shape2.index], m->boundingBoxes[pair.shape1.index]);
            }
        }
        else
            for (int j = 0; j < pairs.size(); j++)
            {
                OverlapPair &pair(pairs[j]);

                calculateCollisions(copy.points.range(), pair.shape1, pair.shape2, m->boundingBoxes[pair.shape1.index], m->boundingBoxes[pair.shape2.index]);
                calculateCollisions(copy.points.range(), pair.shape2, pair.shape1, m->boundingBoxes[pair.shape2.index], m->boundingBoxes[pair.shape1.index]);
            }
    }

    return 0;
}
