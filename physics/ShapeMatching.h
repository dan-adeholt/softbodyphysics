#ifndef __GAME_PHYSICS_SHAPE_MATCHING_H__
#define __GAME_PHYSICS_SHAPE_MATCHING_H__

#include "Physics.h"

struct ShapeMatching
{
    static void shapeMatchAlignInit(PointMassesRange points, Shape &shape);

    static void shapeMatchAlign(PointMassesRange points, Array<Shape> &shapes, int draggingShapeIndex);
};

#endif // __GAME_PHYSICS_SHAPE_MATCHING_H__