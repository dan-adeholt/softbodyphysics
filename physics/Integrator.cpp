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

void RK4Integrator::clear()
{
    m->rkTemp.pos.clear();
    m->rkTemp.velocity.clear();
    m->rkTemp.shapeOriginalPos.clear();
    m->rkTemp.mass.clear();
    m->rk1.clear();
    m->rk2.clear();
    m->rk3.clear();
    m->rk4.clear();
    m->rkEmptyDerivatives.clear();
    m->shapeProperties.clear();
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

    Springs::performThreadedSpringDerivatives(space.shapes.range(), m->shapeProperties.range(), pointsRange, derivativeRange, space.shapeMatchingEnabled, dragData, profileInfo);

    if (dragData.dragShapeIndex != -1)
    {
        Shape &shape = space.shapes[dragData.dragShapeIndex];

        for (int i = shape.start; i < shape.end; i++)
        {
            PointDerivative &derivative = outDerivatives[i];
            derivative.acceleration += space.points.velocity[i] * -0.008f;
        }
    }

    for (int i = 0; i < space.radialAccelerators.size(); i++)
    {
        const RadialAccelerator &accelerator = space.radialAccelerators[i];
        if (!accelerator.enabled)
        {
            continue;
        }

        const Shape &shape = space.shapes[accelerator.shapeIndex];
        ShapeProperties properties = m->shapeProperties[accelerator.shapeIndex];

        for (ShapeIterator s(shape); s.isValid(); s.next())
        {
            int pointIndex = s.index();
            if (pointIndex < 0 || pointIndex >= space.points.size())
            {
                continue;
            }

            PointDerivative &derivative = outDerivatives[pointIndex];
            Vector2 pos = space.points.pos[pointIndex];
            Vector2 direction = pos - properties.center;
            Vector2 tangent = direction.normalVector();

            derivative.acceleration += tangent * accelerator.strength * 0.00002f;
        }
    }
}

float maxDistFromCenter = 180.0f;

void Integrator::performIntegration(PhysicsSpace &space, const ShapeMatchDragData &dragData, ConsoleProfileInfo &profileInfo)
{
    space.updateIndices();
    rk4Integrator.performRK4Integration(space, dragData, profileInfo);

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

    PointMassesRange points = space.points.range();
    for (int i = 0; i < space.shapeJoints.size(); i++)
    {
        const ShapeJoint &joint = space.shapeJoints[i];
        Vector2 vA, vB;

        for (int i = 0; i < 4; ++i)
        {
            if (joint.shape1Points[i] != -1)
            {
                vA += points.velocity[joint.shape1Points[i]] * joint.shape1Weights[i];
            }

            if (joint.shape2Points[i] != -1)
            {
                vB += points.velocity[joint.shape2Points[i]] * joint.shape2Weights[i];
            }
        }

        Vector2 v_rel = vB - vA;

        float invMassA = 0.0f;
        float invMassB = 0.0f;

        for (int i = 0; i < 4; ++i)
        {
            if (joint.shape1Points[i] != -1)
            {
                invMassA += joint.shape1Weights[i] * joint.shape1Weights[i] * (1.0f / (points.mass[joint.shape1Points[i]]));
            }

            if (joint.shape2Points[i] != -1)
            {
                invMassB += joint.shape2Weights[i] * joint.shape2Weights[i] * (1.0f / (points.mass[joint.shape2Points[i]]));
            }
        }

        float effectiveMass = 1.0f / (invMassA + invMassB);
        Vector2 impulse = -v_rel * effectiveMass;

        for (int i = 0; i < 4; ++i)
        {
            int idxA = joint.shape1Points[i];
            if (idxA != -1)
            {
                float w = joint.shape1Weights[i];
                points.velocity[idxA] -= impulse * (w * (1.0f / points.mass[idxA]));
            }

            int idxB = joint.shape2Points[i];
            if (idxB != -1)
            {
                float w = joint.shape2Weights[i];
                points.velocity[idxB] += impulse * (w * (1.0f / points.mass[idxB]));
            }
        }

        PositionPair posPair = ShapeUtils::getShapeJointPositions(points, joint);

        Vector2 delta = (posPair.p1 - posPair.p0) * 0.5f; // Half the error to each shape

        for (int i = 0; i < 4; i++)
        {
            int index0 = joint.shape1Points[i];
            if (index0 != -1)
            {
                float weight = joint.shape1Weights[i];
                points.pos[index0] += delta * weight; // proportional correction
            }

            int index1 = joint.shape2Points[i];
            if (index1 != -1)
            {
                float weight = joint.shape2Weights[i];
                points.pos[index1] -= delta * weight; // opposing correction
            }
        }
    }

    m->shapeProperties.clear();

    for (int i = 0; i < space.shapes.size(); i++)
    {
        m->shapeProperties.push(ShapeUtils::getShapeProperties(space.points.range(), space.shapes[i]));
    }
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
        Springs::performThreadedSpringDerivatives(space.shapes.range(), shapeProperties.range(), space.points.range(), derivativeRange, space.shapeMatchingEnabled, dragData, profileInfo);
    }
}

void Integrator::clear()
{
    rk4Integrator.clear();
}
