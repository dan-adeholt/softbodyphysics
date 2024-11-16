#ifndef __GAME_PHYSICS_SPRINGS_H
#define __GAME_PHYSICS_SPRINGS_H

#include "Physics.h"

struct ConsoleProfileInfo;

struct PhysicsSprings
{
    static void performThreadedSpringDerivatives(Range<Shape> shapeRange, PointMassesRange points, Range<Spring> springs, Range<PointDerivative> derivatives, Range<ShapeQuad> partialShapes, bool enableShapeMatching, ConsoleProfileInfo &profileInfo);
};

#endif // __GAME_PHYSICS_SPRINGS_H