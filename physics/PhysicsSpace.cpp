#include "PhysicsSpace.h"

#define NUM_SHAPES 40
#define NUM_POINTS 1024

PhysicsSpace::PhysicsSpace() : gravityEnabled(true), collisionsEnabled(true), shapeMatchingEnabled(true), springsEnabled(true), draggingShapeIndex(-1), draggingSubShapeIndex(-1)
{
    shapes.reserve(NUM_SHAPES);
    partialShapes.reserve(NUM_SHAPES);
    points.reserve(NUM_POINTS);
    staticShapes.reserve(NUM_SHAPES);
    staticPoints.reserve(NUM_POINTS);
    staticJoints.reserve(NUM_POINTS / 2);
    springs.reserve(NUM_POINTS / 2);
    mouseJoint.pointIndex = -1;
    mouseJoint.position = Vector2::zero();
}

void PhysicsSpace::assign(PhysicsSpace &other)
{
    shapes.replace(other.shapes);
    partialShapes.replace(other.partialShapes);
    staticShapes.replace(other.staticShapes);
    points.replace(other.points);
    staticPoints.replace(other.staticPoints);
    springs.replace(other.springs);
    staticJoints.replace(other.staticJoints);
}

int PhysicsSpace::nextShapeIndex() const
{
    return shapes.size();
}

int PhysicsSpace::nextStaticShapeIndex() const
{
    return staticShapes.size();
}

void PhysicsSpace::clear()
{
    shapes.clear();
    partialShapes.clear();
    points.clear();
    staticShapes.clear();
    staticPoints.clear();
    springs.clear();
    staticJoints.clear();
    mouseJoint.pointIndex = -1;
    gravityEnabled = true;
    shapeMatchingEnabled = true;
}
