#ifndef __SHAPE_UTILS_H
#define __SHAPE_UTILS_H

#include "../math/Vector2.h"
#include "Physics.h"

struct Shape;
struct PointMassesRange;

struct ShapeIterator
{
    int cur = 0;
    int start = 0;
    int end = 0;
    uint16_t indices[4];
    bool indexed = false;

    ShapeIterator(const Shape &shape) : start(shape.start), end(shape.end)
    {
        indexed = shape.hasIndices();
        cur = indexed ? 0 : start;

        for (int i = 0; i < 4; i++)
        {
            indices[i] = shape.indices[i];
        }
    }

    bool isValid() const
    {
        return indexed ? cur < 4 : cur < end;
    }

    int index()
    {
        return indexed ? (start + indices[cur]) : cur;
    }

    void next()
    {
        cur++;
    }
};

namespace ShapeUtils
{
    Vector2 getAverageShapeVelocity(PointMassesRange points, const Shape &shape);
    ShapeProperties getShapeProperties(PointMassesRange points, const Shape &shape);
    Vector2 getShapePos(const PointMassesRange &points, const Shape &shape, int j, const ShapeProperties &averages, const ShapeMatchDragData &dragData);
}

#endif