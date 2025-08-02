#ifndef __GAME_CollisionSolver_H__
#define __GAME_CollisionSolver_H__

#include "../containers/Array.h"
#include "./Physics.h"
#include <math.h>

struct PhysicsSpace;

struct IntersectionResult
{
    Vector2 point;
    float t;
    int segmentIndex;
    bool found;
};

struct BoundingBox
{

    float x1, y1;
    float x2, y2;

    float width() const
    {
        return x2 - x1;
    }

    float height() const
    {
        return y2 - y1;
    }

    Vector2 center() const
    {
        return Vector2(x1 + (x2 - x1) * 0.5f, y1 + (y2 - y1) * 0.5f);
    }

    bool includes(const Vector2 &point) const
    {
        return point.x >= x1 && point.x <= x2 && point.y >= y1 && point.y <= y2;
    }

    bool encloses(const BoundingBox &other) const
    {
        return x1 <= other.x1 && x2 >= other.x2 && y1 <= other.y1 && y2 >= other.y2;
    }

    bool overlaps(const BoundingBox &other) const
    {
        return x1 < other.x2 && x2 > other.x1 && y1 < other.y2 && y2 > other.y1;
    }
};

struct ShapeBoundingBox : BoundingBox
{
    int shapeIndex;
};

struct OrientedBoundingBox
{
    Vector2 center;
    Vector2 axisX; // Unit vector
    Vector2 axisY; // Unit vector, perpendicular to axisX
    float halfX;
    float halfY;

    /// Projects an OBB onto an axis and returns the min/max range
    inline void projectOBB(const Vector2 &axis, float &outMin, float &outMax) const
    {
        float c = center.dot(axis);
        float r = halfX * fabs(axisX.dot(axis)) +
                  halfY * fabs(axisY.dot(axis));
        outMin = c - r;
        outMax = c + r;
    }

    /// Returns true if two OBBs overlap
    inline bool overlaps(const OrientedBoundingBox &rhs) const
    {
        const Vector2 axes[4] = {
            axisX,
            axisY,
            rhs.axisX,
            rhs.axisY,
        };

        for (int i = 0; i < 4; ++i)
        {
            float minA, maxA, minB, maxB;
            projectOBB(axes[i], minA, maxA);
            rhs.projectOBB(axes[i], minB, maxB);
            if (maxA < minB || maxB < minA)
            {
                return false; // Found a separating axis
            }
        }
        return true;
    }
};

struct KDOPProjection
{
    float min[4];
    float max[4];
};

IntersectionResult
lineIntersection(const Vector2 &s1, const Vector2 &s2, const Vector2 &p1, const Vector2 &p2);

struct ConsoleProfileInfo;

// enum class EdgeStrategy
// {
//     ClosestSegment,
//     RelativeVelocitySegment,
//     NumStrategies,
//     IntersectionPrevAndCurrent
// };

// const char *edgeStrategyToString(EdgeStrategy strategy);

struct ClosestSegmentResult
{
    int entryEdgeIndex0;
    Vector2 closestPoint0;
    float entryTime0;
};

struct CollisionSolver
{
    CollisionSolver();
    ~CollisionSolver();

    CollisionSolver(const CollisionSolver &) = delete;
    // Delete the copy assignment operator
    CollisionSolver &operator=(const CollisionSolver &) = delete;

    void clear();
    void updateBoundingBoxes(PhysicsSpace &space, ConsoleProfileInfo &profileInfo);
    void handleCollisions(PhysicsSpace &space, ConsoleProfileInfo &profileInfo);

    void assign(CollisionSolver &other);

    static ClosestSegmentResult findEntryEdgeClosestSegment(PointMassesRange points,
                                                            const Shape &collisionShape,
                                                            const Vector2 &currentPoint);

    static ShapeBoundingBox calculateShapeBoundingBox(const PointMassesRange &points, int shapeIndex, const Shape &shape);

    static OrientedBoundingBox calculateOrientedBoundingBox(const PointMassesRange &points, int shapeIndex, const Shape &shape);

    static void calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes,
                                       Array<OrientedBoundingBox> &orientedBoundingBoxes,
                                       Array<KDOPProjection> &projections,
                                       const Array<Shape> &shapes, const PointMasses &points);

    static bool isPointOutsideShape(int pointIndex, float pointX, float pointY, const BoundingBox &box, const PointMassesRange &points, const Shape &collisionShape);

    static int calculateCollisions(
        PointMassesRange points,
        const Shape &collisionShape,
        const Shape &movingShape,
        const BoundingBox &collisionBox,
        const BoundingBox &movingBox);

    static int calculateCollisionsMidPoint(
        PointMassesRange points,
        const Shape &collisionShape,
        const Shape &movingShape,
        const BoundingBox &collisionBox,
        const BoundingBox &movingBox);

    Array<ShapeBoundingBox> &boundingBoxes();

    Array<OrientedBoundingBox> &orientedBoundingBoxes();

    CollisionGridSimple &grid();

    void toggleCollisionGrid();

    void getCollisionCandidates(int shapeIndex, Array<int> &candidates, PhysicsSpace &space);

    int testBoundingBoxesPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations);
    int testAlignedBoundingBoxesPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations);
    int testSimpleOverlapPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations);
    int testOverlapPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations);
    int testCollisionPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations, bool testMidPoint);
    int testKdopPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations);

private:
    void boxSeparateDynamicAndStaticShapes(PointMassesRange points, const Shape &movingShape, const Shape &staticShape);

    struct Impl;
    Impl *m;
};

#endif // __GAME_CollisionSolver_H__