#ifndef __PHYSICS__TEST_H
#define __PHYSICS__TEST_H

#include "Physics.h"
#include "./PhysicsSpace.h"
#include "../containers/Array.h"
#include "../game/Shapes.h"
#include "../utils/Console.h"
#include "../utils/UnitTestUtil.h"
#include "CollisionSolver.h"
#include "CollisionSolverInternal.h"

void testCase(Shape &shape1, Shape &shape2, PointMasses &points, int expectedCollisions)
{
    static Array<Shape> shapes;
    static Array<ShapeBoundingBox> boundingBoxes;
    static Array<OrientedBoundingBox> orientedBoundingBoxes;
    static Array<KDOPProjection> kdopProjections;
    shapes.clear();
    boundingBoxes.clear();
    kdopProjections.clear();
    orientedBoundingBoxes.clear();

    shapes.append({shape1, shape2});

    CollisionSolver::calculateBoundingBoxes(boundingBoxes, orientedBoundingBoxes, kdopProjections, shapes, points);

    PointMassesRange range = points.range();

    testExpectInt(CollisionSolver::calculateCollisions(range, shape1, shape2, boundingBoxes[0], boundingBoxes[1]), expectedCollisions);
}

UNIT_TEST(PhysicsTestfindEntryEdge)
{
    PhysicsSpace space;
    Shape shelf = Shapes::createTriangle(space, false, 50.0f, 700.0f, 800.0f, 500.0f, 800.0f, 700.0f, 0.0f);
    Vector2 pos1(500.0, 588.0);
    ClosestSegmentResult result = CollisionSolver::findEntryEdgeClosestSegment(space.points.range(), shelf, pos1);
    testExpectInt(result.entryEdgeIndex0, 0);
}

void setVelocity(PointMasses &points, Shape shape, float vx, float vy)
{
    ShapeIndexedRange range = ShapeIndexedRange(shape);

    for (int i = 0; i < range.size(); i++)
    {
        points.velocity[range[i]].x = vx;
        points.velocity[range[i]].y = vy;
    }
}

UNIT_TEST(PhysicsTestCollisions)
{
    PhysicsSpace space;

    // One half of a quad is inside another quad
    Shape shape1 = Shapes::createQuad(space, 31.9f, 0.1f, 32.0f, 32.0f, 1.0f);
    Shape shape2 = Shapes::createQuad(space, 0.0f, 0.0f, 32.0f, 32.0f, 1.0f);
    setVelocity(space.points, shape1, 1.0f, 0.0f);
    setVelocity(space.points, shape2, -2.0f, 0.0f);
    testCase(shape1, shape2, space.points, 1);

    // One corner of a quad is inside another quad
    shape1 = Shapes::createQuad(space, 0.0f, 0.0f, 32.0f, 32.0f, 1.0f);
    shape2 = Shapes::createQuad(space, 16.0f, 16.0f, 32.0f, 32.0f, 1.0f);
    setVelocity(space.points, shape1, 1.0f, 0.0f);
    setVelocity(space.points, shape2, -2.0f, 0.0f);
    testCase(shape1, shape2, space.points, 1);

    // One quad is completely inside another quad
    shape1 = Shapes::createQuad(space, 0.0f, 0.0f, 32.0f, 32.0f, 1.0f);
    shape2 = Shapes::createQuad(space, 16.0f, 16.0f, 4.0f, 4.0f, 1.0f);
    setVelocity(space.points, shape1, 1.0f, 0.0f);
    setVelocity(space.points, shape2, -2.0f, 0.0f);

    testCase(shape1, shape2, space.points, 4);

    // One quad is completely inside a parallelogram
    shape1 = Shapes::createParallelogram(space, 0.0f, 0.0f, 32.0f, 32.0f, 10.0f, 1.0f);
    shape2 = Shapes::createQuad(space, 16.0f, 16.0f, 4.0f, 4.0f, 1.0f);
    setVelocity(space.points, shape1, 1.0f, 0.0f);
    setVelocity(space.points, shape2, -2.0f, 0.0f);

    testCase(shape1, shape2, space.points, 4);

    // One corner of a triangle is inside another triangle
    shape1 = Shapes::createTriangle(space, false, 10.0f, 0.0f, 16.0f, 5.0f, 5.0f, 10.0f, 1.0f);
    shape2 = Shapes::createTriangle(space, false, 10.0f, 3.0f, 11.0f, 13.0f, 7.0f, 16.0f, 1.0f);
    setVelocity(space.points, shape1, 1.0f, 0.0f);
    setVelocity(space.points, shape2, -2.0f, 0.0f);

    testCase(shape1, shape2, space.points, 1);

    // Two triangles close to eachother, but not intersecting
    shape1 = Shapes::createTriangle(space, false, 10.0f, 0.0f, 16.0f, 5.0f, 5.0f, 10.0f, 1.0f);
    shape2 = Shapes::createTriangle(space, false, 16.0f, 6.0f, 11.0f, 13.0f, 7.0f, 16.0f, 1.0f);
    setVelocity(space.points, shape1, 1.0f, 0.0f);
    setVelocity(space.points, shape2, -2.0f, 0.0f);

    testCase(shape1, shape2, space.points, 0);
}


// The push-out applied after a penetrating point is snapped onto the entry
// edge must always separate it from the surface. The legacy
// normalize(v - 2n) direction lost most of its outward component by |v| = 2
// and inverted beyond that, so pin the property across the speed range and
// over every incidence angle - for both winding directions.
static void checkPushOutSeparates(PhysicsSpace &space, const Shape &shape, const char *label)
{
    ShapeIndexedRange range(shape);

    Vector2 centroid;
    for (int i = 0; i < range.size(); i++)
    {
        centroid += space.points.pos[range[i]];
    }
    centroid /= (float)range.size();

    const float speeds[] = {0.0f, 0.1f, 0.5f, 0.9f, 1.34f, 2.0f, 6.9f, 50.0f};

    for (int edge = 0; edge < range.size(); edge++)
    {
        Vector2 pm0Pos = space.points.pos[range[edge]];
        Vector2 pm1Pos = space.points.pos[range[(edge + 1) % range.size()]];

        // Computed exactly as the collision solver does.
        Vector2 segmentNormal = Vector2(-pm1Pos.y + pm0Pos.y, pm1Pos.x - pm0Pos.x).normalized();
        Vector2 edgeMidpoint = (pm0Pos + pm1Pos) * 0.5f;

        // These test shapes are convex, so the centroid is a valid reference
        // for which side is inside.
        Vector2 outward = (edgeMidpoint - centroid).normalized();

        for (int s = 0; s < 8; s++)
        {
            for (int degrees = -80; degrees <= 80; degrees += 20)
            {
                float radians = (float)degrees * PI_F / 180.0f;
                Vector2 velocity = segmentNormal.rotate(radians) * speeds[s];

                Vector2 direction = depenetrationDirection(velocity, segmentNormal, shape.windingSign);

                // Never NaN, and never pointing back into the shape.
                testAssertMsg(direction.x == direction.x && direction.y == direction.y, label);
                testAssertMsg(direction.dot(outward) > 0.5f, label);
            }
        }
    }
}

UNIT_TEST(PhysicsTestDepenetrationDirection)
{
    PhysicsSpace space;
    Shape quad = Shapes::createQuad(space, 100.0f, 100.0f, 40.0f, 40.0f, 1.0f);

    // The usual winding: perp(edge) points inward, so windingSign is +1.
    quad.windingSign = 1.0f;
    checkPushOutSeparates(space, quad, "normal winding");

    // A shape wound the other way, as found in some saved levels. Reversing
    // the point order flips segmentNormal to point outward, so the solver has
    // to compensate via windingSign.
    PhysicsSpace reversedSpace;
    Shapes::createQuad(reversedSpace, 100.0f, 100.0f, 40.0f, 40.0f, 1.0f);
    for (int i = 0; i < 2; i++)
    {
        Vector2 temp = reversedSpace.points.pos[i];
        reversedSpace.points.pos[i] = reversedSpace.points.pos[3 - i];
        reversedSpace.points.pos[3 - i] = temp;
    }

    Shape reversed = reversedSpace.shapes[0];
    reversed.windingSign = -1.0f;
    checkPushOutSeparates(reversedSpace, reversed, "reversed winding");
}

#endif