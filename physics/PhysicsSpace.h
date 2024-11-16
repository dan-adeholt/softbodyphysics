#ifndef __GAME_PHYSICSSPACE_H__
#define __GAME_PHYSICSSPACE_H__

#include "./Physics.h"

struct PhysicsSpace
{
    PhysicsSpace();

    void assign(PhysicsSpace &other);

    int nextShapeIndex() const;
    int nextStaticShapeIndex() const;

    void clear();

    Array<Shape> shapes;
    Array<ShapeQuad> partialShapes;
    PointMasses points;
    Array<Shape> staticShapes;
    PointMasses staticPoints;
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