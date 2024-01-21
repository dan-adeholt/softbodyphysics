#ifndef __PHYSICS__TEST_H
#define __PHYSICS__TEST_H

#include "Physics.h"
#include "../containers/Array.h"
#include "./Shapes.h"

void testCase(Shape& shape1, Shape& shape2, Array<PointMass>& points, int expectedCollisions, const Range<int> counterForCollisions) {
    static Array<Shape> shapes;
    static Array<ShapeBoundingBox> boundingBoxes;    
    shapes.clear();
    boundingBoxes.clear();
    shapes.append({ shape1, shape2 });
    
    calculateBoundingBoxes(boundingBoxes, shapes, points);

    Range<PointMass> shape1Points = points.range(shape1);
    Range<PointMass> shape2Points = points.range(shape2);
    Range<int> collisionRange1 = counterForCollisions.slice(shape1);
    Range<int> collisionRange2 = counterForCollisions.slice(shape2);

    assert(calculateCollisions(shape1Points, shape2Points, boundingBoxes[0], boundingBoxes[1], collisionRange1, collisionRange2) == expectedCollisions);
}

void testSprings() {
    Array<Spring> springs;
    Array<PointMass> points;
    Shape shape1 = Shapes::createLine(points, springs, 100.0f, 100.0f, 100.0f, 300.0f, 1.0f);
    PointMass& p1 = points[shape1.end - 1];

    // Test that the spring is pulling the point towards the other end
    p1.pos.y += 10.0f;
    Range<PointMass> pointRange = points.range();
    Range<Spring> springRange = springs.range();
    // printf("p1.pos: %f %f\n", p1.pos.x, p1.pos.y);
    // applySprings(pointRange, springRange, 1.0f);
    // printf("p1.pos: %f %f\n", p1.pos.x, p1.pos.y);
    // applySprings(pointRange, springRange, 1.0f);
    printf("p1.pos: %f %f\n", p1.pos.x, p1.pos.y);
    // printf("p1.pos: %f %f\n", p1.pos.x, p1.pos.y);
    // applySprings(pointRange, springRange, 1.0f);
    // printf("p1.pos: %f %f\n", p1.pos.x, p1.pos.y);
}

void testCollisions() {
    Array<Spring> springs;
    Array<PointMass> points;
    Array<int> counterForCollisions;
    // One corner of a quad is inside another quad
    Shape shape1 = Shapes::createQuad(points, springs, 0.0f, 0.0f, 32.0f, 32.0f, 1.0f);
    Shape shape2 = Shapes::createQuad(points, springs, 16.0f, 16.0f, 32.0f, 32.0f, 1.0f);
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