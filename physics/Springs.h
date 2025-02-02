#ifndef __GAME_PHYSICS_SPRINGS_H
#define __GAME_PHYSICS_SPRINGS_H

#include "Physics.h"

struct ConsoleProfileInfo;
struct SpringsTempDataImpl;
struct ShapeMatchDragData;

struct Springs

{
    static void performThreadedSpringDerivatives(
        Range<Shape> shapeRange,
        Range<ShapeProperties> shapeProperties,
        PointMassesRange points,
        Range<Spring> springs,
        Range<PointDerivative> derivatives,
        bool enableShapeMatching,
        const ShapeMatchDragData &dragData,
        ConsoleProfileInfo &profileInfo);
};

#endif // __GAME_PHYSICS_SPRINGS_H