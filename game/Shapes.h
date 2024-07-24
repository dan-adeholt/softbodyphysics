#ifndef __SHAPES_H
#define __SHAPES_H

template <typename T>
class Range;

template <typename T>
class Array;

struct Spring;
struct Shape;
struct PointMasses;

namespace Shapes
{
    Shape createCircle(PointMasses &points, Array<Spring> &springs, float x, float y, float radius, float mass);

    Shape createBridge(PointMasses &points, Array<Spring> &springs, float x, float y, float mass, int numSegments);
    Shape createLine(PointMasses &points, Array<Spring> &springs, float x0, float y0, float x1, float y1, float drag);
    Shape createQuad(PointMasses &points, Array<Spring> &springs, float x, float y, float width, float height, float drag);
    Shape createStaticQuad(PointMasses &points, float x, float y, float width, float height, float mass);
    Shape createParallelogram(PointMasses &points, float x, float y, float width = 32.0f, float height = 32.0f, float sideOffset = 0.0f, float drag = 1.0f);
    Shape createTriangle(PointMasses &points, float x0, float y0, float x1, float y1, float x2, float y2, float drag = 1.0f);
}

#endif