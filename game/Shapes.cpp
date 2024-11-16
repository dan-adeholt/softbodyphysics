#include "Shapes.h"
#include "../containers/Array.h"
#include "./PhysicsSpace.h"
#include "./PhysicsShapeMatching.h"
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
    Shape createCircle(PhysicsSpace &space, float x, float y, float radius, float mass)
    {
        int shapeIndex = space.nextShapeIndex();
        float stiffness = 2.5f;
        float damping = 1080.9f;
        int numSegments = 16;
        float segmentAngle = 2 * PI_F / numSegments;
        int startIndex = space.points.size();
        int curIndex = startIndex;
        float curAngle = 0.0f;

        for (int i = 0; i < numSegments; i++)
        {
            space.points.push(
                x + cos(curAngle) * radius, y + sin(curAngle) * radius, mass);

            curAngle += segmentAngle;
            curIndex++;
        }

        Vector2 p0 = space.points.pos[startIndex];
        Vector2 p1 = space.points.pos[startIndex + 3];
        Vector2 p2 = space.points.pos[startIndex + 5];

        float lengthSpring1 = Vector2::vec2distance(p0.x, p0.y, p1.x, p1.y);
        float lengthSpring2 = Vector2::vec2distance(p0.x, p0.y, p2.x, p2.y);

        for (int i = 0; i < numSegments; i++)
        {
            int n0 = startIndex + i;
            int n1 = startIndex + wrapIndex(i + 3, numSegments);
            int n2 = startIndex + wrapIndex(i + 5, numSegments);

            space.springs.push(Spring(n0, n1, lengthSpring1, stiffness, damping, shapeIndex));
            space.springs.push(Spring(n2, n0, lengthSpring2, stiffness, damping, shapeIndex));
        }

        Shape circle = Shape(startIndex, curIndex, 300.0f);
        space.shapes.push(circle);
        PhysicsShapeMatching::shapeMatchAlignInit(space.points.range(), circle, space.partialShapes.range());
        return circle;
    }

    Shape createBridge(PhysicsSpace &space, float x, float y, float mass, int numSegments)
    {
        int shapeIndex = space.nextShapeIndex();
        int startIndex = space.points.size();
        int curIndex = startIndex;
        float curX = x;
        float width = 70.0f;
        float height = 70.0f;
        float diagonal = sqrt(width * width + height * height);
        float stiffness = 6.5f;
        float damping = 280.9f;

        for (int i = 0; i < numSegments; i++)
        {
            space.points.push(curX, y, mass);

            curX += width;
            curIndex++;
        }

        curX -= width;

        for (int i = 0; i < numSegments; i++)
        {
            space.points.push(curX, y + height, mass);

            curX -= width;
            curIndex++;
        }
        int partialShapesStart = space.partialShapes.size();

        for (int i = 0; i < numSegments; i++)
        {
            int topIndex = startIndex + i;
            int bottomIndex = startIndex + numSegments * 2 - i - 1;

            space.springs.push(Spring(topIndex, bottomIndex, height, stiffness, damping, shapeIndex));

            if (i > 0)
            {
                Vector2 topLeft = space.points.pos[topIndex];
                Vector2 topRight = space.points.pos[topIndex - 1];
                Vector2 bottomLeft = space.points.pos[bottomIndex + 1];
                Vector2 bottomRight = space.points.pos[bottomIndex];

                ShapeQuad quad = {{topIndex - 1, topIndex, bottomIndex, bottomIndex + 1}, {topLeft, topRight, bottomLeft, bottomRight}, {topLeft, topRight, bottomLeft, bottomRight}, 4};
                space.partialShapes.push(quad);

                // Top bar
                space.springs.push(Spring(topIndex - 1, topIndex, width, stiffness, damping, shapeIndex));
                // Bottom bar
                space.springs.push(Spring(bottomIndex + 1, bottomIndex, width, stiffness, damping, shapeIndex));

                // Diagonal 1
                space.springs.push(Spring(topIndex, bottomIndex + 1, diagonal, stiffness, damping, shapeIndex));
                // Diagonal 2
                space.springs.push(Spring(bottomIndex, topIndex - 1, diagonal, stiffness, damping, shapeIndex));
            }
        }

        curX -= width;

        Shape bridge = Shape(startIndex, curIndex);
        bridge.subShapeSpan = {partialShapesStart, space.partialShapes.size()};
        PhysicsShapeMatching::shapeMatchAlignInit(space.points.range(), bridge, space.partialShapes.range());
        space.shapes.push(bridge);
        return bridge;
    }

    Shape createStaticQuad(PhysicsSpace &space, float x, float y, float width, float height, float mass)
    {
        space.staticPoints.push(x, y, mass);
        space.staticPoints.push(x + width, y, mass);
        space.staticPoints.push(x + width, y + height, mass);
        space.staticPoints.push(x, y + height, mass);

        const Span span(space.staticPoints.size() - 4, space.staticPoints.size());
        Shape quad = Shape(span.start, span.end);
        PhysicsShapeMatching::shapeMatchAlignInit(space.staticPoints.range(), quad, space.partialShapes.range());
        space.staticShapes.push(quad);

        return quad;
    }

    Shape createQuad(PhysicsSpace &space, float x, float y, float width, float height, float mass)
    {
        int shapeIndex = space.nextShapeIndex();
        space.points.push(x, y, mass);
        space.points.push(x + width, y, mass);
        space.points.push(x + width, y + height, mass);
        space.points.push(x, y + height, mass);

        const Span span(space.points.size() - 4, space.points.size());
        Shape quad = Shape(span.start, span.end);
        PhysicsShapeMatching::shapeMatchAlignInit(space.points.range(), quad, space.partialShapes.range());

        Vector2 p0 = space.points.pos[span.start];
        Vector2 p1 = space.points.pos[span.start + 1];
        Vector2 p2 = space.points.pos[span.start + 2];
        Vector2 p3 = space.points.pos[span.start + 3];

        float stiffness = 2.5f * mass;
        float damping = 28.9f * mass;

        space.springs.push(Spring(span.start, span.start + 1, Vector2::vec2distance(p0.x, p0.y, p1.x, p1.y), stiffness, damping, shapeIndex));
        space.springs.push(Spring(span.start + 1, span.start + 2, Vector2::vec2distance(p1.x, p1.y, p2.x, p2.y), stiffness, damping, shapeIndex));
        space.springs.push(Spring(span.start + 2, span.start + 3, Vector2::vec2distance(p2.x, p2.y, p3.x, p3.y), stiffness, damping, shapeIndex));
        space.springs.push(Spring(span.start + 3, span.start, Vector2::vec2distance(p3.x, p3.y, p0.x, p0.y), stiffness, damping, shapeIndex));
        space.springs.push(Spring(span.start, span.start + 2, Vector2::vec2distance(p0.x, p0.y, p2.x, p2.y), stiffness, damping, shapeIndex));
        space.springs.push(Spring(span.start + 1, span.start + 3, Vector2::vec2distance(p1.x, p1.y, p3.x, p3.y), stiffness, damping, shapeIndex));

        space.shapes.push(quad);

        return quad;
    }
    Shape createParallelogram(PhysicsSpace &space, float x, float y, float width, float height, float sideOffset, float mass)
    {
        space.points.push(x, y, mass);
        space.points.push(x + width, y, mass);
        space.points.push(x + width + sideOffset, y + height, mass);
        space.points.push(x + sideOffset, y + height, mass);

        Shape parallelogram = Shape(space.points.size() - 4, space.points.size());
        PhysicsShapeMatching::shapeMatchAlignInit(space.points.range(), parallelogram, space.partialShapes.range());
        space.shapes.push(parallelogram);
        return parallelogram;
    }

    Shape createTriangle(PhysicsSpace &space, bool isStatic, float x0, float y0, float x1, float y1, float x2, float y2, float mass)
    {
        PointMasses &points = isStatic ? space.staticPoints : space.points;
        points.push(x0, y0, mass);
        points.push(x1, y1, mass);
        points.push(x2, y2, mass);

        const Span span(points.size() - 3, points.size());

        Shape triangle = Shape(span.start, span.end);
        PhysicsShapeMatching::shapeMatchAlignInit(points.range(), triangle, space.partialShapes.range());
        if (isStatic)
        {
            space.staticShapes.push(triangle);
        }
        else
        {
            space.shapes.push(triangle);
        }

        return triangle;
    }

    Shape createLine(PhysicsSpace &space, float x0, float y0, float x1, float y1, float mass)
    {
        int shapeIndex = space.nextShapeIndex();
        space.points.push(x0, y0, mass);
        space.points.push(x1, y1, mass);

        const Span span(space.points.size() - 2, space.points.size());
        float lineLength = Vector2::vec2distance(x0, y0, x1, y1);
        space.springs.push(Spring(span.start, span.start + 1, lineLength, 1.5f, 28.9f, shapeIndex));
        Shape line = Shape(span.start, span.end);
        PhysicsShapeMatching::shapeMatchAlignInit(space.points.range(), line, space.partialShapes.range());
        space.shapes.push(line);
        return line;
    }
}
