#include "PhysicsSpace.h"
#include "../utils/Console.h"

#define NUM_SHAPES 40
#define NUM_POINTS 1024

PhysicsSpace::PhysicsSpace() : gravityEnabled(true), collisionsEnabled(true), shapeMatchingEnabled(true), springsEnabled(true), draggingShapeIndex(-1), draggingSubShapeIndex(-1)
{
    shapes.reserve(NUM_SHAPES);
    partialShapes.reserve(NUM_SHAPES);
    points.reserve(NUM_POINTS);
    staticJoints.reserve(NUM_POINTS / 2);
    springs.reserve(NUM_POINTS / 2);
    mouseJoint.pointIndex = -1;
    mouseJoint.position = Vector2::zero();
}

void PhysicsSpace::assign(PhysicsSpace &other)
{
    shapes.replace(other.shapes);
    partialShapes.replace(other.partialShapes);
    points.replace(other.points);
    springs.replace(other.springs);
    staticJoints.replace(other.staticJoints);
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

        for (int j = 0; j < entry.springs.size(); j++)
        {
            Spring spring = entry.springs[j];
            spring.shapeIndex = shapeId;
            springs.push(spring);
        }
    }
}

int PhysicsSpace::nextShapeIndex() const
{
    return shapes.size();
}

void PhysicsSpace::clear()
{
    shapes.clear();
    partialShapes.clear();
    points.clear();
    springs.clear();
    staticJoints.clear();
    mouseJoint.pointIndex = -1;
    gravityEnabled = true;
    shapeMatchingEnabled = false;
}

void PhysicsSpace::removeShape(int shapeIndex)
{
    if (shapeIndex < 0 || shapeIndex >= shapes.size())
    {
        return;
    }

    Shape &shape = shapes[shapeIndex];

    points.mass.removeRange(shape);
    points.pos.removeRange(shape);
    points.velocity.removeRange(shape);
    points.shapePos.removeRange(shape);
    points.shapeOriginalPos.removeRange(shape);

    int shapeSize = shape.end - shape.start;
    shapes.remove(shapeIndex);

    for (int i = 0; i < shapes.size(); i++)
    {
        Shape &s = shapes[i];
        if (i >= shapeIndex)
        {
            s.start -= shapeSize;
            s.end -= shapeSize;
        }
    }

    for (int i = 0; i < springs.size(); i++)
    {
        Spring &spring = springs[i];

        if (spring.shapeIndex == shapeIndex)
        {
            springs.remove(i);
            i--;
        }
        else if (spring.shapeIndex >= shapeIndex)
        {
            spring.shapeIndex--;
            spring.pointA -= shapeSize;
            spring.pointB -= shapeSize;
        }
    }
}
