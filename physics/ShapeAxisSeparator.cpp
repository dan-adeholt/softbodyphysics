#include "ShapeAxisSeparator.h"
#include "ShapeUtils.h"
#include "../math/Vector2.h"
#include "../physics/Physics.h"
#include "../utils/MinMax.h"
#include <math.h>

// The last resort for two moving shapes that the collision passes have failed to pull apart for a while.
// Single shapes, like balls and boxes, get their points pushed across a line between them, see
// pushPointsAcrossDividingLine. Parts of bigger bodies, like the segments of a bridge, are moved apart in one
// go instead, along the direction that takes the shortest move to clear them, as found by
// the separating axis test over the shapes' outer edges. The bodies they belong to are shifted as a whole,
// all their points together, so their shapes are left alone and shape matching has nothing to undo, and they
// share the move by inverse mass. Any speed they still have towards each other along that direction is taken
// away too, so they don't run straight back in.
//
// Whole bodies move, not just the two shapes, because a shape can be one segment of a strip such as a
// bridge, sharing its points with the next. Moving one segment on its own wrenched its neighbours into the
// other strip, and strips lying across each other stayed tangled.
//
// Edges a shape shares with a neighbouring shape of the same body, such as the joints between bridge
// segments, aren't used as directions: moving apart along them would push a segment into its neighbour.

namespace
{
    struct Projection
    {
        float min;
        float max;
    };

    Projection project(const PointMassesRange &points, const ShapeIndexedRange &range, Vector2 axis)
    {
        Projection projection = {__FLT_MAX__, -__FLT_MAX__};

        for (int i = 0; i < range.size(); i++)
        {
            const float distance = points.pos[range[i]].dot(axis);
            projection.min = min(projection.min, distance);
            projection.max = max(projection.max, distance);
        }

        return projection;
    }

    struct Separation
    {
        bool found;
        Vector2 direction; // The way shape 2 moves away from shape 1
        float distance;
    };

    // Tries the normals of the shape's outer edges as directions to move the shapes apart along, keeping the
    // one with the shortest move. Returns false if the shapes are already apart along one of them.
    bool findShortestSeparation(const PointMassesRange &points, const ShapeIndexedRange &edgeRange, const ShapeIndexedRange &range1,
                                const ShapeIndexedRange &range2, Separation &best)
    {
        for (int i = 0; i < edgeRange.size(); i++)
        {
            if (edgeRange.hasInteriorEdge(i))
            {
                continue;
            }

            const Vector2 edge = points.pos[edgeRange[(i + 1) % edgeRange.size()]] - points.pos[edgeRange[i]];

            if (edge.lengthSquared() < 1e-8f)
            {
                continue;
            }

            const Vector2 axis = edge.normalVector().normalized();
            const Projection projection1 = project(points, range1, axis);
            const Projection projection2 = project(points, range2, axis);

            // How far shape 2 would have to move along the axis, one way or the other, to clear shape 1
            const float forwards = projection1.max - projection2.min;
            const float backwards = projection2.max - projection1.min;

            if (forwards <= 0.0f || backwards <= 0.0f)
            {
                return false;
            }

            const float distance = min(forwards, backwards);

            if (!best.found || distance < best.distance)
            {
                best.found = true;
                best.direction = forwards < backwards ? axis : -axis;
                best.distance = distance;
            }
        }

        return true;
    }

    float totalMass(const PointMassesRange &points, const ShapeIndexedRange &range)
    {
        float mass = 0.0f;

        for (int i = 0; i < range.size(); i++)
        {
            mass += points.mass[range[i]];
        }

        return mass;
    }

    Vector2 averageVelocity(const PointMassesRange &points, const ShapeIndexedRange &range)
    {
        Vector2 velocity;

        for (int i = 0; i < range.size(); i++)
        {
            velocity += points.velocity[range[i]];
        }

        return velocity / static_cast<float>(range.size());
    }

    Vector2 centroid(const PointMassesRange &points, const ShapeIndexedRange &range)
    {
        Vector2 sum;

        for (int i = 0; i < range.size(); i++)
        {
            sum += points.pos[range[i]];
        }

        return sum / static_cast<float>(range.size());
    }

    // How single shapes, like balls and boxes, are pulled apart. A line is drawn through where their outlines
    // cross, square to the line between their centers. The shape with the point farthest from that line stays
    // on that point's side, the other goes to the other side, and their points on the wrong side are moved onto
    // the line. Moving points rather than whole shapes also unfolds a shape crushed into a tangle in a pile;
    // moved as a whole, it only got shoved, still folded, into its neighbours, and stayed folded for longer.
    bool pushPointsAcrossDividingLine(PointMassesRange &points, const Shape &shape1, const Shape &shape2)
    {
        const ShapeIndexedRange range1(shape1);
        const ShapeIndexedRange range2(shape2);

        // Where the outlines cross, averaged
        Vector2 crossing;
        int numCrossings = 0;

        for (int i = 0; i < range1.size(); i++)
        {
            const int i0 = range1[i];
            const int i1 = range1[(i + 1) % range1.size()];

            for (int j = 0; j < range2.size(); j++)
            {
                const int j0 = range2[j];
                const int j1 = range2[(j + 1) % range2.size()];
                float t1, t2;

                if (i0 != j0 && i0 != j1 && i1 != j0 && i1 != j1 &&
                    lineSegmentIntersection(points.pos[i0], points.pos[i1], points.pos[j0], points.pos[j1], t1, t2))
                {
                    crossing += points.pos[i0] + (points.pos[i1] - points.pos[i0]) * t1;
                    numCrossings++;
                }
            }
        }

        const Vector2 betweenCenters = centroid(points, range2) - centroid(points, range1);

        if (numCrossings == 0 || betweenCenters.lengthSquared() < 1e-8f)
        {
            return false;
        }

        crossing = crossing / static_cast<float>(numCrossings);
        const Vector2 lineDirection = betweenCenters.normalized().normalVector();
        const Vector2 lineNormal = lineDirection.normalVector();

        // The point farthest from the line decides which side its shape keeps
        float farthest = -1.0f;
        bool farthestOnPositiveSide = false;
        bool farthestInShape1 = true;

        for (int k = 0; k < 2; k++)
        {
            const ShapeIndexedRange &range = k == 0 ? range1 : range2;

            for (int i = 0; i < range.size(); i++)
            {
                const float side = (points.pos[range[i]] - crossing).dot(lineNormal);

                if (fabsf(side) > farthest)
                {
                    farthest = fabsf(side);
                    farthestOnPositiveSide = side > 0.0f;
                    farthestInShape1 = k == 0;
                }
            }
        }

        for (int k = 0; k < 2; k++)
        {
            const ShapeIndexedRange &range = k == 0 ? range1 : range2;
            const bool positiveSide = (k == 0) == farthestInShape1 ? farthestOnPositiveSide : !farthestOnPositiveSide;

            for (int i = 0; i < range.size(); i++)
            {
                const int index = range[i];
                const float side = (points.pos[index] - crossing).dot(lineNormal);

                if ((side > 0.0f) != positiveSide)
                {
                    // Onto the line, and a hair past it
                    points.pos[index] += lineNormal * (positiveSide ? 0.01f - side : -0.01f - side);
                    points.velocity[index] *= 0.8f;
                }
            }
        }

        return true;
    }

    void shift(PointMassesRange &points, const ShapeIndexedRange &range, Vector2 offset, Vector2 velocityChange)
    {
        for (int i = 0; i < range.size(); i++)
        {
            points.pos[range[i]] += offset;
            points.velocity[range[i]] += velocityChange;
        }
    }
}

bool ShapeAxisSeparator::separateShapesFromIntersectionAxis(PointMassesRange &points, const Shape &shape1, const Shape &shape2,
                                                            const Vector2 *keepDirection, Vector2 *usedDirection)
{
    int commonEdge = shape1.commonEdge(shape2);
    if (commonEdge != -1)
    {
        // TODO: Handle common edge?
        return false;
    }

    // Parts of bodies, like the segments of a bridge, are moved with their whole bodies below
    if (!shape1.hasIndices() && !shape2.hasIndices())
    {
        if (usedDirection != nullptr)
        {
            *usedDirection = Vector2();
        }

        return pushPointsAcrossDividingLine(points, shape1, shape2);
    }

    const ShapeIndexedRange range1(shape1);
    const ShapeIndexedRange range2(shape2);

    if (range1.size() < 3 || range2.size() < 3)
    {
        return false;
    }

    Separation separation = {false, Vector2(), 0.0f};

    if (!findShortestSeparation(points, range1, range1, range2, separation) ||
        !findShortestSeparation(points, range2, range1, range2, separation) ||
        !separation.found)
    {
        return false;
    }

    // The same way as last time, if moving that way clears them without moving more than three times as far as the
    // shortest way would. Much further, and a shape in a pile gets shoved a long way into its neighbours.
    if (keepDirection != nullptr)
    {
        const Projection projection1 = project(points, range1, *keepDirection);
        const Projection projection2 = project(points, range2, *keepDirection);
        const float distance = projection1.max - projection2.min;

        if (distance > 0.0f && distance <= separation.distance * 3.0f)
        {
            separation.direction = *keepDirection;
            separation.distance = distance;
        }
    }

    if (usedDirection != nullptr)
    {
        *usedDirection = separation.direction;
    }

    // A little past touching, like the collision passes' push out
    const float clearance = 0.1f;
    // All the points of the bodies the shapes belong to. Two parts of the same body, such as a strip
    // folded onto itself, move just the two parts; moving the body both ways would cancel out.
    const bool sameBody = shape1.start == shape2.start;
    const Shape bodyShape1(shape1.start, shape1.end);
    const Shape bodyShape2(shape2.start, shape2.end);
    const ShapeIndexedRange body1(sameBody ? shape1 : bodyShape1);
    const ShapeIndexedRange body2(sameBody ? shape2 : bodyShape2);

    const float inverseMass1 = 1.0f / totalMass(points, body1);
    const float inverseMass2 = 1.0f / totalMass(points, body2);
    const float share1 = inverseMass1 / (inverseMass1 + inverseMass2);
    const float share2 = inverseMass2 / (inverseMass1 + inverseMass2);
    const Vector2 move = separation.direction * (separation.distance + clearance);

    // Only take away speed towards each other, not apart
    const float closingSpeed = (averageVelocity(points, body2) - averageVelocity(points, body1)).dot(separation.direction);
    const Vector2 velocityChange = closingSpeed < 0.0f ? separation.direction * -closingSpeed : Vector2();

    shift(points, body1, -move * share1, -velocityChange * share1);
    shift(points, body2, move * share2, velocityChange * share2);
    return true;
}
