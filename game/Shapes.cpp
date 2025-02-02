#include "Shapes.h"
#include "../containers/Array.h"
#include "../physics/PhysicsSpace.h"
#include "../utils/Console.h"
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
    Shape createLooseCircle(PhysicsSpace &space, float x, float y, float radius, float mass, float stiffnessFactor)
    {
        int shapeIndex = space.nextShapeIndex();
        float stiffness = mass * stiffnessFactor;
        float damping = 28.9f * mass;
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
        Vector2 pNext = space.points.pos[startIndex + 1];

        float lengthSpring1 = Vector2::vec2distance(p0.x, p0.y, p1.x, p1.y);
        float lengthSpring2 = Vector2::vec2distance(p0.x, p0.y, p2.x, p2.y);
        float lengthSpringNext = Vector2::vec2distance(p0.x, p0.y, pNext.x, pNext.y);

        for (int i = 0; i < numSegments; i++)
        {
            int n0 = startIndex + i;

            int nNext = startIndex + wrapIndex(i + 1, numSegments);

            space.springs.push(Spring(n0, nNext, lengthSpringNext, stiffness, damping, shapeIndex));
        }

        Shape circle = Shape(startIndex, curIndex, 300.0f);
        space.shapes.push(circle);
        return circle;
    }

    void addPointToShape(PhysicsSpace &space, int shapeIndex, float x, float y)
    {
        Shape &shape = space.shapes[shapeIndex];

        float mass = space.points.mass[shape.start];

        float minDistance = __FLT_MAX__;
        int closestIndex = -1;
        Vector2 minClosestPoint;
        for (int i = shape.start; i < shape.end; i++)
        {
            int iNext = i + 1;

            if (iNext == shape.end)
            {
                iNext = 0;
            }

            Vector2 p0 = space.points.pos[i];
            Vector2 p1 = space.points.pos[iNext];
            Vector2 closestPoint = closestPointToLineSegment(p0, p1, Vector2(x, y));
            float distance = Vector2::vec2distance(x, y, closestPoint.x, closestPoint.y);

            Console::drawPoint(Vector2(x, y), 0xffff0000);

            if (distance < minDistance)
            {
                minDistance = distance;
                closestIndex = i + 1;
                minClosestPoint = closestPoint;
            }
        }

        Console::drawPoint(minClosestPoint, 0xffff00ff);
        Console::log("Closest index: %d", closestIndex);
        Console::log("Shape start: %d", shape.start);

        int newPointIndex = closestIndex;

        space.points.insert(newPointIndex, x, y, mass);
        shape.end++;

        for (int i = 0; i < space.shapes.size(); i++)
        {
            Shape &otherShape(space.shapes[i]);

            if (otherShape.start > shape.start)
            {
                otherShape.start++;
                otherShape.end++;
            }
        }

        for (int i = 0; i < space.springs.size(); i++)
        {
            Spring &spring = space.springs[i];

            if (spring.pointA >= newPointIndex)
            {
                spring.pointA++;
            }

            if (spring.pointB >= newPointIndex)
            {
                spring.pointB++;
            }
        }

        for (int i = 0; i < space.staticJoints.size(); i++)
        {
            StaticJoint &joint = space.staticJoints[i];

            if (joint.pointIndex >= newPointIndex)
            {
                joint.pointIndex++;
            }
        }
    }

    void resetShape(PhysicsSpace &space, int shapeIndex)
    {
        // TODO: Reimplement
        // Shape &shape = space.shapes[shapeIndex];

        // for (int i = shape.start; i < shape.end; i++)
        // {
        //     space.points.pos[i] = space.points.shapePos[i];
        //     space.points.velocity[i] = Vector2();
        // }
    }

    Shape createMesh(PhysicsSpace &space, float x, float y, float width, float height, int numSegments, float mass, float stiffnessFactor)
    {
        int shapeIndex = space.nextShapeIndex();
        int startIndex = space.points.size();
        int curIndex = startIndex;
        float curX = x;
        float diagonal = sqrt(width * width + height * height);
        float stiffness = 4.5f;
        float damping = 180.9f;

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

        Shape bridge = Shape(startIndex, curIndex);

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

                // ShapeQuad quad = {{topIndex - 1, topIndex, bottomIndex, bottomIndex + 1}, {topLeft, topRight, bottomLeft, bottomRight}, {topLeft, topRight, bottomLeft, bottomRight}, 4};
                // space.partialShapes.push(quad);

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

        // bridge.subShapeSpan = {partialShapesStart, space.partialShapes.size()};
        bridge.disableShapeMatching = true;
        space.shapes.push(bridge);

        return bridge;
    }

    Shape createCircle(PhysicsSpace &space, float x, float y, float radius, float mass, float stiffnessFactor)
    {
        int shapeIndex = space.nextShapeIndex();
        float stiffness = mass * stiffnessFactor;
        float damping = 28.9f * mass;

        stiffness = 2.5f;
        damping = 1090.0f;
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
        Vector2 pNext = space.points.pos[startIndex + 1];

        float lengthSpring1 = Vector2::vec2distance(p0.x, p0.y, p1.x, p1.y);
        float lengthSpring2 = Vector2::vec2distance(p0.x, p0.y, p2.x, p2.y);
        float lengthSpringNext = Vector2::vec2distance(p0.x, p0.y, pNext.x, pNext.y);

        for (int i = 0; i < numSegments; i++)
        {
            int n0 = startIndex + i;
            int n1 = startIndex + wrapIndex(i + 3, numSegments);
            int n2 = startIndex + wrapIndex(i + 5, numSegments);

            int nNext = startIndex + wrapIndex(i + 1, numSegments);

            space.springs.push(Spring(n0, n1, lengthSpring1, stiffness, damping, shapeIndex));
            space.springs.push(Spring(n2, n0, lengthSpring2, stiffness, damping, shapeIndex));

            // space.springs.push(Spring(n0, nNext, lengthSpringNext, stiffness, damping, shapeIndex));
        }

        Shape circle = Shape(startIndex, curIndex, 300.0f);
        space.shapes.push(circle);
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
        float stiffness = 4.5f;
        float damping = 180.9f;

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
        Shape bridge = Shape(startIndex, curIndex);
        bridge.disableShapeMatching = true;
        space.shapes.push(bridge);
        int subShapeIndex = space.nextShapeIndex();

        for (int i = 0; i < numSegments; i++)
        {
            int topIndex = startIndex + i;
            int bottomIndex = startIndex + numSegments * 2 - i - 1;

            space.springs.push(Spring(topIndex, bottomIndex, height, stiffness, damping, subShapeIndex));

            if (i > 0)
            {
                Vector2 topLeft = space.points.pos[topIndex - 1];
                Vector2 topRight = space.points.pos[topIndex];
                Vector2 bottomLeft = space.points.pos[bottomIndex + 1];
                Vector2 bottomRight = space.points.pos[bottomIndex];

                Shape subBridge(startIndex, curIndex);
                subBridge.parentIndex = shapeIndex;
                subBridge.indices[0] = (uint16_t)(topIndex - 1 - bridge.start);
                subBridge.indices[1] = (uint16_t)(topIndex - bridge.start);
                subBridge.indices[2] = (uint16_t)(bottomIndex - bridge.start);
                subBridge.indices[3] = (uint16_t)(bottomIndex + 1 - bridge.start);

                // ShapeQuad quad = {{topIndex - 1, topIndex, bottomIndex, bottomIndex + 1}, {topLeft, topRight, bottomLeft, bottomRight}, {topLeft, topRight, bottomLeft, bottomRight}, 4};
                // space.partialShapes.push(quad);

                // Top bar
                space.springs.push(Spring(topIndex - 1, topIndex, width, stiffness, damping, subShapeIndex));
                // Bottom bar
                space.springs.push(Spring(bottomIndex + 1, bottomIndex, width, stiffness, damping, subShapeIndex));

                // Diagonal 1
                space.springs.push(Spring(topIndex, bottomIndex + 1, diagonal, stiffness, damping, subShapeIndex));
                // Diagonal 2
                space.springs.push(Spring(bottomIndex, topIndex - 1, diagonal, stiffness, damping, subShapeIndex));

                space.shapes.push(subBridge);
                subShapeIndex = space.nextShapeIndex();
            }
        }

        curX -= width;

        // bridge.subShapeSpan = {partialShapesStart, space.partialShapes.size()};

        return bridge;
    }

    Shape createStaticQuad(PhysicsSpace &space, float x, float y, float width, float height, float mass)
    {
        space.points.push(x, y, mass);
        space.points.push(x + width, y, mass);
        space.points.push(x + width, y + height, mass);
        space.points.push(x, y + height, mass);

        const Span span(space.points.size() - 4, space.points.size());
        Shape quad = Shape(span.start, span.end);
        quad.isStatic = true;
        space.shapes.push(quad);

        return quad;
    }

    Shape createRoundedQuad(PhysicsSpace &space, float x, float y, float width, float height, float mass, float stiffnessFactor)
    {
        int shapeIndex = space.nextShapeIndex();

        float cornerSize = width / 8.0f;

        space.points.push(x, y + cornerSize, mass);
        space.points.push(x + cornerSize, y, mass);
        space.points.push(x + width - cornerSize, y, mass);
        space.points.push(x + width, y + cornerSize, mass);
        space.points.push(x + width, y + height - cornerSize, mass);
        space.points.push(x + width - cornerSize, y + height, mass);
        space.points.push(x + cornerSize, y + height, mass);
        space.points.push(x, y + height - cornerSize, mass);

        const Span span(space.points.size() - 8, space.points.size());
        Shape quad = Shape(span.start, span.end);

        Vector2 p0 = space.points.pos[span.start];
        Vector2 p1 = space.points.pos[span.start + 1];

        Vector2 p2 = space.points.pos[span.start + 2];
        Vector2 p3 = space.points.pos[span.start + 3];

        Vector2 p4 = space.points.pos[span.start + 4];
        Vector2 p5 = space.points.pos[span.start + 5];

        Vector2 p6 = space.points.pos[span.start + 6];
        Vector2 p7 = space.points.pos[span.start + 7];

        float stiffness = stiffnessFactor * mass;
        float damping = 28.9f * mass;

        for (int corner = 0; corner < 4; corner++)
        {
            int indexP0 = span.start + corner * 2;
            int indexP1 = span.start + corner * 2 + 1;
            int indexP2 = span.start + ((corner * 2 + 2) % 8);
            int indexP3 = span.start + ((corner * 2 + 3) % 8);

            Vector2 p0 = space.points.pos[indexP0];
            Vector2 p1 = space.points.pos[indexP1];
            Vector2 p2 = space.points.pos[indexP2];
            Vector2 p3 = space.points.pos[indexP3];

            space.springs.push(Spring(indexP0, indexP1, Vector2::vec2distance(p0.x, p0.y, p1.x, p1.y), stiffness, damping, shapeIndex));
            space.springs.push(Spring(indexP1, indexP2, Vector2::vec2distance(p1.x, p1.y, p2.x, p2.y), stiffness, damping, shapeIndex));
            space.springs.push(Spring(indexP2, indexP3, Vector2::vec2distance(p2.x, p2.y, p3.x, p3.y), stiffness, damping, shapeIndex));
            space.springs.push(Spring(indexP0, indexP3, Vector2::vec2distance(p0.x, p0.y, p3.x, p3.y), stiffness, damping, shapeIndex));

            space.springs.push(Spring(indexP1, indexP3, Vector2::vec2distance(p1.x, p1.y, p3.x, p3.y), stiffness, damping, shapeIndex));

            space.springs.push(Spring(indexP0, indexP2, Vector2::vec2distance(p0.x, p0.y, p2.x, p2.y), stiffness, damping, shapeIndex));
        }

        for (int side = 0; side < 2; side++)
        {
            int indexP0 = span.start + side * 2;
            int indexP1 = span.start + side * 2 + 1;

            int indexP2 = span.start + ((side * 2 + 4) % 8);
            int indexP3 = span.start + ((side * 2 + 5) % 8);

            Vector2 p0 = space.points.pos[indexP0];
            Vector2 p1 = space.points.pos[indexP1];
            Vector2 p2 = space.points.pos[indexP2];
            Vector2 p3 = space.points.pos[indexP3];
            space.springs.push(Spring(indexP0, indexP3, Vector2::vec2distance(p0.x, p0.y, p3.x, p3.y), stiffness, damping, shapeIndex));
            space.springs.push(Spring(indexP1, indexP2, Vector2::vec2distance(p1.x, p1.y, p2.x, p2.y), stiffness, damping, shapeIndex));
        }

        // space.springs.push(Spring(span.start, span.start + 1, Vector2::vec2distance(p0.x, p0.y, p1.x, p1.y), stiffness, damping, shapeIndex));
        // space.springs.push(Spring(span.start + 1, span.start + 2, Vector2::vec2distance(p1.x, p1.y, p2.x, p2.y), stiffness, damping, shapeIndex));
        // space.springs.push(Spring(span.start + 2, span.start + 3, Vector2::vec2distance(p2.x, p2.y, p3.x, p3.y), stiffness, damping, shapeIndex));
        // space.springs.push(Spring(span.start + 3, span.start, Vector2::vec2distance(p3.x, p3.y, p0.x, p0.y), stiffness, damping, shapeIndex));
        // space.springs.push(Spring(span.start, span.start + 2, Vector2::vec2distance(p0.x, p0.y, p2.x, p2.y), stiffness, damping, shapeIndex));
        // space.springs.push(Spring(span.start + 1, span.start + 3, Vector2::vec2distance(p1.x, p1.y, p3.x, p3.y), stiffness, damping, shapeIndex));

        space.shapes.push(quad);

        return quad;
    }

    Shape createQuad(PhysicsSpace &space, float x, float y, float width, float height, float mass, float stiffnessFactor)
    {
        int shapeIndex = space.nextShapeIndex();
        space.points.push(x, y, mass);
        space.points.push(x + width, y, mass);
        space.points.push(x + width, y + height, mass);
        space.points.push(x, y + height, mass);

        const Span span(space.points.size() - 4, space.points.size());
        Shape quad = Shape(span.start, span.end);

        Vector2 p0 = space.points.pos[span.start];
        Vector2 p1 = space.points.pos[span.start + 1];
        Vector2 p2 = space.points.pos[span.start + 2];
        Vector2 p3 = space.points.pos[span.start + 3];

        float stiffness = stiffnessFactor * mass;
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
        space.shapes.push(parallelogram);
        return parallelogram;
    }

    Shape createTriangle(PhysicsSpace &space, bool isStatic, float x0, float y0, float x1, float y1, float x2, float y2, float mass)
    {
        space.points.push(x0, y0, mass);
        space.points.push(x1, y1, mass);
        space.points.push(x2, y2, mass);

        const Span span(space.points.size() - 3, space.points.size());

        Shape triangle = Shape(span.start, span.end);
        triangle.isStatic = isStatic;
        triangle.isStatic = isStatic;
        space.shapes.push(triangle);

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
        space.shapes.push(line);
        return line;
    }
}
