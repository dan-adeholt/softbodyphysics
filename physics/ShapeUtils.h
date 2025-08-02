#ifndef __SHAPE_UTILS_H
#define __SHAPE_UTILS_H

#include "../math/Vector2.h"
#include "Physics.h"

struct Shape;
struct PointMassesRange;
struct PositionPair
{
    Vector2 p0;
    Vector2 p1;
};
namespace ShapeUtils
{
    ShapeVelocities getAverageShapeVelocity(PointMassesRange points, const Shape &shape);
    ShapeProperties getShapeProperties(PointMassesRange points, const Shape &shape);
    Vector2 getShapePos(const PointMassesRange &points, const Shape &shape, int j, const ShapeProperties &averages, const ShapeMatchDragData &dragData);
    PositionPair getShapeJointPositions(const PointMassesRange &points, const ShapeJoint &joint);
}

#endif