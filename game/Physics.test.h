#ifndef __PHYSICS__TEST_H
#define __PHYSICS__TEST_H

#include "Physics.h"
#include "../containers/Array.h"
#include "./Shapes.h"
#include "../utils/Console.h"
#include "../utils/UnitTestUtil.h"

void testCase(Shape &shape1, Shape &shape2, PointMasses &points, int expectedCollisions, const Range<int> counterForCollisions)
{
    static Array<Shape> shapes;
    static Array<ShapeBoundingBox> boundingBoxes;
    shapes.clear();
    boundingBoxes.clear();
    shapes.append({shape1, shape2});

    calculateBoundingBoxes(boundingBoxes, shapes, points);

    PointMassesRange shape1Points = points.range(shape1);
    PointMassesRange shape2Points = points.range(shape2);
    Range<int> collisionRange1 = counterForCollisions.slice(shape1);
    Range<int> collisionRange2 = counterForCollisions.slice(shape2);

    testExpectInt(calculateCollisions(shape1Points, shape2Points, boundingBoxes[0], boundingBoxes[1], collisionRange1, collisionRange2, 1.0), expectedCollisions);
}

UNIT_TEST(testFindClosestLineSegmentToPoint, "Physics")
{
    PointMasses points;
    Array<Spring> springs;
    Shape shelf = Shapes::createTriangle(0, points, 50.0f, 700.0f, 800.0f, 500.0f, 800.0f, 700.0f, 0.0f);
    Vector2 pos1(500.0, 588.0);
    int minIndex = -1;
    float minT = 0.0f;
    Vector2 minPoint = {0.0f, 0.0f};

    findClosestLineSegmentToPoint(points.range(shelf), pos1, Vector2(1.0f, 0.0f), minIndex, minPoint, minT);
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
    PointMasses points;
    Array<Spring> springs;
    Shape shape = Shapes::createQuad(0, points, springs, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f);
    PointMassesRange range = points.range(shape);

    for (int i = 0; i < range.size(); i++)
    {
        Vector2 rotated = range.pos[i].rotate(-PI / 2.0f);
        range.pos[i] = rotated;
    }

    printf("Rotated by: %f\n", -PI / 2.0f);
    Array<Shape> shapes;
    shapes.push(shape);

    shapeMatchAlign(points.range(), shapes);

    for (int i = 0; i < range.size(); i++)
    {
        Vector2 diff = range.shapePos[i] - range.pos[i];
        testAssertMsg(diff.length() < 0.000001f, "After shape matching the points should be in the same position");
    }
}

UNIT_TEST(testCollisions, "Physics")
{
    Array<Spring> springs;
    PointMasses points;
    Array<int> counterForCollisions;

    // One half of a quad is inside another quad
    Shape shape1 = Shapes::createQuad(0, points, springs, 31.9f, 0.1f, 32.0f, 32.0f, 1.0f);
    Shape shape2 = Shapes::createQuad(1, points, springs, 0.0f, 0.0f, 32.0f, 32.0f, 1.0f);
    setVelocity(points, shape1, 1.0f, 0.0f);
    setVelocity(points, shape2, -2.0f, 0.0f);
    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 1, counterForCollisions.range());

    // One corner of a quad is inside another quad
    shape1 = Shapes::createQuad(2, points, springs, 0.0f, 0.0f, 32.0f, 32.0f, 1.0f);
    shape2 = Shapes::createQuad(3, points, springs, 16.0f, 16.0f, 32.0f, 32.0f, 1.0f);
    setVelocity(points, shape1, 1.0f, 0.0f);
    setVelocity(points, shape2, -2.0f, 0.0f);
    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 1, counterForCollisions.range());

    // One quad is completely inside another quad
    shape1 = Shapes::createQuad(4, points, springs, 0.0f, 0.0f, 32.0f, 32.0f, 1.0f);
    shape2 = Shapes::createQuad(5, points, springs, 16.0f, 16.0f, 4.0f, 4.0f, 1.0f);
    setVelocity(points, shape1, 1.0f, 0.0f);
    setVelocity(points, shape2, -2.0f, 0.0f);

    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 4, counterForCollisions.range());

    // One quad is completely inside a parallelogram
    shape1 = Shapes::createParallelogram(6, points, 0.0f, 0.0f, 32.0f, 32.0f, 10.0f, 1.0f);
    shape2 = Shapes::createQuad(7, points, springs, 16.0f, 16.0f, 4.0f, 4.0f, 1.0f);
    setVelocity(points, shape1, 1.0f, 0.0f);
    setVelocity(points, shape2, -2.0f, 0.0f);

    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 4, counterForCollisions.range());

    // One corner of a triangle is inside another triangle
    shape1 = Shapes::createTriangle(8, points, 10.0f, 0.0f, 16.0f, 5.0f, 5.0f, 10.0f, 1.0f);
    shape2 = Shapes::createTriangle(9, points, 10.0f, 3.0f, 11.0f, 13.0f, 7.0f, 16.0f, 1.0f);
    setVelocity(points, shape1, 1.0f, 0.0f);
    setVelocity(points, shape2, -2.0f, 0.0f);

    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 1, counterForCollisions.range());

    // Two triangles close to eachother, but not intersecting
    shape1 = Shapes::createTriangle(10, points, 10.0f, 0.0f, 16.0f, 5.0f, 5.0f, 10.0f, 1.0f);
    shape2 = Shapes::createTriangle(11, points, 16.0f, 6.0f, 11.0f, 13.0f, 7.0f, 16.0f, 1.0f);
    setVelocity(points, shape1, 1.0f, 0.0f);
    setVelocity(points, shape2, -2.0f, 0.0f);

    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 0, counterForCollisions.range());
}

#endif