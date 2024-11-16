#include "PhysicsIntegrator.h"
#include "PhysicsSpace.h"
#include "../utils/Console.h"
#include "./PhysicsSprings.h"
#include "./PhysicsShapeMatching.h"

void RK4Integrator::prepareRK4Step(PhysicsSpace &space, float dt, Array<PointDerivative> &derivatives, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo)
{
    if (rkTemp.pos.size() != space.points.size())
    {
        rkTemp.pos.fill(Vector2(), space.points.size());
        rkTemp.velocity.fill(Vector2(), space.points.size());
    }
    rkTemp.mass.replace(space.points.mass);
    rkTemp.shapePos.replace(space.points.shapePos);
    PointDerivative outDerivative;

    if (space.gravityEnabled)
    {
        outDerivative.acceleration = Vector2(0.0f, 0.00015f); // Gravity
    }
    outDerivatives.fill(outDerivative, space.points.size());

    Vector2 *posOut = &rkTemp.pos[0];
    Vector2 *velOut = &rkTemp.velocity[0];
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

void RK4Integrator::updateRK4Springs(PhysicsSpace &space, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo)
{
    PointMassesRange pointsRange = rkTemp.range();
    Range<PointDerivative> derivativeRange = outDerivatives.range();

    if (!space.springsEnabled)
    {
        return;
    }

    PhysicsSprings::performThreadedSpringDerivatives(space.shapes.range(), pointsRange, space.springs.range(), derivativeRange, space.partialShapes.range(), space.shapeMatchingEnabled, profileInfo);

    if (space.draggingShapeIndex != -1)
    {
        Shape &shape = space.shapes[space.draggingShapeIndex];

        for (int i = shape.start; i < shape.end; i++)
        {
            PointDerivative &derivative = outDerivatives[i];
            derivative.acceleration += space.points.velocity[i] * -0.008f;
        }
    }
}

float maxDistFromCenter = 14.0f;

void PhysicsIntegrator::performIntegration(PhysicsSpace &space, ConsoleProfileInfo &profileInfo)
{
    PhysicsShapeMatching::shapeMatchAlign(space.points.range(), space.shapes, space.partialShapes.range(), space.draggingShapeIndex);
    rk4Integrator.performRK4Integration(space, profileInfo);

    for (int i = 0; i < space.staticJoints.size(); i++)
    {
        StaticJoint &joint = space.staticJoints[i];
        space.points.pos[joint.pointIndex] = joint.position;
        space.points.velocity[joint.pointIndex] = Vector2();
    }

    if (space.mouseJoint.pointIndex != -1)
    {
        space.points.pos[space.mouseJoint.pointIndex] = space.mouseJoint.position;
        space.points.velocity[space.mouseJoint.pointIndex] = Vector2();
    }

    // for (int i = 0; i < space.shapes.size(); i++)
    // {
    //     Shape &shape = space.shapes[i];
    //     if (i == space.draggingShapeIndex)
    //     {
    //         continue;
    //     }

    //     if (shape.subShapeSpan.isValid())
    //     {
    //         Range<ShapeQuad> subShapes = space.partialShapes.range().slice(shape.subShapeSpan);
    //         for (int j = 0; j < subShapes.size; j++)
    //         {
    //             ShapeQuad &subShape = subShapes[j];
    //             Vector2 center;

    //             for (int k = 0; k < subShape.size; k++)
    //             {
    //                 center += space.points.pos[subShape.indices[k]];
    //             }

    //             center /= subShape.size;

    //             for (int k = 0; k < subShape.size; k++)
    //             {
    //                 Vector2 delta = center - space.points.pos[subShape.indices[k]];
    //                 if (delta.length() > maxDistFromCenter)
    //                 {
    //                     delta = delta.normalized() * maxDistFromCenter;
    //                     space.points.pos[subShape.indices[k]] = center - delta;
    //                 }
    //             }
    //         }
    //     }
    //     else
    //     {
    //         Vector2 center;

    //         for (int j = shape.start; j < shape.end; j++)
    //         {
    //             center += space.points.pos[j];
    //         }

    //         int numPoints = shape.end - shape.start;

    //         center /= numPoints;

    //         for (int j = shape.start; j < shape.end; j++)
    //         {
    //             Vector2 delta = center - space.points.pos[j];
    //             if (delta.length() > maxDistFromCenter)
    //             {
    //                 delta = delta.normalized() * maxDistFromCenter;
    //                 space.points.pos[j] = center - delta;
    //             }
    //         }
    //     }
    // }
}

float prevMaxVelocity = 0.0f;
void RK4Integrator::performRK4Integration(PhysicsSpace &space, ConsoleProfileInfo &profileInfo)
{
    if (rkEmptyDerivatives.size() != space.points.size())
    {
        rkEmptyDerivatives.fill(PointDerivative(), space.points.size());
    }

    prepareRK4Step(space, 0.0, rkEmptyDerivatives, rk1, profileInfo);
    updateRK4Springs(space, rk1, profileInfo);
    prepareRK4Step(space, physicsStep * 0.5f, rk1, rk2, profileInfo);
    updateRK4Springs(space, rk2, profileInfo);
    prepareRK4Step(space, physicsStep * 0.5f, rk2, rk3, profileInfo);
    updateRK4Springs(space, rk3, profileInfo);
    prepareRK4Step(space, physicsStep, rk3, rk4, profileInfo);
    updateRK4Springs(space, rk4, profileInfo);

    float factor = (1.0f / 6.0f) * physicsStep;

    for (int i = 0; i < space.points.size(); i++)
    {
        PointDerivative rk1d = rk1[i];
        PointDerivative rk2d = rk2[i];
        PointDerivative rk3d = rk3[i];
        PointDerivative rk4d = rk4[i];

        Vector2 deltaVelocity = (rk1d.velocity + (rk2d.velocity + rk3d.velocity) * 2.0f + rk4d.velocity) * factor;
        Vector2 deltaAcceleration = (rk1d.acceleration + (rk2d.acceleration + rk3d.acceleration) * 2.0f + rk4d.acceleration) * factor;
        space.points.pos[i] += deltaVelocity;
        space.points.velocity[i] += deltaAcceleration;

        // if (velocityAmplitude > 3.0f)
        // {
        //     space.points.velocity[i] *= 3.0f / velocityAmplitude;
        // }
    }
}

void RK4Integrator::testRK4Performance(int iterations, PhysicsSpace &space)
{
    rkEmptyDerivatives.fill(PointDerivative(), space.points.size());
    ConsoleProfileInfo profileInfo;

    for (int i = 0; i < iterations; i++)
    {
        performRK4Integration(space, profileInfo);
    }
}

void RK4Integrator::testRK4PreparePerformance(int iterations, PhysicsSpace &space)
{
    ConsoleProfileInfo profileInfo;
    rkEmptyDerivatives.fill(PointDerivative(), space.points.size());

    for (int i = 0; i < iterations; i++)
    {
        prepareRK4Step(space, 0.0, rkEmptyDerivatives, rk1, profileInfo);
    }
}

void RK4Integrator::testSpringPerformance(int iterations, PhysicsSpace &space)
{
    ConsoleProfileInfo profileInfo;
    rkEmptyDerivatives.fill(PointDerivative(), space.points.size());

    prepareRK4Step(space, 0.0, rkEmptyDerivatives, rk1, profileInfo);
    auto derivativeRange = rk1.range();
    for (int i = 0; i < iterations; i++)
    {
        PhysicsSprings::performThreadedSpringDerivatives(space.shapes.range(), space.points.range(), space.springs.range(), derivativeRange, space.partialShapes.range(), space.shapeMatchingEnabled, profileInfo);
    }
}
