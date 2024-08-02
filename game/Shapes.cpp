#include "Shapes.h"
#include "../containers/Array.h"
#include "./Physics.h"
#include <math.h>

int wrapIndex(int index, int size)
{
    if (index < 0)
    {
        return size + index;
    }
    else if (index >= size)
    {
        return index - size;
    }

    return index;
}

namespace Shapes
{
    Shape createCircle(int shapeIndex, PointMasses &points, Array<Spring> &springs, float x, float y, float radius, float mass)
    {
        float stiffness = 2.5f;
        float damping = 1080.9f;
        int numSegments = 16;
        float segmentAngle = 2 * M_PI / numSegments;
        int startIndex = points.size();
        int curIndex = startIndex;
        float curAngle = 0.0f;

        for (int i = 0; i < numSegments; i++)
        {
            points.push(
                x + cos(curAngle) * radius, y + sin(curAngle) * radius, mass);

            curAngle += segmentAngle;
            curIndex++;
        }

        Vector2 p0 = points.pos[startIndex];
        Vector2 p1 = points.pos[startIndex + 3];
        Vector2 p2 = points.pos[startIndex + 5];

        float lengthSpring1 = Vector2::vec2distance(p0.x, p0.y, p1.x, p1.y);
        float lengthSpring2 = Vector2::vec2distance(p0.x, p0.y, p2.x, p2.y);

        for (int i = 0; i < numSegments; i++)
        {
            int n0 = startIndex + i;
            int n1 = startIndex + wrapIndex(i + 3, numSegments);
            int n2 = startIndex + wrapIndex(i + 5, numSegments);

            springs.push(Spring(n0, n1, lengthSpring1, stiffness, damping, shapeIndex));
            springs.push(Spring(n2, n0, lengthSpring2, stiffness, damping, shapeIndex));
        }

        return Shape{.start = startIndex, .end = curIndex, .volume = 300.0f};
    }

    Shape createBridge(int shapeIndex, PointMasses &points, Array<Spring> &springs, float x, float y, float mass, int numSegments)
    {
        int startIndex = points.size();
        int curIndex = startIndex;
        float curX = x;
        float width = 70.0f;
        float height = 70.0f;
        float diagonal = sqrt(width * width + height * height);
        float stiffness = 6.5f;
        float damping = 280.9f;

        for (int i = 0; i < numSegments; i++)
        {
            points.push(curX, y, mass);

            curX += width;
            curIndex++;
        }

        curX -= width;

        for (int i = 0; i < numSegments; i++)
        {
            points.push(curX, y + height, mass);

            curX -= width;
            curIndex++;
        }

        for (int i = 0; i < numSegments; i++)
        {
            int topIndex = startIndex + i;
            int bottomIndex = startIndex + numSegments * 2 - i - 1;

            springs.push(Spring(topIndex, bottomIndex, height, stiffness, damping, shapeIndex));

            if (i > 0)
            {
                // Top bar
                springs.push(Spring(topIndex - 1, topIndex, width, stiffness, damping, shapeIndex));
                // Bottom bar
                springs.push(Spring(bottomIndex + 1, bottomIndex, width, stiffness, damping, shapeIndex));

                // Diagonal 1
                springs.push(Spring(topIndex, bottomIndex + 1, diagonal, stiffness, damping, shapeIndex));
                // Diagonal 2
                springs.push(Spring(bottomIndex, topIndex - 1, diagonal, stiffness, damping, shapeIndex));
            }
        }

        curX -= width;

        return Shape{.start = startIndex, .end = curIndex};
    }

    Shape createStaticQuad(int shapeIndex, PointMasses &points, float x, float y, float width, float height, float mass)
    {
        points.push(x, y, mass);
        points.push(x + width, y, mass);
        points.push(x + width, y + height, mass);
        points.push(x, y + height, mass);

        const Span span = {.start = points.size() - 4, .end = points.size()};

        return Shape{.start = span.start, .end = span.end};
    }

    Shape createQuad(int shapeIndex, PointMasses &points, Array<Spring> &springs, float x, float y, float width, float height, float mass)
    {
        Shape quad = createStaticQuad(shapeIndex, points, x, y, width, height, mass);

        const Span span = {.start = points.size() - 4, .end = points.size()};
        Vector2 p0 = points.pos[span.start];
        Vector2 p1 = points.pos[span.start + 1];
        Vector2 p2 = points.pos[span.start + 2];
        Vector2 p3 = points.pos[span.start + 3];

        float stiffness = 1.5f;
        float damping = 28.9f;

        springs.push(Spring(span.start, span.start + 1, Vector2::vec2distance(p0.x, p0.y, p1.x, p1.y), stiffness, damping, shapeIndex));
        springs.push(Spring(span.start + 1, span.start + 2, Vector2::vec2distance(p1.x, p1.y, p2.x, p2.y), stiffness, damping, shapeIndex));
        springs.push(Spring(span.start + 2, span.start + 3, Vector2::vec2distance(p2.x, p2.y, p3.x, p3.y), stiffness, damping, shapeIndex));
        springs.push(Spring(span.start + 3, span.start, Vector2::vec2distance(p3.x, p3.y, p0.x, p0.y), stiffness, damping, shapeIndex));
        springs.push(Spring(span.start, span.start + 2, Vector2::vec2distance(p0.x, p0.y, p2.x, p2.y), stiffness, damping, shapeIndex));
        springs.push(Spring(span.start + 1, span.start + 3, Vector2::vec2distance(p1.x, p1.y, p3.x, p3.y), stiffness, damping, shapeIndex));

        return quad;
    }
    Shape createParallelogram(int shapeIndex, PointMasses &points, float x, float y, float width, float height, float sideOffset, float mass)
    {
        points.push(x, y, mass);
        points.push(x + width, y, mass);
        points.push(x + width + sideOffset, y + height, mass);
        points.push(x + sideOffset, y + height, mass);

        return Shape{.start = points.size() - 4, .end = points.size()};
    }

    Shape createTriangle(int shapeIndex, PointMasses &points, float x0, float y0, float x1, float y1, float x2, float y2, float mass)
    {
        points.push(x0, y0, mass);
        points.push(x1, y1, mass);
        points.push(x2, y2, mass);

        const Span span = {.start = points.size() - 3, .end = points.size()};

        return Shape{.start = span.start, .end = span.end};
    }

    Shape createLine(int shapeIndex, PointMasses &points, Array<Spring> &springs, float x0, float y0, float x1, float y1, float mass)
    {
        points.push(x0, y0, mass);
        points.push(x1, y1, mass);

        const Span span = {.start = points.size() - 2, .end = points.size()};
        float lineLength = Vector2::vec2distance(x0, y0, x1, y1);
        springs.push(Spring(span.start, span.start + 1, lineLength, 1.5f, 28.9f, shapeIndex));

        return Shape{.start = span.start, .end = span.end};
    }
}
