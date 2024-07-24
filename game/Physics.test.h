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

void testFindClosestLineSegmentToPoint()
{
    PointMasses points;
    Array<Spring> springs;
    Shape shelf = Shapes::createTriangle(points, 50.0f, 700.0f, 800.0f, 500.0f, 800.0f, 700.0f, 0.0f);
    Vector2 pos1(500.0, 588.0);
    int minIndex = -1;
    float minT = 0.0f;
    Vector2 minPoint = {0.0f, 0.0f};

    findClosestLineSegmentToPoint(points.range(shelf), pos1, Vector2(1.0f, 0.0f), minIndex, minPoint, minT);
    testExpectInt(minIndex, 0);
    float size = 100.0f;

    Vector2 pos2(400.0f, 220.0f);
    Shape q1 = Shapes::createQuad(points, springs, 400.0f, 220.0f, size, size, 1.0f);
    Vector2 velocity(1, 0);
    findClosestLineSegmentToPoint(points.range(q1), pos2, velocity, minIndex, minPoint, minT);

    Vector2 segmentNormal(0, 1);
    Vector2 segmentNormal2(1, 0);
    Vector2 segmentNormal3(1, 0);

    Console::log("t1: %.2f", velocity.dot(segmentNormal));
    Console::log("t2: %.2f", velocity.dot(segmentNormal2));
    Console::log("t3: %.2f", velocity.dot(segmentNormal3));

    Console::log("Min index found: %d", minIndex);
}

void testCollisions()
{
    testFindClosestLineSegmentToPoint();
    return;
    Array<Spring> springs;
    PointMasses points;
    Array<int> counterForCollisions;

    // One half of a quad is inside another quad
    Shape shape1 = Shapes::createQuad(points, springs, 31.9f, 0.1f, 32.0f, 32.0f, 1.0f);
    Shape shape2 = Shapes::createQuad(points, springs, 0.0f, 0.0f, 32.0f, 32.0f, 1.0f);

    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 1, counterForCollisions.range());

    // One corner of a quad is inside another quad
    shape1 = Shapes::createQuad(points, springs, 0.0f, 0.0f, 32.0f, 32.0f, 1.0f);
    shape2 = Shapes::createQuad(points, springs, 16.0f, 16.0f, 32.0f, 32.0f, 1.0f);
    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 1, counterForCollisions.range());

    // One quad is completely inside another quad
    shape1 = Shapes::createQuad(points, springs, 0.0f, 0.0f, 32.0f, 32.0f, 1.0f);
    shape2 = Shapes::createQuad(points, springs, 16.0f, 16.0f, 4.0f, 4.0f, 1.0f);
    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 4, counterForCollisions.range());

    // One quad is completely inside a parallelogram
    shape1 = Shapes::createParallelogram(points, 0.0f, 0.0f, 32.0f, 32.0f, 10.0f, 1.0f);
    shape2 = Shapes::createQuad(points, springs, 16.0f, 16.0f, 4.0f, 4.0f, 1.0f);
    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 4, counterForCollisions.range());

    // One corner of a triangle is inside another triangle
    shape1 = Shapes::createTriangle(points, 10.0f, 0.0f, 16.0f, 5.0f, 5.0f, 10.0f, 1.0f);
    shape2 = Shapes::createTriangle(points, 10.0f, 3.0f, 11.0f, 13.0f, 7.0f, 16.0f, 1.0f);
    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 1, counterForCollisions.range());

    // Two triangles close to eachother, but not intersecting
    shape1 = Shapes::createTriangle(points, 10.0f, 0.0f, 16.0f, 5.0f, 5.0f, 10.0f, 1.0f);
    shape2 = Shapes::createTriangle(points, 16.0f, 6.0f, 11.0f, 13.0f, 7.0f, 16.0f, 1.0f);
    counterForCollisions.fill(0, points.size());
    testCase(shape1, shape2, points, 0, counterForCollisions.range());
}

#endif