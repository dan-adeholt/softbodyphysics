#ifndef __SHAPES_H
#define __SHAPES_H

template <typename T>
class Range;

template <typename T>
class Array;

struct Spring;
struct Shape;
struct PointMass;

namespace Shapes
{
    Shape createBridge(Array<PointMass> &points, Array<Spring> &springs, float x, float y, float mass, int numSegments);
    Shape createLine(Array<PointMass> &points, Array<Spring> &springs, float x0, float y0, float x1, float y1, float drag);
    Shape createQuad(Array<PointMass> &points, Array<Spring> &springs, float x, float y, float width, float height, float drag);
    Shape createParallelogram(Array<PointMass> &points, float x, float y, float width = 32.0f, float height = 32.0f, float sideOffset = 0.0f, float drag = 1.0f);
    Shape createTriangle(Array<PointMass> &points, float x0, float y0, float x1, float y1, float x2, float y2, float drag = 1.0f);
}

#endif