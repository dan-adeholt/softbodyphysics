#ifndef __GAME_CollisionSolver_H__
#define __GAME_CollisionSolver_H__

#include "../containers/Array.h"
#include "./Physics.h"

struct PhysicsSpace;

struct IntersectionResult
{
    Vector2 point;
    float t;
    int segmentIndex;
    bool found;
};

struct ShapeBoundingBox
{
    int shapeIndex;
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

    bool encloses(const ShapeBoundingBox &other) const
    {
        return x1 <= other.x1 && x2 >= other.x2 && y1 <= other.y1 && y2 >= other.y2;
    }

    bool overlaps(const ShapeBoundingBox &other) const
    {
        return x1 < other.x2 && x2 > other.x1 && y1 < other.y2 && y2 > other.y1;
    }
};

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
    Vector2 pointOutside0;
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
    void handleCollisions(PhysicsSpace &space, PhysicsSpace &prevSpace, ConsoleProfileInfo &profileInfo);

    void assign(CollisionSolver &other);

    static ClosestSegmentResult findEntryEdgeClosestSegment(
        PointMassesRange collisionShape,
        const Vector2 &point, bool log = false);

    static ShapeBoundingBox calculateShapeBoundingBox(int shapeIndex, const PointMassesRange &range);
    static void calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes, const Array<Shape> &shapes, const PointMasses &points);

    static bool isPointOutsideShape(float pointX, float pointY, const ShapeBoundingBox &box, const PointMassesRange &shape);

    static int calculateCollisions(
        PointMassesRange collisionShape,
        PointMassesRange movingShape,
        PointMassesRange prevCollisionShape,
        PointMassesRange prevMovingShape,
        const ShapeBoundingBox &collisionBox,
        const ShapeBoundingBox &box2,
        bool isStatiCollisionShape,
        bool isStaticMovingShape);

    static int calculateCollisionsMidPoint(
        PointMassesRange collisionShape,
        PointMassesRange movingShape,
        PointMassesRange prevCollisionShape,
        PointMassesRange prevMovingShape,
        const ShapeBoundingBox &collisionBox,
        const ShapeBoundingBox &box2,
        bool isStatiCollisionShape,
        bool isStaticMovingShape);

    Array<ShapeBoundingBox> &boundingBoxes();

private:
    void boxSeparateDynamicAndStaticShapes(
        PointMassesRange &movingRange,
        PointMassesRange &staticRange,
        PhysicsSpace &space);

    struct Impl;
    Impl *m;
};

#endif // __GAME_CollisionSolver_H__