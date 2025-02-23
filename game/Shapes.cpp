#include "Shapes.h"
#include "../containers/Array.h"

#include "../physics/PhysicsSpace.h"
#include "../physics/ShapeUtils.h"
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

    void recalculateOriginalPos(PhysicsSpace &space, const Shape &shape)
    {
        Vector2 averageCenter = ShapeUtils::getShapeProperties(space.points.range(), shape).center;

        for (int i = shape.start; i < shape.end; i++)
        {
            space.points.shapeOriginalPos[i] = space.points.pos[i] - averageCenter;
        }
    }

    void snapToGrid(PhysicsSpace &space, int shapeIndex)
    {
        Shape &shape = space.shapes[shapeIndex];

        for (int i = shape.start; i < shape.end; i++)
        {
            Vector2 &pos = space.points.pos[i];

            // Round to nearest multiple of 50
            pos.x = round(pos.x / 50.0f) * 50.0f;
            pos.y = round(pos.y / 50.0f) * 50.0f;
        }

        recalculateOriginalPos(space, shape);

        for (int i = 0; i < space.springs.size(); i++)
        {
            Spring &spring = space.springs[i];
            Shape &springShape = space.shapes[spring.shapeIndex];

            if (spring.shapeIndex == shapeIndex || (springShape.parentId != -1 && springShape.parentId == shape.parentId))
            {
                Vector2 p0 = space.points.pos[spring.pointA];
                Vector2 p1 = space.points.pos[spring.pointB];
                float length = Vector2::vec2distance(p0.x, p0.y, p1.x, p1.y);
                spring.length = length;
            }
        }
    }

    void addPointToShape(PhysicsSpace &space, int shapeIndex, float x, float y)
    {
        Shape &shape = space.shapes[shapeIndex];

        if (shape.hasIndices() || shape.parentId != -1)
        {
            Console::log("Unable to add points to subshape");
            return;
        }

        float mass = space.points.mass[shape.start];

        float minDistance = __FLT_MAX__;
        int closestIndex = -1;
        Vector2 minClosestPoint;
        for (int i = shape.start; i < shape.end; i++)
        {
            int iNext = i + 1;

            if (iNext == shape.end)
            {
                iNext = shape.start;
            }

            Vector2 p0 = space.points.pos[i];
            Vector2 p1 = space.points.pos[iNext];
            Vector2 closestPoint = closestPointToLineSegment(p0, p1, Vector2(x, y));
            float distance = Vector2::vec2distance(x, y, closestPoint.x, closestPoint.y);

            if (distance < minDistance)
            {
                minDistance = distance;
                closestIndex = i + 1;
                minClosestPoint = closestPoint;
            }
        }

        int newPointIndex = closestIndex;
        int prevIndex = newPointIndex - 1;

        Vector2 prevPoint = space.points.pos[prevIndex];
        Vector2 delta = Vector2(x, y) - prevPoint;
        Vector2 origPos = space.points.shapeOriginalPos[prevIndex] + delta;

        space.points.insert(newPointIndex, x, y, mass, origPos.x, origPos.y);
        shape.end++;

        handlePointInserted(space, shape.start, newPointIndex);
    }

    void handlePointInserted(PhysicsSpace &space, int shapeStart, int newPointIndex)
    {
        for (int i = 0; i < space.shapes.size(); i++)
        {
            Shape &otherShape(space.shapes[i]);

            if (otherShape.start > shapeStart)
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

    void addSubshapeToShape(PhysicsSpace &space, const Vector2 &pos, const Range<Vector2> &pointsRange)
    {
        if (pointsRange.size > 4)
        {
            Console::log("Subshape must have at most 4 points");
            return;
        }

        int matchingVertices[4] = {-1, -1, -1, -1};
        int matchingShapeIndices[4] = {-1, -1, -1, -1};

        for (int i = 0; i < pointsRange.size; i++)
        {
            Vector2 p0 = pointsRange[i];
            Vector2 p0Translated = p0 + pos;
            for (int s = 0; s < space.shapes.size(); s++)
            {
                Shape &shape = space.shapes[s];

                if (!shape.hasIndices())
                {
                    continue;
                }

                for (int j = 0; j < 4; j++)
                {
                    if (shape.indices[j] == UINT16_MAX)
                    {
                        break;
                    }

                    Vector2 p1 = space.points.pos[shape.indices[j] + shape.start];
                    float dist = p0Translated.distance(p1);

                    if (dist < minPointSnapDist)
                    {
                        matchingVertices[i] = shape.indices[j] + shape.start;
                        matchingShapeIndices[i] = shape.index;
                        break;
                    }
                }
            }
        }

        int numMatchingVertices = 0;
        int referenceShapeIndex = -1;
        int curShapeParentId = -1;
        Vector2 referencePoint;
        int referenceIndex = -1;
        float referenceMass = 0.0f;

        for (int i = 0; i < 4; i++)
        {
            if (matchingVertices[i] != -1)
            {
                numMatchingVertices++;

                Shape &shape = space.shapes[matchingShapeIndices[i]];

                if (curShapeParentId != -1 && curShapeParentId != shape.parentId)
                {
                    Console::log("Vertices must belong to the same shape");
                    return;
                }

                curShapeParentId = shape.parentId;
                referenceShapeIndex = shape.index;
                referencePoint = space.points.pos[matchingVertices[i]];
                referenceMass = space.points.mass[matchingVertices[i]];
                referenceIndex = matchingVertices[i];
            }
        }

        if (numMatchingVertices == 1)
        {
            Console::log("At least 2 vertices must match");
            return;
        }
        else if (numMatchingVertices == 0)
        {
            Shape newShape = Shape(space.points.size(), space.points.size() + pointsRange.size);
            newShape.parentId = space.nextParentId();
            for (int i = 0; i < pointsRange.size; i++)
            {
                Vector2 p0 = pointsRange[i] + pos;
                space.points.push(Vertex{.pos = p0});
                newShape.indices[i] = (uint16_t)i;
            }

            newShape.selfIntersecting = true;
            space.shapes.push(newShape);
            recalculateOriginalPos(space, newShape);

            return;
        }

        for (int s = 0; s < space.shapes.size(); s++)
        {
            Shape &otherShape = space.shapes[s];

            if (otherShape.parentId != curShapeParentId)
            {
                continue;
            }

            bool connectedPointsOtherShape[4] = {false, false, false, false};
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (matchingVertices[j] != -1 && otherShape.indices[i] == matchingVertices[j] - otherShape.start)
                    {
                        connectedPointsOtherShape[i] = true;
                        break;
                    }
                }
            }

            for (int i = 0; i < 4; i++)
            {
                if (connectedPointsOtherShape[i] && connectedPointsOtherShape[(i + 1) % 4])
                {
                    otherShape.interiorEdges[i] = true;
                }
            }
        }

        Shape &referenceShape = space.shapes[referenceShapeIndex];

        Shape subShape;
        subShape.start = referenceShape.start;
        subShape.end = referenceShape.end;
        subShape.parentId = referenceShape.parentId;
        subShape.index = space.shapes.size();
        subShape.selfIntersecting = true;

        for (int i = 0; i < pointsRange.size; i++)
        {
            if (matchingVertices[i] != -1)
            {
                subShape.indices[i] = (uint16_t)(matchingVertices[i] - referenceShape.start);

                int nextIndex = (i + 1) % 4;
                if (matchingVertices[nextIndex] != -1)
                {
                    subShape.interiorEdges[i] = true;
                }
            }
            else
            {
                int newPointIndex = referenceShape.end;
                Vector2 p0 = pointsRange[i];
                Vector2 origPos = space.points.shapeOriginalPos[referenceIndex] + (p0 - referencePoint);

                space.points.insert(newPointIndex,
                                    Vertex{.pos = p0 + pos,
                                           .mass = referenceMass,
                                           .originalPos = origPos});
                subShape.end++;

                for (int j = 0; j < space.shapes.size(); j++)
                {
                    Shape &otherShape = space.shapes[j];
                    if (otherShape.parentId == referenceShape.parentId)
                    {
                        otherShape.end++;
                    }
                }

                subShape.indices[i] = (uint16_t)(newPointIndex - referenceShape.start);
                handlePointInserted(space, referenceShape.start, newPointIndex);
            }
        }

        recalculateOriginalPos(space, referenceShape);

        space.shapes.push(subShape);
        Console::log("Num matching vertices: %d", numMatchingVertices);
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

    int createMesh(PhysicsSpace &space, float x, float y, float meshWidth, float meshHeight, int numSegments, float mass, float stiffnessFactor)
    {
        int startIndex = space.points.size();
        int curIndex = startIndex;
        float curX = x;
        float curY = y;

        float width = meshWidth / numSegments;
        float height = meshHeight / numSegments;

        float diagonal = sqrt(width * width + height * height);
        float stiffness = 4.5f;
        float damping = 1280.9f;
        Console::log("Mesh size: %f %f %d", meshWidth, meshHeight, numSegments);
        Console::log("Width: %f", width);
        Console::log("Height: %f", height);
        for (int row = 0; row < numSegments; row++)
        {
            for (int i = 0; i < numSegments; i++)
            {
                space.points.push(curX, curY, mass);

                curX += width;
                curIndex++;
            }

            curX = x;
            curY += height;
        }

        int meshStart = startIndex;
        int meshEnd = curIndex;
        int parentId = space.nextParentId();
        int subShapeIndex = space.nextShapeIndex();
        int rowOffset = 0;
        for (int row = 0; row < numSegments - 1; row++)
        {
            for (int i = 0; i < numSegments; i++)
            {

                int topIndex = rowOffset + startIndex + i;
                int bottomIndex = topIndex + numSegments;

                space.springs.push(Spring(topIndex, bottomIndex, height, stiffness, damping, subShapeIndex));

                if (i > 0)
                {
                    Vector2 topLeft = space.points.pos[topIndex - 1];
                    Vector2 topRight = space.points.pos[topIndex];
                    Vector2 bottomLeft = space.points.pos[bottomIndex - 1];
                    Vector2 bottomRight = space.points.pos[bottomIndex];

                    Shape subMesh(meshStart, meshEnd);
                    subMesh.parentId = parentId;
                    subMesh.indices[0] = (uint16_t)(topIndex - 1 - meshStart);
                    subMesh.indices[1] = (uint16_t)(topIndex - meshStart);
                    subMesh.indices[2] = (uint16_t)(bottomIndex - meshStart);
                    subMesh.indices[3] = (uint16_t)(bottomIndex - 1 - meshStart);

                    subMesh.interiorEdges[0] = row > 0;
                    subMesh.interiorEdges[1] = i < numSegments - 1;
                    subMesh.interiorEdges[2] = row < numSegments - 2;
                    subMesh.interiorEdges[3] = i > 1;

                    // Top bar
                    space.springs.push(Spring(topIndex - 1, topIndex, width, stiffness, damping, subShapeIndex));
                    // Bottom bar
                    space.springs.push(Spring(bottomIndex - 1, bottomIndex, width, stiffness, damping, subShapeIndex));

                    // // Diagonal 1
                    space.springs.push(Spring(topIndex - 1, bottomIndex, diagonal, stiffness, damping, subShapeIndex));
                    // // Diagonal 2
                    space.springs.push(Spring(bottomIndex - 1, topIndex, diagonal, stiffness, damping, subShapeIndex));

                    space.shapes.push(subMesh);

                    subShapeIndex = space.nextShapeIndex();
                }
            }

            rowOffset += numSegments;
        }
        curX -= width;
        return parentId;
    }

    Shape createCircle(PhysicsSpace &space, float x, float y, float radius, float mass, float stiffnessFactor)
    {
        int shapeIndex = space.nextShapeIndex();
        float stiffness = mass * stiffnessFactor;
        float damping = 545.0f * mass;

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
        circle.index = shapeIndex;
        return circle;
    }

    int createBridge(PhysicsSpace &space, float x, float y, float mass, int numSegments)
    {
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

        int bridgeStart = startIndex;
        int bridgeEnd = curIndex;
        int parentId = space.nextParentId();
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

                Shape subBridge(bridgeStart, bridgeEnd);
                subBridge.parentId = parentId;
                subBridge.selfIntersecting = true;
                subBridge.indices[0] = (uint16_t)(topIndex - 1 - bridgeStart);
                subBridge.indices[1] = (uint16_t)(topIndex - bridgeStart);
                subBridge.indices[2] = (uint16_t)(bottomIndex - bridgeStart);
                subBridge.indices[3] = (uint16_t)(bottomIndex + 1 - bridgeStart);

                subBridge.interiorEdges[1] = i < numSegments - 1;
                subBridge.interiorEdges[3] = i > 1;

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
        return parentId;
    }

    Shape createStaticQuad(PhysicsSpace &space, float x, float y, float width, float height, float mass)
    {
        int shapeIndex = space.nextShapeIndex();
        space.points.push(x, y, mass);
        space.points.push(x + width, y, mass);
        space.points.push(x + width, y + height, mass);
        space.points.push(x, y + height, mass);

        const Span span(space.points.size() - 4, space.points.size());
        Shape quad = Shape(span.start, span.end);
        quad.isStatic = true;
        space.shapes.push(quad);
        quad.index = shapeIndex;

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
        quad.index = shapeIndex;
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
        quad.index = shapeIndex;
        return quad;
    }
    Shape createParallelogram(PhysicsSpace &space, float x, float y, float width, float height, float sideOffset, float mass)
    {
        int shapeIndex = space.shapes.size();
        space.points.push(x, y, mass);
        space.points.push(x + width, y, mass);
        space.points.push(x + width + sideOffset, y + height, mass);
        space.points.push(x + sideOffset, y + height, mass);

        Shape parallelogram = Shape(space.points.size() - 4, space.points.size());
        space.shapes.push(parallelogram);
        parallelogram.index = shapeIndex;
        return parallelogram;
    }

    Shape createTriangle(PhysicsSpace &space, bool isStatic, float x0, float y0, float x1, float y1, float x2, float y2, float mass)
    {
        int shapeIndex = space.nextShapeIndex();
        space.points.push(x0, y0, mass);
        space.points.push(x1, y1, mass);
        space.points.push(x2, y2, mass);

        const Span span(space.points.size() - 3, space.points.size());

        Shape triangle = Shape(span.start, span.end);
        triangle.isStatic = isStatic;
        triangle.isStatic = isStatic;
        space.shapes.push(triangle);
        triangle.index = shapeIndex;
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
        line.index = shapeIndex;
        return line;
    }

    void createCar(PhysicsSpace &space, float x, float y)
    {
        Shape wheel1 = createCircle(space, x + 20.0f, y + 20.0f, 25.0f, 0.25f);
        Shape wheel2 = createCircle(space, x + 100.0f, y + 20.0f, 25.0f, 0.25f);
        Shape upperBody = createQuad(space, x, y - 42.0f, 120.0f, 30.0f, 1.0f);

        ShapeJoint wheel1Joint;

        wheel1Joint.shapeIndex1 = wheel1.index;
        wheel1Joint.shapeIndex2 = upperBody.index;
        int wheelSize = wheel1.end - wheel1.start;
        wheel1Joint.shape1Points[0] = wheel1.start;
        wheel1Joint.shape1Points[1] = wheel1.start + wheelSize / 4;
        wheel1Joint.shape1Points[2] = wheel1.start + wheelSize / 2;
        wheel1Joint.shape1Points[3] = wheel1.start + wheelSize * 3 / 4;

        wheel1Joint.shape2Points[0] = upperBody.start;
        wheel1Joint.shape2Points[1] = upperBody.start + 1;
        wheel1Joint.shape2Points[2] = upperBody.start + 2;
        wheel1Joint.shape2Points[3] = upperBody.start + 3;
        wheel1Joint.offset = Vector2(0.0f, 50.0f);
        space.shapeJoints.push(wheel1Joint);
    }
}
