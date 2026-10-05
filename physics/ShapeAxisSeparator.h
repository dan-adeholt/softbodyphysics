#ifndef __GAME_ShapeAxisSeparator_H__
#define __GAME_ShapeAxisSeparator_H__

struct PointMassesRange;
struct Shape;
class Vector2;

namespace ShapeAxisSeparator
{
    // Moves the bodies of two overlapping moving shapes apart, the shortest way or, given keepDirection, along
    // it if that clears them: the way shape 2's body moves from shape 1's. usedDirection, if given, gets the
    // way shape 2's body moved.
    bool separateShapesFromIntersectionAxis(PointMassesRange &points, const Shape &shape1, const Shape &shape2,
                                            const Vector2 *keepDirection = nullptr, Vector2 *usedDirection = nullptr);
}

#endif