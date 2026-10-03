#include "PhysicsSpace.h"
#include "../utils/Console.h"
#include "ShapeUtils.h"
#include "../utils/MinMax.h"
#include "stdio.h"
#include <math.h>

#define NUM_SHAPES 40
#define NUM_POINTS 1024

PhysicsSpace::PhysicsSpace() : gravityEnabled(true), collisionsEnabled(true), shapeMatchingEnabled(true), springsEnabled(true)
{
    shapes.reserve(NUM_SHAPES);
    points.reserve(NUM_POINTS);
    staticJoints.reserve(NUM_POINTS / 2);
    shapeJoints.reserve(NUM_SHAPES / 2);
    radialAccelerators.reserve(NUM_SHAPES / 2);
    wheelMotors.reserve(NUM_SHAPES / 2);
    mouseJoint.pointIndex = -1;
    mouseJoint.position = Vector2::zero();
    triangleIndices.reserve(NUM_POINTS * 3);
}

void PhysicsSpace::assign(PhysicsSpace &other)
{
    shapes.replace(other.shapes);
    points.replace(other.points);
    staticJoints.replace(other.staticJoints);
    shapeJoints.replace(other.shapeJoints);
    triangleIndices.replace(other.triangleIndices);
    radialAccelerators.replace(other.radialAccelerators);
    wheelMotors.replace(other.wheelMotors);
}

void PhysicsSpace::initFromEntries(const Array<ShapeEntry> &entries)
{
    for (int i = 0; i < entries.size(); i++)
    {
        const ShapeEntry &entry = entries[i];
        Shape shape = Shape(points.size(), points.size() + entry.points.size());
        shape.isStatic = entry.isStatic;
        shapes.push(shape);
        points.append(entry.points);
        int shapeId = shapes.size() - 1;
    }

    triangulate();
}

int PhysicsSpace::nextShapeIndex() const
{
    return shapes.size();
}

int PhysicsSpace::nextParentId() const
{
    int maxParentId = -1;

    for (int i = 0; i < shapes.size(); i++)
    {
        maxParentId = max(maxParentId, shapes[i].parentId);
    }

    return maxParentId + 1;
}

void PhysicsSpace::updateIndices()
{
    for (int i = 0; i < shapes.size(); i++)
    {
        Shape &shape = shapes[i];
        shape.index = i;
    }
}

void PhysicsSpace::clear()
{
    shapes.clear();
    points.clear();
    staticJoints.clear();
    triangleIndices.clear();
    mouseJoint.pointIndex = -1;
    gravityEnabled = true;
    shapeJoints.clear();
    radialAccelerators.clear();
    wheelMotors.clear();
}

void PhysicsSpace::removeShapeWithoutPoints(int shapeIndex)
{
    if (shapeIndex < 0 || shapeIndex >= shapes.size())
    {
        return;
    }

    Shape &shape = shapes[shapeIndex];
    shapes.remove(shapeIndex);

    for (int i = 0; i < radialAccelerators.size(); i++)
    {
        RadialAccelerator &radialAccelerator = radialAccelerators[i];

        if (radialAccelerator.shapeIndex == shapeIndex)
        {
            radialAccelerators.remove(i);
            i--;
        }
        else if (radialAccelerator.shapeIndex > shapeIndex)
        {

            radialAccelerator.shapeIndex--;
        }
    }

    for (int i = 0; i < wheelMotors.size(); i++)
    {
        WheelMotor &wheelMotor = wheelMotors[i];

        if (wheelMotor.shapeIndex == shapeIndex)
        {
            wheelMotors.remove(i);
            i--;
        }
        else if (wheelMotor.shapeIndex > shapeIndex)
        {
            wheelMotor.shapeIndex--;
        }
    }

    for (int i = 0; i < shapeJoints.size(); i++)
    {
        ShapeJoint &shapeJoint = shapeJoints[i];

        if (shapeJoint.shapeIndex1 == shapeIndex || shapeJoint.shapeIndex2 == shapeIndex)
        {
            shapeJoints.remove(i);
            i--;
        }
        else
        {
            if (shapeJoint.shapeIndex1 >= shapeIndex)
            {
                shapeJoint.shapeIndex1--;
            }

            if (shapeJoint.shapeIndex2 >= shapeIndex)
            {
                shapeJoint.shapeIndex2--;
            }
        }
    }

    triangulate();
}

void PhysicsSpace::removeShape(int shapeIndex)
{
    if (shapeIndex < 0 || shapeIndex >= shapes.size())
    {
        return;
    }

    Shape shapeToDelete = shapes[shapeIndex];

    int shapeStart = shapeToDelete.start;
    int shapeEnd = shapeToDelete.end;
    Console::log("Shape at start: %d end: %d", shapeStart, shapeEnd);

    if (shapeToDelete.parentId != -1)
    {
        for (int i = 0; i < shapes.size(); i++)
        {
            Shape s = shapes[i];
            if (s.parentId == shapeToDelete.parentId)
            {
                shapeStart = min(shapeStart, s.start);
                shapeEnd = max(shapeEnd, s.end);
                removeShapeWithoutPoints(i);

                i--;
            }
        }
    }
    else
    {
        removeShapeWithoutPoints(shapeIndex);
    }

    Console::log("Shape after itr: %d end: %d", shapeStart, shapeEnd);

    int shapeSize = shapeEnd - shapeStart;
    Span shapeSpan = Span(shapeStart, shapeEnd);
    Console::log("Removing points %d => %d", shapeStart, shapeEnd);
    points.mass.removeRange(shapeSpan);
    points.pos.removeRange(shapeSpan);
    points.velocity.removeRange(shapeSpan);
    points.shapeOriginalPos.removeRange(shapeSpan);

    for (int i = 0; i < staticJoints.size(); i++)
    {
        StaticJoint &joint = staticJoints[i];
        if (joint.pointIndex >= shapeSpan.start && joint.pointIndex < shapeSpan.end)
        {
            staticJoints.remove(i);
            i--;
        }
        else if (joint.pointIndex >= shapeSpan.end)
        {
            joint.pointIndex -= shapeSize;
        }
    }

    for (int i = 0; i < shapes.size(); i++)
    {
        Shape &s = shapes[i];
        if (s.start >= shapeStart)
        {
            s.start -= shapeSize;
            s.end -= shapeSize;
        }
    }

    for (int i = 0; i < shapeJoints.size(); i++)
    {
        ShapeJoint &joint = shapeJoints[i];

        for (int i = 0; i < 4; i++)
        {
            if (joint.shape1Points[i] >= shapeStart)
            {
                joint.shape1Points[i] -= shapeSize;
            }
        }

        for (int i = 0; i < 4; i++)
        {
            if (joint.shape2Points[i] >= shapeStart)
            {
                joint.shape2Points[i] -= shapeSize;
            }
        }
    }

    triangulate();
}

Shape PhysicsSpace::addShapeFromSpace(float x, float y, const PhysicsSpace &other, int resourceId)
{
    // Start by finding the shape with the corresponding resourceId
    int copyShapeIndex = -1;
    for (int shapeIndex = 0; shapeIndex < other.shapes.size(); shapeIndex++)
    {
        const Shape &shape = other.shapes[shapeIndex];
        Console::log("Checking shape with resourceId %d at index %d", shape.resourceId, shapeIndex);
        if (shape.resourceId == resourceId)
        {
            copyShapeIndex = shapeIndex;
            break;
        }
    }

    if (copyShapeIndex == -1)
    {
        Console::log("No shape found with resourceId %d", resourceId);
        return Shape();
    }

    // Now we can paste the shape from the other space
    return pasteShape(x, y, copyShapeIndex, other);
}

Shape PhysicsSpace::pasteShape(float x, float y, int copyIndex, const PhysicsSpace &sourceSpace)
{
    const Shape &copyShape = sourceSpace.shapes[copyIndex];

    int shapeSize = copyShape.end - copyShape.start;
    int newShapeStart = points.size();
    int newShapeEnd = newShapeStart + shapeSize;

    ShapeProperties averages = ShapeUtils::getShapeProperties(sourceSpace.points.range(), copyShape);
    Vector2 offset = Vector2(x, y) - averages.center;

    for (int i = copyShape.start; i < copyShape.end; i++)
    {
        points.mass.push(sourceSpace.points.mass[i]);
        points.pos.push(sourceSpace.points.pos[i] + offset);
        points.velocity.push(sourceSpace.points.velocity[i]);
        points.shapeOriginalPos.push(sourceSpace.points.shapeOriginalPos[i]);
    }

    Shape returnedShape = copyShape;

    if (copyShape.parentId != -1)
    {
        int newParentId = nextParentId();
        int oldShapesSize = sourceSpace.shapes.size();

        for (int i = 0; i < oldShapesSize; i++)
        {
            Shape sCopy = sourceSpace.shapes[i];

            if (sCopy.parentId != copyShape.parentId)
            {
                continue;
            }
            int subIndex = sCopy.index;
            sCopy.parentId = newParentId;
            sCopy.start = newShapeStart;
            sCopy.end = newShapeEnd;
            sCopy.index = shapes.size();
            shapes.push(sCopy);
            returnedShape = sCopy;
        }
    }
    else
    {
        Shape sCopy = copyShape;
        sCopy.start = newShapeStart;
        sCopy.end = newShapeEnd;
        sCopy.index = shapes.size();
        shapes.push(sCopy);
        returnedShape = sCopy;
    }

    triangulate();
    return returnedShape;
}

int PhysicsSpace::closestPointIndex(float x, float y, int shapeIndex) const
{
    if (shapeIndex == -1)
    {
        return -1;
    }

    const Shape &shape = shapes[shapeIndex];
    float minDistance = __FLT_MAX__;
    int closestIndex = -1;

    ShapeIndexedRange range(shape);

    for (int i = 0; i < range.size(); i++)
    {
        Vector2 pos = points.pos[range[i]];
        float distance = Vector2::vec2distance(x, y, pos.x, pos.y);

        if (distance < minDistance)
        {
            minDistance = distance;
            closestIndex = range[i];
        }
    }

    return closestIndex;
}

// Compute signed area of triangle (a→b→c):
//   >0 means a→b→c is CCW (left turn)
//   <0 means CW  (right turn)
float signedArea(const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// Check if point p is inside triangle abc (including on edge)
bool pointInTriangle(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &p)
{
    float s1 = signedArea(p, a, b);
    float s2 = signedArea(p, b, c);
    float s3 = signedArea(p, c, a);
    bool hasNeg = (s1 < 0) || (s2 < 0) || (s3 < 0);
    bool hasPos = (s1 > 0) || (s2 > 0) || (s3 > 0);
    return !(hasNeg && hasPos);
}
float calculatePolygonSignedArea(const Array<int> &indices, const Array<Vector2> &positions)
{
    float area = 0.0f;
    int size = indices.size();

    for (int i = 0; i < size; i++)
    {
        int j = (i + 1) % size;
        const Vector2 &p1 = positions[indices[i]];
        const Vector2 &p2 = positions[indices[j]];
        area += (p1.x * p2.y - p2.x * p1.y);
    }

    return area / 2.0f;
}

// Reverse the winding order of the indices
void reverseWindingOrder(Array<int> &indices)
{
    int size = indices.size();
    for (int i = 0; i < size / 2; i++)
    {
        swap(indices[i], indices[size - 1 - i]);
    }
}

// The collision code assumes perp(edge) points into the shape, which holds
// only for one winding direction. Derive it from the rest pose rather than the
// current one: a soft body that has momentarily folded through itself has a
// reversed current winding, and following that would flip the contact
// depenetration direction back and forth between steps.
static float restPoseWindingSign(const PointMasses &points, const Shape &shape)
{
    ShapeIndexedRange range(shape);
    int count = range.size();

    float doubleArea = 0.0f;
    float sumSquaredEdgeLengths = 0.0f;
    for (int i = 0; i < count; i++)
    {
        const Vector2 &a = points.shapeOriginalPos[range[i]];
        const Vector2 &b = points.shapeOriginalPos[range[(i + 1) % count]];
        doubleArea += a.x * b.y - b.x * a.y;
        sumSquaredEdgeLengths += (b - a).lengthSquared();
    }

    // Compare against the shape's own scale, not an absolute epsilon: a
    // collapsed sliver can span tens of units and still enclose no area, and
    // picking a winding out of that much numerical noise is meaningless.
    if (fabsf(doubleArea) < 0.0001f * sumSquaredEdgeLengths)
    {
        return shape.windingSign;
    }

    return doubleArea > 0.0f ? 1.0f : -1.0f;
}

void PhysicsSpace::triangulate()
{
    triangleIndices.clear();

    static Array<int> curIndices;

    for (int shapeIdx = 0; shapeIdx < shapes.size(); shapeIdx++) // Changed i to shapeIdx
    {
        Shape &shape = shapes[shapeIdx];

        shape.triangleStart = triangleIndices.size();
        shape.windingSign = restPoseWindingSign(points, shape);

        ShapeIndexedRange range(shape);
        curIndices.clear();

        for (int j = 0; j < range.size(); j++)
        {
            curIndices.push(range[j]);
        }

        // Clip ears until only one triangle remains
        while (curIndices.size() > 3)
        {
            bool clipped = false;
            int m = curIndices.size();

            for (int i = 0; i < m; i++)
            {
                int iPrev = (i + m - 1) % m;
                int iNext = (i + 1) % m;

                int idxA = curIndices[iPrev];
                int idxB = curIndices[i];
                int idxC = curIndices[iNext];

                const Vector2 &A = points.shapeOriginalPos[idxA];
                const Vector2 &B = points.shapeOriginalPos[idxB];
                const Vector2 &C = points.shapeOriginalPos[idxC];

                // For CW polygons, we require a right turn (signedArea > 0)
                float signedAreaValue = signedArea(A, B, C);
                if (signedAreaValue < 0)
                {
                    continue;
                }

                // Check no other vertex lies inside triangle ABC
                bool anyInside = false;
                for (int j = 0; j < m; ++j)
                {
                    if (j == iPrev || j == i || j == iNext)
                        continue;
                    if (pointInTriangle(A, B, C, points.shapeOriginalPos[curIndices[j]]))
                    {
                        anyInside = true;
                        break;
                    }
                }
                if (anyInside)
                    continue;

                // Found an ear: record its indices and remove B
                triangleIndices.push(idxA);
                triangleIndices.push(idxB);
                triangleIndices.push(idxC);
                curIndices.remove(i);
                clipped = true;
                break;
            }

            // If no ear was clipped, polygon is likely degenerate
            if (!clipped)
            {
                Console::log("Degenerate polygon detected, triangulation failed.");
                break;
            }
        }

        // Add the final triangle (moved outside of the while loop)
        if (curIndices.size() == 3)
        {
            triangleIndices.push(curIndices[0]);
            triangleIndices.push(curIndices[1]);
            triangleIndices.push(curIndices[2]);
        }

        shape.triangleEnd = triangleIndices.size();
    }
}

void PhysicsSpace::addShape(const Shape &shape)
{
    Shape copy = shape;
    copy.index = shapes.size();
    shapes.push(copy);
    triangulate();
}
