#ifndef __GAME_PHYSICSSPACE_H__
#define __GAME_PHYSICSSPACE_H__

#include "./Physics.h"

template <typename T>
class Array;
struct ShapeEntry
{
    bool isStatic;
    PointMasses points;
    Array<Spring> springs;
};

struct PhysicsSpace
{
    PhysicsSpace();

    void assign(PhysicsSpace &other);

    void initFromEntries(const Array<ShapeEntry> &entries);

    int nextShapeIndex() const;

    void updateIndices();

    void clear();

    void removeShape(int shapeIndex);

    Array<Shape> shapes;
    PointMasses points;
    Array<Spring> springs;
    Array<PointJoint> pointJoints;
    Array<StaticJoint> staticJoints;

    StaticJoint mouseJoint;
    bool gravityEnabled;
    bool collisionsEnabled;
    bool shapeMatchingEnabled;
    bool springsEnabled;
};

#endif // __GAME_PHYSICSSPACE_H__