#ifndef __GAME_PHYSICSCOLLISIONSOLVER_H__
#define __GAME_PHYSICSCOLLISIONSOLVER_H__

#include "../containers/Array.h"
#include "./Physics.h"

struct PhysicsSpace;

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

    bool overlaps(const ShapeBoundingBox &other) const
    {
        return x1 < other.x2 && x2 > other.x1 && y1 < other.y2 && y2 > other.y1;
    }
};

int calculateCollisions(
    PointMassesRange shape1,
    PointMassesRange movingShape,
    PointMassesRange movingShapePrevPos,
    const ShapeBoundingBox &box1,
    const ShapeBoundingBox &box2,
    int shapeIndex1,
    int shapeIndex2,
    float step);

int calculateStaticCollisions(
    PointMassesRange staticShape,
    PointMassesRange movingShape,
    PointMassesRange movingShapePrevPos,
    const ShapeBoundingBox &staticBox,
    const ShapeBoundingBox &movingBox,
    float step);

struct CollisionPair
{
    int shape1Index;
    int shape2Index;
};

struct CollisionMap
{
    Array<unsigned char> data;
    int numElements;

    int calculateIndex(int i, int j) const
    {
        if (i > j)
        {
            int temp = i;
            i = j;
            j = temp;
        }

        return (j * (j - 1) / 2) + i;
    }

    static int arraySize(int numElements)
    {
        return (numElements * (numElements - 1)) / 2;
    }

    CollisionMap(int numElements) : data(CollisionMap::arraySize(numElements)), numElements(numElements) {}

    void resize(int numElements)
    {
        data.fill(0, CollisionMap::arraySize(numElements));
        this->numElements = numElements;
    }

    void assign(const CollisionMap &other)
    {
        data.fill(0, CollisionMap::arraySize(other.numElements));

        for (int i = 0; i < other.numElements; i++)
        {
            data[i] = other.data[i];
        }

        numElements = other.numElements;
    }

    void clear()
    {
        data.fill(0, CollisionMap::arraySize(this->numElements));
    }

    void resetCollision(int i, int j)
    {
        int index = calculateIndex(i, j);
        data[index] = 0;
    }

    void incrementCollision(int i, int j)
    {
        int index = calculateIndex(i, j);
        if (data[index] < 255)
        {
            ++data[index];
        }
    }

    unsigned char getCollisionCount(int i, int j) const
    {
        return data[calculateIndex(i, j)];
    }
};

struct ConsoleProfileInfo;

struct PhysicsCollisionSolver
{
    PhysicsCollisionSolver();
    void clear();
    void updateBoundingBoxes(PhysicsSpace &space, ConsoleProfileInfo &profileInfo);
    void handleCollisions(PhysicsSpace &space, PhysicsSpace &prevSpace, float step, ConsoleProfileInfo &profileInfo);

    void assign(PhysicsCollisionSolver &other);

    CollisionMap collisionMap;
    Array<CollisionPair> resolvedCollisionPairs;
    Array<ShapeBoundingBox> boundingBoxes;
    Array<ShapeBoundingBox> staticBoundingBoxes;
    Array<ShapeBoundingBox> sortedBoundingBoxes;
    Array<ShapeBoundingBox> sortedStaticBoundingBoxes;
    Array<int> ejectShapeIndices;
};

void findClosestLineSegmentToPoint(PointMassesRange collisionShape, const Vector2 &point, const Vector2 &velocity, int &minIndex, Vector2 &minPoint, float &minT);

void calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes, const Array<Shape> &shapes, const PointMasses &points);

ShapeBoundingBox calculateShapeBoundingBox(const Shape &shape, int shapeIndex, const PointMasses &points);

bool shapesOverlap(const PointMassesRange &poly1, const PointMassesRange &poly2);

bool pointInShape(const Vector2 &point, const PointMassesRange &shape);

#endif // __GAME_PHYSICSCOLLISIONSOLVER_H__