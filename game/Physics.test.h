#ifndef __PHYSICS__TEST_H
#define __PHYSICS__TEST_H

#include "Physics.h"
#include "../containers/Array.h"
#include "./Shapes.h"
#include "../utils/Console.h"
#include "../utils/UnitTestUtil.h"

void testCase(Shape &shape1, Shape &shape2, PointMasses &points, int expectedCollisions)
{
    static Array<Shape> shapes;
    static Array<ShapeBoundingBox> boundingBoxes;
    shapes.clear();
    boundingBoxes.clear();
    shapes.append({shape1, shape2});

    calculateBoundingBoxes(boundingBoxes, shapes, points);

    PointMassesRange shape1Points = points.range(shape1);
    PointMassesRange shape2Points = points.range(shape2);

    testExpectInt(calculateCollisions(shape1Points, shape2Points, shape2Points, boundingBoxes[0], boundingBoxes[1], 1.0), expectedCollisions);
}

UNIT_TEST(testFindClosestLineSegmentToPoint, "Physics")
{
    PhysicsSpace space;
    Shape shelf = Shapes::createTriangle(space, false, 50.0f, 700.0f, 800.0f, 500.0f, 800.0f, 700.0f, 0.0f);
    Vector2 pos1(500.0, 588.0);
    int minIndex = -1;
    float minT = 0.0f;
    Vector2 minPoint = {0.0f, 0.0f};

    findClosestLineSegmentToPoint(space.points.range(shelf), pos1, Vector2(1.0f, 0.0f), minIndex, minPoint, minT);
    testExpectInt(minIndex, 0);
}

void setVelocity(PointMasses &points, Shape shape, float vx, float vy)
{
    PointMassesRange range = points.range(shape);
    for (int i = 0; i < range.size(); i++)
    {
        range.velocity[i].x = vx;
        range.velocity[i].y = vy;
    }
}

UNIT_TEST(testShapeMatching, "Physics")
{
    PhysicsSpace space;
    Shape shape = Shapes::createQuad(space, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f);
    PointMassesRange range = space.points.range(shape);

    for (int i = 0; i < range.size(); i++)
    {
        Vector2 rotated = range.pos[i].rotate(-PI_F / 2.0f);
        range.pos[i] = rotated;
    }

    printf("Rotated by: %f\n", -PI / 2.0f);

    shapeMatchAlign(space.points.range(), space.shapes, space.draggingShapeIndex);

    for (int i = 0; i < range.size(); i++)
    {
        Vector2 diff = range.shapePos[i] - range.pos[i];
        testAssertMsg(diff.length() < 0.000001f, "After shape matching the points should be in the same position");
    }
}

UNIT_TEST(testCollisions, "Physics")
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

#endif