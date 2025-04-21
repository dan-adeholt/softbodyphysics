#ifndef __GAME_PHYSICSSPACE_H__
#define __GAME_PHYSICSSPACE_H__

#include "./Physics.h"

template <typename T>
class Array;
struct ShapeEntry
{
    bool isStatic;
    PointMasses points;
};

struct PhysicsSpace
{
    PhysicsSpace();

    void assign(PhysicsSpace &other);

    void initFromEntries(const Array<ShapeEntry> &entries);

    int nextShapeIndex() const;

    int nextParentId() const;

    void updateIndices();

    void clear();

    void removeShape(int shapeIndex);

    void pasteShape(int copyIndex);

    int closestPointIndex(float x, float y, int shapeIndex) const;

    Array<Shape> shapes;
    PointMasses points;
    Array<StaticJoint> staticJoints;
    Array<ShapeJoint> shapeJoints;

    StaticJoint mouseJoint;
    bool gravityEnabled;
    bool collisionsEnabled;
    bool shapeMatchingEnabled;
    bool springsEnabled;

private:
    void removeShapeWithoutPoints(int shapeIndex);
};

#endif // __GAME_PHYSICSSPACE_H__