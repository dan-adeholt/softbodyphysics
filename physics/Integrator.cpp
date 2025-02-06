#include "Integrator.h"
#include "PhysicsSpace.h"
#include "../utils/Console.h"
#include "./Springs.h"
#include "ShapeUtils.h"
struct RK4Integrator::Impl
{
    Array<PointDerivative> rk1;
    Array<PointDerivative> rk2;
    Array<PointDerivative> rk3;
    Array<PointDerivative> rk4;
    PointMasses rkTemp;
    Array<PointDerivative> rkEmptyDerivatives;
    Array<ShapeProperties> shapeProperties;
};

RK4Integrator::RK4Integrator()
{
    m = new Impl;
}

RK4Integrator::~RK4Integrator()
{
    delete m;
}

void RK4Integrator::prepareRK4Step(PhysicsSpace &space, float dt, Array<PointDerivative> &derivatives, Array<PointDerivative> &outDerivatives, const ShapeMatchDragData &dragData, ConsoleProfileInfo &profileInfo)
{
    if (m->rkTemp.pos.size() != space.points.size())
    {
        m->rkTemp.pos.fill(Vector2(), space.points.size());
        m->rkTemp.velocity.fill(Vector2(), space.points.size());
        m->rkTemp.shapeOriginalPos.replace(space.points.shapeOriginalPos);
    }

    m->rkTemp.mass.replace(space.points.mass);
    PointDerivative outDerivative;

    if (space.gravityEnabled)
    {
        outDerivative.acceleration = Vector2(0.0f, 0.00015f); // Gravity
    }

    outDerivatives.fill(outDerivative, space.points.size());

    Vector2 *posOut = &m->rkTemp.pos[0];
    Vector2 *velOut = &m->rkTemp.velocity[0];
    PointDerivative *outDerivativeOut = &outDerivatives[0];

    PointDerivative *inDerivative = &derivatives[0];
    Vector2 *posIn = &space.points.pos[0];
    Vector2 *posOutEnd = posOut + space.points.size();
    Vector2 *velIn = &space.points.velocity[0];

    float firstPointVelocity = velIn->y;

    while (posOut != posOutEnd)
    {
        PointDerivative derivative = *inDerivative++;
        Vector2 originalVelocity = *velIn++;
        Vector2 originalPos = *posIn++;
        outDerivativeOut->velocity = originalVelocity;
        outDerivativeOut++;
        *posOut++ = originalPos + derivative.velocity * dt;
        *velOut++ = originalVelocity + derivative.acceleration * dt;
    }
}

void RK4Integrator::updateRK4Springs(PhysicsSpace &space, Array<PointDerivative> &outDerivatives, const ShapeMatchDragData &dragData, ConsoleProfileInfo &profileInfo)
{
    PointMassesRange pointsRange = m->rkTemp.range();
    Range<PointDerivative> derivativeRange = outDerivatives.range();

    if (!space.springsEnabled)
    {
        return;
    }

    Springs::performThreadedSpringDerivatives(space.shapes.range(), m->shapeProperties.range(), pointsRange, space.springs.range(), derivativeRange, space.shapeMatchingEnabled, dragData, profileInfo);

    if (dragData.dragShapeIndex != -1)
    {
        Shape &shape = space.shapes[dragData.dragShapeIndex];

        for (int i = shape.start; i < shape.end; i++)
        {
            PointDerivative &derivative = outDerivatives[i];
            derivative.acceleration += space.points.velocity[i] * -0.008f;
        }
    }
}

float maxDistFromCenter = 180.0f;

void Integrator::performIntegration(PhysicsSpace &space, const ShapeMatchDragData &dragData, ConsoleProfileInfo &profileInfo)
{
    space.updateIndices();
    rk4Integrator.performRK4Integration(space, dragData, profileInfo);

    for (int i = 0; i < space.springs.size(); i++)
    {
        Spring &spring = space.springs[i];
        const Shape &shapeA = space.shapes[spring.shapeIndex];

        if (shapeA.isStatic || shapeA.disableShapeMatching)
        {
            continue;
        }

        if (spring.shapeIndex == -1)
        {
            continue;
        }
    }

    for (int i = 0; i < space.staticJoints.size(); i++)
    {
        StaticJoint &joint = space.staticJoints[i];
        space.points.pos[joint.pointIndex] = joint.position;
        Console::drawPoint(joint.position, 0xff0000ff);
        space.points.velocity[joint.pointIndex] = Vector2();
    }

    for (int i = 0; i < space.pointJoints.size(); i++)
    {
        PointJoint &joint = space.pointJoints[i];
        Vector2 midPoint = (space.points.pos[joint.pointIndex] + space.points.pos[joint.otherPointIndex]) / 2.0f;
        space.points.pos[joint.pointIndex] = midPoint;
        space.points.pos[joint.otherPointIndex] = midPoint;
        Vector2 midVelocity = (space.points.velocity[joint.pointIndex] + space.points.velocity[joint.otherPointIndex]) / 2.0f;
        space.points.velocity[joint.pointIndex] = midVelocity;
        space.points.velocity[joint.otherPointIndex] = midVelocity;
    }

    if (space.mouseJoint.pointIndex != -1)
    {
        space.points.pos[space.mouseJoint.pointIndex] = space.mouseJoint.position;
        space.points.velocity[space.mouseJoint.pointIndex] = Vector2();
    }

    for (int i = 0; i < space.shapes.size(); i++)
    {
        Shape &shape = space.shapes[i];

        if (shape.isStatic || shape.disableShapeMatching)
        {
            continue;
        }
    }
}

float maxVelocity = 5.0f;
float maxVelocitySquared = maxVelocity * maxVelocity;

void RK4Integrator::performRK4Integration(PhysicsSpace &space, const ShapeMatchDragData &dragData, ConsoleProfileInfo &profileInfo)
{
    if (m->rkEmptyDerivatives.size() != space.points.size())
    {
        m->rkEmptyDerivatives.fill(PointDerivative(), space.points.size());
    }

    m->shapeProperties.clear();

    for (int i = 0; i < space.shapes.size(); i++)
    {
        m->shapeProperties.push(ShapeUtils::getShapeProperties(space.points.range(), space.shapes[i]));
    }

    prepareRK4Step(space, 0.0, m->rkEmptyDerivatives, m->rk1, dragData, profileInfo);
    updateRK4Springs(space, m->rk1, dragData, profileInfo);
    prepareRK4Step(space, physicsStep * 0.5f, m->rk1, m->rk2, dragData, profileInfo);
    updateRK4Springs(space, m->rk2, dragData, profileInfo);
    prepareRK4Step(space, physicsStep * 0.5f, m->rk2, m->rk3, dragData, profileInfo);
    updateRK4Springs(space, m->rk3, dragData, profileInfo);
    prepareRK4Step(space, physicsStep, m->rk3, m->rk4, dragData, profileInfo);
    updateRK4Springs(space, m->rk4, dragData, profileInfo);

    float factor = (1.0f / 6.0f) * physicsStep;

    for (int i = 0; i < space.shapes.size(); i++)
    {
        const Shape &shape = space.shapes[i];
        if (shape.isStatic)
        {
            for (int j = shape.start; j < shape.end; j++)
            {
                m->rk1[j].velocity = Vector2();
                m->rk1[j].acceleration = Vector2();
                m->rk2[j].velocity = Vector2();
                m->rk2[j].acceleration = Vector2();
                m->rk3[j].velocity = Vector2();
                m->rk3[j].acceleration = Vector2();
                m->rk4[j].velocity = Vector2();
                m->rk4[j].acceleration = Vector2();
            }
        }
    }

    for (int i = 0; i < space.points.size(); i++)
    {
        PointDerivative rk1d = m->rk1[i];
        PointDerivative rk2d = m->rk2[i];
        PointDerivative rk3d = m->rk3[i];
        PointDerivative rk4d = m->rk4[i];

        Vector2 deltaVelocity = (rk1d.velocity + (rk2d.velocity + rk3d.velocity) * 2.0f + rk4d.velocity) * factor;
        Vector2 deltaAcceleration = (rk1d.acceleration + (rk2d.acceleration + rk3d.acceleration) * 2.0f + rk4d.acceleration) * factor;

        space.points.pos[i] += deltaVelocity;
        space.points.velocity[i] += deltaAcceleration;
    }

    m->shapeProperties.clear();

    for (int i = 0; i < space.shapes.size(); i++)
    {
        m->shapeProperties.push(ShapeUtils::getShapeProperties(space.points.range(), space.shapes[i]));
    }

    for (int i = 0; i < space.springs.size(); i++)
    {
        const Spring &spring = space.springs[i];
        const Vector2 pointA = space.points.pos[spring.pointA];
        const Vector2 pointB = space.points.pos[spring.pointB];
        const ShapeProperties &averages = m->shapeProperties[spring.shapeIndex];
        const Shape &shape = space.shapes[spring.shapeIndex];

        // These don't work well since multiple springs are connected to the same point,
        // and the point has different meanings in each substructure

        Vector2 origPointA = ShapeUtils::getShapePos(space.points.range(), space.shapes[spring.shapeIndex], spring.pointA, averages, dragData);
        Vector2 origPointB = ShapeUtils::getShapePos(space.points.range(), space.shapes[spring.shapeIndex], spring.pointB, averages, dragData);

        float dot = (pointB - pointA).dot(origPointB - origPointA);

        // if (dot < 0)
        // {
        //     Console::log("Flipping spring %d => %d %d", i, spring.pointA, spring.pointB);
        //     if (shape.hasIndices())
        //     {
        //         space.points.pos[spring.pointB] = origPointB;
        //         space.points.pos[spring.pointA] = origPointA;
        //         space.points.velocity[spring.pointA] *= 0.5f;
        //         space.points.velocity[spring.pointB] *= 0.5f;
        //     }
        //     else
        //     {
        //         space.points.pos[spring.pointB] = pointA;
        //         space.points.pos[spring.pointA] = pointB;
        //         Vector2 velocityA = space.points.velocity[spring.pointA];
        //         space.points.velocity[spring.pointA] = space.points.velocity[spring.pointB] * 0.5f;
        //         space.points.velocity[spring.pointB] = velocityA * 0.5f;
        //     }
        // }
    }

    // for (int i = 0; i < space.shapes.size(); i++)
    // {
    //     Shape &shape = space.shapes[i];

    //     if (shape.isStatic || shape.disableShapeMatching)
    //     {
    //         continue;
    //     }

    //     ShapeProperties &averages = m->shapeProperties[i];

    //     for (ShapeIterator s(shape); s.isValid(); s.next())
    //     {
    //         int j = s.index();
    //         Vector2 shapePos = ShapeUtils::getShapePos(space.points.range(), shape, j, averages, dragData);

    //         Vector2 delta = space.points.pos[j] - shapePos;

    //         if (delta.length() > maxDistFromCenter)
    //         {
    //             space.points.pos[j] = shapePos + delta.normalized() * maxDistFromCenter;
    //         }
    //     }
    // }
}

void RK4Integrator::testRK4Performance(int iterations, PhysicsSpace &space)
{
    m->rkEmptyDerivatives.fill(PointDerivative(), space.points.size());
    ConsoleProfileInfo profileInfo;
    ShapeMatchDragData dragData;
    for (int i = 0; i < iterations; i++)
    {
        performRK4Integration(space, dragData, profileInfo);
    }
}

void RK4Integrator::testRK4PreparePerformance(int iterations, PhysicsSpace &space)
{
    ConsoleProfileInfo profileInfo;
    m->rkEmptyDerivatives.fill(PointDerivative(), space.points.size());

    ShapeMatchDragData dragData;
    for (int i = 0; i < iterations; i++)
    {
        prepareRK4Step(space, 0.0, m->rkEmptyDerivatives, m->rk1, dragData, profileInfo);
    }
}

void RK4Integrator::testSpringPerformance(int iterations, PhysicsSpace &space)
{
    ConsoleProfileInfo profileInfo;
    m->rkEmptyDerivatives.fill(PointDerivative(), space.points.size());

    Array<ShapeProperties> shapeProperties;
    shapeProperties.fill(ShapeProperties(), space.shapes.size());

    ShapeMatchDragData dragData;
    prepareRK4Step(space, 0.0, m->rkEmptyDerivatives, m->rk1, dragData, profileInfo);
    auto derivativeRange = m->rk1.range();
    for (int i = 0; i < iterations; i++)
    {
        Springs::performThreadedSpringDerivatives(space.shapes.range(), shapeProperties.range(), space.points.range(), space.springs.range(), derivativeRange, space.shapeMatchingEnabled, dragData, profileInfo);
    }
}
