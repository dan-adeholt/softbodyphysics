#ifndef __SHAPE_UTILS_H
#define __SHAPE_UTILS_H

#include "../math/Vector2.h"
#include "Physics.h"

struct Shape;
struct PointMassesRange;

namespace ShapeUtils
{
    Vector2 getAverageShapeVelocity(PointMassesRange points, const Shape &shape);
    ShapeProperties getShapeProperties(PointMassesRange points, const Shape &shape);
    Vector2 getShapePos(const PointMassesRange &points, const Shape &shape, int j, const ShapeProperties &averages, const ShapeMatchDragData &dragData);
}

#endif