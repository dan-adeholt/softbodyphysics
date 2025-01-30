#ifndef __GAME_ShapeAxisSeparator_H__
#define __GAME_ShapeAxisSeparator_H__

struct PointMassesRange;

namespace ShapeAxisSeparator
{
    void separateShapesFromIntersectionAxis(PointMassesRange &range1, PointMassesRange &range2);
}

#endif