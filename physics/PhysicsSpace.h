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

    void clear();

    void removeShape(int shapeIndex);

    Array<Shape> shapes;
    Array<ShapeQuad> partialShapes;
    Array<float> shapeSpringDiffs;
    PointMasses points;
    Array<Spring> springs;
    Array<StaticJoint> staticJoints;

    StaticJoint mouseJoint;
    bool gravityEnabled;
    bool collisionsEnabled;
    bool shapeMatchingEnabled;
    bool springsEnabled;
    int draggingShapeIndex;
    int draggingSubShapeIndex;
};

#endif // __GAME_PHYSICSSPACE_H__