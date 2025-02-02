#ifndef __GAME_ShapeAxisSeparator_H__
#define __GAME_ShapeAxisSeparator_H__

struct PointMassesRange;
struct Shape;

namespace ShapeAxisSeparator
{
    void separateShapesFromIntersectionAxis(PointMassesRange &points, const Shape &shape1, const Shape &shape2);
}

#endif