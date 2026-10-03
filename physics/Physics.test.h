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
#include "../game/Game.h"
#include "../game/Scenes.h"

// Ray casting point in polygon, for checking where points ended up after collisions were resolved
static bool pointInsideShape(const PointMasses &points, const Shape &shape, Vector2 point)
{
    ShapeIndexedRange range(shape);
    bool inside = false;

    for (int i = 0, j = range.size() - 1; i < range.size(); j = i++)
    {
        Vector2 a = points.pos[range[i]];
        Vector2 b = points.pos[range[j]];

        if ((a.y > point.y) != (b.y > point.y) && point.x < (b.x - a.x) * (point.y - a.y) / (b.y - a.y) + a.x)
        {
            inside = !inside;
        }
    }

    return inside;
}

static int pointsInsideShape(const PointMasses &points, const Shape &shape, const Shape &other)
{
    ShapeIndexedRange range(other);
    int count = 0;

    for (int i = 0; i < range.size(); i++)
    {
        count += pointInsideShape(points, shape, points.pos[range[i]]) ? 1 : 0;
    }

    return count;
}

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

    {
        testExpectInt(CollisionSolver::calculateCollisions(range, shape1, shape2, boundingBoxes[0], boundingBoxes[1]), expectedCollisions);
    }

    // Whatever was found inside has been moved out
    {
        testExpectInt(pointsInsideShape(points, shape1, shape2), 0);
    }
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

    // One quad is completely inside a parallelogram. All four corners start inside, but the parallelogram
    // shares each correction by mass, and after three of them it has moved clear of the fourth corner.
    shape1 = Shapes::createParallelogram(space, 0.0f, 0.0f, 32.0f, 32.0f, 10.0f, 1.0f);
    shape2 = Shapes::createQuad(space, 16.0f, 16.0f, 4.0f, 4.0f, 1.0f);
    setVelocity(space.points, shape1, 1.0f, 0.0f);
    setVelocity(space.points, shape2, -2.0f, 0.0f);

    testCase(shape1, shape2, space.points, 3);

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

// Drops a circle of the given mass onto the Bridge scene, lets everything settle and measures jitter:
// how much each point's velocity changes from one collision pass to the next, averaged over a stretch
// once the scene has settled. Smooth motion, like the bridge slowly swaying, changes velocity little
// between passes; jitter keeps flipping it. Reported for the dropped circle and for the whole scene.
struct SettleJitter
{
    float circle;
    float scene;
};

static SettleJitter settleJitterOnBridge(float circleMass)
{
    Game game("levels", nullptr);
    game.init("Bridge");
    PhysicsSpace &space = game.physicsSpace();
    Shape circle = Shapes::createCircle(space, 655.0f, 150.0f, gridSize, circleMass);
    game.updateBoundingBoxes();

    ConsoleProfileInfo profileInfo;
    const int settleSteps = 6000;
    const int measureSteps = 2000;
    // Collisions are resolved every second step, so compare velocities two steps apart
    const int passLength = 2;
    Array<Vector2> previousVelocity;
    double circleJitter = 0.0;
    double sceneJitter = 0.0;
    int samples = 0;

    for (int step = 0; step < settleSteps + measureSteps; step++)
    {
        game.update(0.0, true, profileInfo);

        if (step < settleSteps - passLength || step % passLength != 0)
        {
            continue;
        }

        if (previousVelocity.size() == space.points.size())
        {
            ShapeIndexedRange range(space.shapes[circle.index]);

            for (int i = 0; i < range.size(); i++)
            {
                circleJitter += (space.points.velocity[range[i]] - previousVelocity[range[i]]).length() / static_cast<double>(range.size());
            }

            for (int i = 0; i < space.points.size(); i++)
            {
                sceneJitter += (space.points.velocity[i] - previousVelocity[i]).length() / static_cast<double>(space.points.size());
            }

            samples++;
        }

        previousVelocity.replace(space.points.velocity);
    }

    return {static_cast<float>(circleJitter / samples), static_cast<float>(sceneJitter / samples)};
}

// A circle six times as heavy as the rest, as added from the right click menu, used to keep being
// pushed fully out of the light balls it rested on and sinking back in. That measured about 0.013 for
// the circle and 0.0024 for the scene; sharing the collision correction by mass brings them down to
// about 0.0002 and 0.0011.
UNIT_TEST(PhysicsTestHeavyCircleSettles)
{
    SettleJitter heavy = settleJitterOnBridge(12.0f);
    Console::log("Jitter with a heavy circle on the bridge: circle %.5f, scene %.5f", heavy.circle, heavy.scene);
    testAssert(heavy.circle < 0.002f);
    testAssert(heavy.scene < 0.0018f);
}

#endif