#include "Shapes.h"
#include "../containers/Array.h"
#include "./Physics.h"
#include <math.h>

namespace Shapes
{
    Shape createBridge(Array<PointMass> &points, Array<Spring> &springs, float x, float y, float mass, int numSegments)
    {
        int startIndex = points.size();
        int curIndex = startIndex;
        float curX = x;
        float width = 70.0f;
        float height = 70.0f;
        float diagonal = sqrt(width * width + height * height);
        float stiffness = 0.3f;
        float damping = 280.9f;

        for (int i = 0; i < numSegments; i++)
        {
            points.append({
                {curX, y, mass},
            });

            curX += width;
            curIndex++;
        }

        curX -= width;

        for (int i = 0; i < numSegments; i++)
        {
            points.append({
                {curX, y + height, mass},
            });

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
                springs.push({bottomIndex - 1, bottomIndex, width, stiffness, damping});

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

        return Shape{.start = startIndex, .end = curIndex, .isStatic = false};
    }

    Shape createQuad(Array<PointMass> &points, Array<Spring> &springs, float x, float y, float width, float height, float mass)
    {
        points.append({{x, y, mass},
                       {x + width, y, mass},
                       {x + width, y + height, mass},
                       {x, y + height, mass}});

        const Span span = {.start = points.size() - 4, .end = points.size()};
        const PointMass &p0 = points[span.start];
        const PointMass &p1 = points[span.start + 1];
        const PointMass &p2 = points[span.start + 2];
        const PointMass &p3 = points[span.start + 3];

        float stiffness = 0.3f;
        float damping = 28.9f;

        springs.push({span.start, span.start + 1, p0.pos.distance(p1.pos), stiffness, damping});
        springs.push({span.start + 1, span.start + 2, p1.pos.distance(p2.pos), stiffness, damping});
        springs.push({span.start + 2, span.start + 3, p2.pos.distance(p3.pos), stiffness, damping});
        springs.push({span.start + 3, span.start, p3.pos.distance(p0.pos), stiffness, damping});
        springs.push({span.start, span.start + 2, p0.pos.distance(p2.pos), stiffness, damping});
        springs.push({span.start + 1, span.start + 3, p1.pos.distance(p3.pos), stiffness, damping});

        return Shape{.start = span.start, .end = span.end, .isStatic = false};
    }
    Shape createParallelogram(Array<PointMass> &points, float x, float y, float width, float height, float sideOffset, float mass)
    {
        points.append({{x, y, mass},
                       {x + width, y, mass},
                       {x + width + sideOffset, y + height, mass},
                       {x + sideOffset, y + height, mass}});

        return Shape{.start = points.size() - 4, .end = points.size(), .isStatic = false};
    }

    Shape createTriangle(Array<PointMass> &points, float x0, float y0, float x1, float y1, float x2, float y2, float mass)
    {
        points.append({{x0, y0, mass},
                       {x1, y1, mass},
                       {x2, y2, mass}});

        const Span span = {.start = points.size() - 3, .end = points.size()};
        const PointMass &p0 = points[span.start];
        const PointMass &p1 = points[span.start + 1];
        const PointMass &p2 = points[span.start + 2];

        return Shape{.start = span.start, .end = span.end, .isStatic = false};
    }

    Shape createLine(Array<PointMass> &points, Array<Spring> &springs, float x0, float y0, float x1, float y1, float mass)
    {
        PointMass p0 = {x0, y0, mass};
        PointMass p1 = {x1, y1, mass};
        points.append({p0, p1});

        const Span span = {.start = points.size() - 2, .end = points.size()};
        springs.push({.pointA = span.start, .pointB = span.start + 1, .length = p0.pos.distance(p1.pos), .stiffness = 5.0f, .damping = 350.0f});

        return Shape{.start = span.start, .end = span.end, .isStatic = false};
    }
}
