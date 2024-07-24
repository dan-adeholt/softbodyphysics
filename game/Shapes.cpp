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
    Shape createCircle(PointMasses &points, Array<Spring> &springs, float x, float y, float radius, float mass)
    {
        float stiffness = 0.5f;
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

        float p0x = points.x[startIndex];
        float p0y = points.y[startIndex];
        float p1x = points.x[startIndex + 3];
        float p1y = points.y[startIndex + 3];
        float p2x = points.x[startIndex + 5];
        float p2y = points.y[startIndex + 5];

        float lengthSpring1 = Vector2::vec2distance(p0x, p0y, p1x, p1y);
        float lengthSpring2 = Vector2::vec2distance(p0x, p0y, p2x, p2y);

        for (int i = 0; i < numSegments; i++)
        {
            int n0 = startIndex + i;
            int n1 = startIndex + wrapIndex(i + 3, numSegments);
            int n2 = startIndex + wrapIndex(i + 5, numSegments);

            springs.push({n0, n1, lengthSpring1, stiffness, damping});
            springs.push({n2, n0, lengthSpring2, stiffness, damping});
        }

        return Shape{.start = startIndex, .end = curIndex, .volume = 300.0f};
    }

    Shape createBridge(PointMasses &points, Array<Spring> &springs, float x, float y, float mass, int numSegments)
    {
        int startIndex = points.size();
        int curIndex = startIndex;
        float curX = x;
        float width = 70.0f;
        float height = 70.0f;
        float diagonal = sqrt(width * width + height * height);
        float stiffness = 1.3f;
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

            springs.push({topIndex, bottomIndex, height, stiffness, damping});

            if (i > 0)
            {
                // Top bar
                springs.push({topIndex - 1, topIndex, width, stiffness, damping});
                // Bottom bar
                springs.push({bottomIndex + 1, bottomIndex, width, stiffness, damping});

                // Diagonal 1
                springs.push({topIndex, bottomIndex + 1, diagonal, stiffness, damping});
                // Diagonal 2
                springs.push({bottomIndex, topIndex - 1, diagonal, stiffness, damping});
            }
        }

        curX -= width;

        // for (int i = 0; i < numSegments; i++)
        // {
        //     points.append({
        //         {curX, y, mass},
        //         {curX, y + height, mass},
        //     });

        //     springs.push({curIndex, curIndex + 1, height, stiffness, damping});

        //     if (i > 0)
        //     {
        //         // Top bar
        //         springs.push({curIndex - 2, curIndex, width, stiffness, damping});
        //         // Bottom bar
        //         springs.push({curIndex - 1, curIndex + 1, width, stiffness, damping});
        //         // Diagonal 1
        //         springs.push({curIndex - 1, curIndex, diagonal, stiffness, damping});
        //         // Diagonal 2
        //         springs.push({curIndex - 2, curIndex + 1, diagonal, stiffness, damping});
        //     }

        //     curIndex += 2;
        //     curX += width;
        // }

        return Shape{.start = startIndex, .end = curIndex};
    }

    Shape createStaticQuad(PointMasses &points, float x, float y, float width, float height, float mass)
    {
        points.push(x, y, mass);
        points.push(x + width, y, mass);
        points.push(x + width, y + height, mass);
        points.push(x, y + height, mass);

        const Span span = {.start = points.size() - 4, .end = points.size()};

        return Shape{.start = span.start, .end = span.end};
    }

    Shape createQuad(PointMasses &points, Array<Spring> &springs, float x, float y, float width, float height, float mass)
    {
        Shape quad = createStaticQuad(points, x, y, width, height, mass);

        const Span span = {.start = points.size() - 4, .end = points.size()};
        float p0x = points.x[span.start];
        float p0y = points.y[span.start];
        float p1x = points.x[span.start + 1];
        float p1y = points.y[span.start + 1];
        float p2x = points.x[span.start + 2];
        float p2y = points.y[span.start + 2];
        float p3x = points.x[span.start + 3];
        float p3y = points.y[span.start + 3];

        float stiffness = 0.3f;
        float damping = 28.9f;

        springs.push({span.start, span.start + 1, Vector2::vec2distance(p0x, p0y, p1x, p1y), stiffness, damping});
        springs.push({span.start + 1, span.start + 2, Vector2::vec2distance(p1x, p1y, p2x, p2y), stiffness, damping});
        springs.push({span.start + 2, span.start + 3, Vector2::vec2distance(p2x, p2y, p3x, p3y), stiffness, damping});
        springs.push({span.start + 3, span.start, Vector2::vec2distance(p3x, p3y, p0x, p0y), stiffness, damping});
        springs.push({span.start, span.start + 2, Vector2::vec2distance(p0x, p0y, p2x, p2y), stiffness, damping});
        springs.push({span.start + 1, span.start + 3, Vector2::vec2distance(p1x, p1y, p3x, p3y), stiffness, damping});

        return quad;
    }
    Shape createParallelogram(PointMasses &points, float x, float y, float width, float height, float sideOffset, float mass)
    {
        points.push(x, y, mass);
        points.push(x + width, y, mass);
        points.push(x + width + sideOffset, y + height, mass);
        points.push(x + sideOffset, y + height, mass);

        return Shape{.start = points.size() - 4, .end = points.size()};
    }

    Shape createTriangle(PointMasses &points, float x0, float y0, float x1, float y1, float x2, float y2, float mass)
    {
        points.push(x0, y0, mass);
        points.push(x1, y1, mass);
        points.push(x2, y2, mass);

        const Span span = {.start = points.size() - 3, .end = points.size()};

        return Shape{.start = span.start, .end = span.end};
    }

    Shape createLine(PointMasses &points, Array<Spring> &springs, float x0, float y0, float x1, float y1, float mass)
    {
        points.push(x0, y0, mass);
        points.push(x1, y1, mass);

        const Span span = {.start = points.size() - 2, .end = points.size()};
        float lineLength = Vector2::vec2distance(x0, y0, x1, y1);
        springs.push({.pointA = span.start, .pointB = span.start + 1, .length = lineLength, .stiffness = 0.3f, .damping = 28.9f});

        return Shape{.start = span.start, .end = span.end};
    }
}
