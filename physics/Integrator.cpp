#include "Integrator.h"
#include "PhysicsSpace.h"
#include "../utils/Console.h"
#include "../utils/MinMax.h"
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
        outDerivative.acceleration = Vector2(0.0f, space.gravity);
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
}

float maxDistFromCenter = 180.0f;

static float averageWheelRadius(PointMassesRange points, const Shape &shape, const Vector2 &center)
{
    float radius = 0.0f;
    int count = 0;

    for (ShapeIterator iter(shape); iter.isValid(); iter.next())
    {
        radius += (points.pos[iter.index()] - center).length();
        count++;
    }

    return count > 0 ? radius / (float)count : 0.0f;
}

static Vector2 wheelCenter(PointMassesRange points, const Shape &shape)
{
    Vector2 center;
    int count = 0;

    for (ShapeIterator iter(shape); iter.isValid(); iter.next())
    {
        center += points.pos[iter.index()];
        count++;
    }

    return count > 0 ? center / (float)count : Vector2();
}

static Vector2 computeParentCenterOfMassVelocity(PointMassesRange points, const PhysicsSpace &space, int parentId)
{
    Vector2 weightedVelocity;
    float totalMass = 0.0f;

    for (int i = 0; i < space.shapes.size(); i++)
    {
        const Shape &shape = space.shapes[i];
        if (shape.parentId != parentId)
        {
            continue;
        }

        for (ShapeIterator iter(shape); iter.isValid(); iter.next())
        {
            int pointIndex = iter.index();
            float mass = points.mass[pointIndex];
            weightedVelocity += points.velocity[pointIndex] * mass;
            totalMass += mass;
        }
    }

    return totalMass > 0.0f ? weightedVelocity / totalMass : Vector2();
}

static void applyVelocityDeltaToParent(PhysicsSpace &space, int parentId, const Vector2 &deltaVelocity)
{
    if (deltaVelocity.isZero())
    {
        return;
    }

    PointMassesRange points = space.points.range();
    for (int i = 0; i < space.shapes.size(); i++)
    {
        const Shape &shape = space.shapes[i];
        if (shape.parentId != parentId)
        {
            continue;
        }

        for (ShapeIterator iter(shape); iter.isValid(); iter.next())
        {
            points.velocity[iter.index()] += deltaVelocity;
        }
    }
}

static bool computeParentForwardAxis(PhysicsSpace &space, int parentId, Vector2 &outAxis)
{
    PointMassesRange points = space.points.range();
    Vector2 firstCenter;
    Vector2 lastCenter;
    bool foundFirst = false;

    for (int i = 0; i < space.wheelMotors.size(); i++)
    {
        const WheelMotor &wheelMotor = space.wheelMotors[i];
        if (wheelMotor.parentId != parentId || wheelMotor.shapeIndex < 0 || wheelMotor.shapeIndex >= space.shapes.size())
        {
            continue;
        }

        Vector2 center = wheelCenter(points, space.shapes[wheelMotor.shapeIndex]);
        if (!foundFirst)
        {
            firstCenter = center;
            foundFirst = true;
        }

        lastCenter = center;
    }

    if (!foundFirst)
    {
        return false;
    }

    Vector2 axis = lastCenter - firstCenter;
    if (axis.lengthSquared() <= 0.0001f)
    {
        axis = Vector2::right();
    }

    outAxis = axis.normalized();
    return true;
}

static bool computeGroundDriveState(PhysicsSpace &space, int parentId, const Vector2 &forwardAxis, Vector2 &outGroundTangent, float &outGroundSpeed)
{
    Vector2 tangentSum;
    float groundSpeedSum = 0.0f;
    int totalContacts = 0;

    for (int i = 0; i < space.wheelMotors.size(); i++)
    {
        const WheelMotor &wheelMotor = space.wheelMotors[i];
        if (wheelMotor.parentId != parentId || wheelMotor.groundedContactCount <= 0)
        {
            continue;
        }

        Vector2 tangent = wheelMotor.groundedTangentSum;
        if (tangent.lengthSquared() <= 0.0001f)
        {
            continue;
        }

        tangent = tangent.normalized();
        if (tangent.dot(forwardAxis) < 0.0f)
        {
            tangent = -tangent;
        }

        Vector2 averageGroundVelocity = wheelMotor.groundedGroundVelocitySum / (float)wheelMotor.groundedContactCount;
        float groundSpeed = averageGroundVelocity.dot(tangent);

        tangentSum += tangent * (float)wheelMotor.groundedContactCount;
        groundSpeedSum += groundSpeed * (float)wheelMotor.groundedContactCount;
        totalContacts += wheelMotor.groundedContactCount;
    }

    if (totalContacts == 0 || tangentSum.lengthSquared() <= 0.0001f)
    {
        return false;
    }

    outGroundTangent = tangentSum.normalized();
    if (outGroundTangent.dot(forwardAxis) < 0.0f)
    {
        outGroundTangent = -outGroundTangent;
    }
    outGroundSpeed = groundSpeedSum / (float)totalContacts;
    return true;
}

static float computeWheelSurfaceSpeed(PointMassesRange points, const Shape &shape, ShapeVelocities &outAverageVelocity, float &outRadius)
{
    outAverageVelocity = ShapeUtils::getAverageShapeVelocity(points, shape);
    outRadius = averageWheelRadius(points, shape, outAverageVelocity.centerOfMass);
    if (outRadius <= 0.0f)
    {
        return 0.0f;
    }

    return outAverageVelocity.angularVelocity * outRadius;
}

static float applyWheelVisualSpin(PointMassesRange points, const Shape &shape, float desiredSurfaceSpeed, float maxDelta, float &outSurfaceSpeed, float &outSpeedError)
{
    ShapeVelocities averageVelocity;
    float averageRadius = 0.0f;
    outSurfaceSpeed = computeWheelSurfaceSpeed(points, shape, averageVelocity, averageRadius);
    outSpeedError = desiredSurfaceSpeed - outSurfaceSpeed;

    if (averageRadius <= 0.0f || maxDelta <= 0.0f)
    {
        return 0.0f;
    }

    float spinDelta = clamp(outSpeedError * 0.35f, -maxDelta, maxDelta);
    for (ShapeIterator iter(shape); iter.isValid(); iter.next())
    {
        int pointIndex = iter.index();
        Vector2 relPos = points.pos[pointIndex] - averageVelocity.centerOfMass;
        float radialLengthSquared = relPos.lengthSquared();
        if (radialLengthSquared <= 0.0001f)
        {
            continue;
        }

        Vector2 tangent = relPos.normalVector() * (1.0f / sqrtf(radialLengthSquared));
        points.velocity[pointIndex] += tangent * spinDelta;
    }

    return spinDelta;
}

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

void Integrator::applyWheelMotorTraction(PhysicsSpace &space)
{
    PointMassesRange points = space.points.range();
    Array<int> processedParentIds;

    for (int i = 0; i < space.wheelMotors.size(); i++)
    {
        WheelMotor &wheelMotor = space.wheelMotors[i];
        if (wheelMotor.parentId == -1)
        {
            continue;
        }

        bool alreadyProcessed = false;
        for (int j = 0; j < processedParentIds.size(); j++)
        {
            if (processedParentIds[j] == wheelMotor.parentId)
            {
                alreadyProcessed = true;
                break;
            }
        }

        if (alreadyProcessed)
        {
            continue;
        }

        processedParentIds.push(wheelMotor.parentId);

        Vector2 forwardAxis;
        if (!computeParentForwardAxis(space, wheelMotor.parentId, forwardAxis))
        {
            continue;
        }

        Vector2 groundTangent;
        float groundSpeed = 0.0f;
        if (!computeGroundDriveState(space, wheelMotor.parentId, forwardAxis, groundTangent, groundSpeed))
        {
            continue;
        }

        Vector2 parentVelocity = computeParentCenterOfMassVelocity(points, space, wheelMotor.parentId);
        float parentForwardSpeed = parentVelocity.dot(groundTangent);
        float relativeForwardSpeed = parentForwardSpeed - groundSpeed;

        float parentCommand = 0.0f;
        float targetSurfaceSpeed = 0.0f;
        float maxDriveImpulse = 0.0f;
        float maxBrakeImpulse = 0.0f;
        float reverseEngageSpeed = 0.0f;

        for (int j = 0; j < space.wheelMotors.size(); j++)
        {
            const WheelMotor &otherWheelMotor = space.wheelMotors[j];
            if (otherWheelMotor.parentId != wheelMotor.parentId)
            {
                continue;
            }

            if (otherWheelMotor.command != 0.0f)
            {
                parentCommand = otherWheelMotor.command;
            }

            targetSurfaceSpeed = max(targetSurfaceSpeed, otherWheelMotor.targetSurfaceSpeed);
            maxDriveImpulse = max(maxDriveImpulse, otherWheelMotor.maxDriveImpulsePerStep);
            maxBrakeImpulse = max(maxBrakeImpulse, otherWheelMotor.maxBrakeImpulsePerStep);
            reverseEngageSpeed = max(reverseEngageSpeed, otherWheelMotor.reverseEngageSpeed);
        }

        WheelMotorMode mode = WheelMotorMode::Coast;
        float appliedDelta = 0.0f;
        float commandSpaceSpeed = 0.0f;
        float authorityClamp = 0.0f;
        if (parentCommand != 0.0f)
        {
            commandSpaceSpeed = relativeForwardSpeed * parentCommand;
            float handoverBand = max(reverseEngageSpeed, 0.0001f);

            if (commandSpaceSpeed <= -handoverBand)
            {
                mode = WheelMotorMode::Brake;
                authorityClamp = maxBrakeImpulse;
                appliedDelta = clamp(-relativeForwardSpeed, -authorityClamp, authorityClamp);
            }
            else if (commandSpaceSpeed < handoverBand)
            {
                mode = WheelMotorMode::Handover;
                if (commandSpaceSpeed < 0.0f)
                {
                    float brakeBlend = clamp((-commandSpaceSpeed) / handoverBand, 0.0f, 1.0f);
                    float minBrakeClamp = maxDriveImpulse * 0.10f;
                    authorityClamp = minBrakeClamp + (maxBrakeImpulse - minBrakeClamp) * brakeBlend;
                    appliedDelta = clamp(-relativeForwardSpeed, -authorityClamp, authorityClamp);
                }
                else
                {
                    float driveBlend = clamp(commandSpaceSpeed / handoverBand, 0.0f, 1.0f);
                    float minDriveClamp = maxDriveImpulse * 0.10f;
                    authorityClamp = minDriveClamp + (maxDriveImpulse - minDriveClamp) * driveBlend;
                    float desiredRelativeSpeed = parentCommand * targetSurfaceSpeed;
                    float speedError = desiredRelativeSpeed - relativeForwardSpeed;
                    appliedDelta = clamp(speedError * 0.75f, -authorityClamp, authorityClamp);
                }
            }
            else
            {
                mode = WheelMotorMode::Drive;
                authorityClamp = maxDriveImpulse;
                float desiredRelativeSpeed = parentCommand * targetSurfaceSpeed;
                float speedError = desiredRelativeSpeed - relativeForwardSpeed;
                appliedDelta = clamp(speedError * 0.75f, -authorityClamp, authorityClamp);
            }
        }

        applyVelocityDeltaToParent(space, wheelMotor.parentId, groundTangent * appliedDelta);

        for (int j = 0; j < space.wheelMotors.size(); j++)
        {
            WheelMotor &parentWheelMotor = space.wheelMotors[j];
            if (parentWheelMotor.parentId != wheelMotor.parentId ||
                parentWheelMotor.shapeIndex < 0 ||
                parentWheelMotor.shapeIndex >= space.shapes.size())
            {
                continue;
            }

            const Shape &wheelShape = space.shapes[parentWheelMotor.shapeIndex];
            float desiredSurfaceSpeed = 0.0f;
            float visualLimit = parentWheelMotor.maxDriveImpulsePerStep;

            if (mode == WheelMotorMode::Drive)
            {
                desiredSurfaceSpeed = parentCommand * parentWheelMotor.targetSurfaceSpeed;
            }
            else if (mode == WheelMotorMode::Handover)
            {
                desiredSurfaceSpeed = relativeForwardSpeed;
                visualLimit = max(parentWheelMotor.maxDriveImpulsePerStep * 0.25f, authorityClamp);
            }
            else if (mode == WheelMotorMode::Brake)
            {
                desiredSurfaceSpeed = relativeForwardSpeed;
                visualLimit = max(parentWheelMotor.maxBrakeImpulsePerStep * 0.35f, parentWheelMotor.maxDriveImpulsePerStep);
            }
            else if (mode == WheelMotorMode::Coast)
            {
                desiredSurfaceSpeed = relativeForwardSpeed;
                visualLimit = parentWheelMotor.maxDriveImpulsePerStep * 0.25f;
            }

            float surfaceSpeed = 0.0f;
            float surfaceSpeedError = 0.0f;
            applyWheelVisualSpin(points, wheelShape, desiredSurfaceSpeed, visualLimit, surfaceSpeed, surfaceSpeedError);

            parentWheelMotor.lastMode = mode;
            parentWheelMotor.lastParentForwardSpeed = parentForwardSpeed;
            parentWheelMotor.lastGroundSpeed = groundSpeed;
            parentWheelMotor.lastRelativeForwardSpeed = relativeForwardSpeed;
            parentWheelMotor.lastCommandSpaceSpeed = commandSpaceSpeed;
            parentWheelMotor.lastAuthorityClamp = authorityClamp;
            parentWheelMotor.lastHandoverBand = reverseEngageSpeed;
            parentWheelMotor.lastSurfaceSpeed = surfaceSpeed;
            parentWheelMotor.lastSurfaceSpeedError = surfaceSpeedError;
            parentWheelMotor.lastAppliedImpulse = appliedDelta;
            parentWheelMotor.lastCommand = parentCommand;
        }
    }
}

void Integrator::dampWheelMotors(PhysicsSpace &space)
{
    PointMassesRange points = space.points.range();

    for (int i = 0; i < space.wheelMotors.size(); i++)
    {
        WheelMotor &wheelMotor = space.wheelMotors[i];
        if (wheelMotor.shapeIndex < 0 || wheelMotor.shapeIndex >= space.shapes.size())
        {
            continue;
        }

        if (wheelMotor.groundedThisStep)
        {
            continue;
        }

        const Shape &shape = space.shapes[wheelMotor.shapeIndex];
        ShapeVelocities averageVelocity = ShapeUtils::getAverageShapeVelocity(points, shape);
        float averageRadius = 0.0f;
        int numPoints = 0;

        for (ShapeIterator iter(shape); iter.isValid(); iter.next())
        {
            averageRadius += (points.pos[iter.index()] - averageVelocity.centerOfMass).length();
            numPoints++;
        }

        averageRadius = numPoints > 0 ? averageRadius / (float)numPoints : 0.0f;

        wheelMotor.lastMode = wheelMotor.enabled && wheelMotor.command != 0.0f ? WheelMotorMode::Air : WheelMotorMode::Coast;

        if (wheelMotor.enabled && wheelMotor.command != 0.0f && wheelMotor.maxDriveImpulsePerStep > 0.0f && averageRadius > 0.0f)
        {
            float currentSurfaceSpeed = averageVelocity.angularVelocity * averageRadius;
            float desiredSurfaceSpeed = wheelMotor.command * wheelMotor.targetSurfaceSpeed;
            float speedError = desiredSurfaceSpeed - currentSurfaceSpeed;
            float airDriveDelta = clamp(speedError * 0.15f,
                                        -wheelMotor.maxDriveImpulsePerStep * 0.35f,
                                        wheelMotor.maxDriveImpulsePerStep * 0.35f);

            for (ShapeIterator iter(shape); iter.isValid(); iter.next())
            {
                int pointIndex = iter.index();
                Vector2 relPos = points.pos[pointIndex] - averageVelocity.centerOfMass;
                float radialLengthSquared = relPos.lengthSquared();
                if (radialLengthSquared <= 0.0001f)
                {
                    continue;
                }

                Vector2 tangent = relPos.normalVector() * (1.0f / sqrtf(radialLengthSquared));
                points.velocity[pointIndex] += tangent * airDriveDelta;
            }

            wheelMotor.lastSurfaceSpeed = currentSurfaceSpeed;
            wheelMotor.lastSurfaceSpeedError = speedError;
            wheelMotor.lastAppliedImpulse = airDriveDelta;
        }
        else
        {
            wheelMotor.lastSurfaceSpeed = averageRadius > 0.0f ? averageVelocity.angularVelocity * averageRadius : 0.0f;
            wheelMotor.lastSurfaceSpeedError = 0.0f;
            wheelMotor.lastAppliedImpulse = 0.0f;
        }

        if (wheelMotor.freeSpinDamping <= 0.0f)
        {
            continue;
        }

        for (ShapeIterator iter(shape); iter.isValid(); iter.next())
        {
            int pointIndex = iter.index();
            Vector2 relPos = points.pos[pointIndex] - averageVelocity.centerOfMass;
            Vector2 rotationVelocity = Vector2(-relPos.y, relPos.x) * averageVelocity.angularVelocity;
            points.velocity[pointIndex] -= rotationVelocity * wheelMotor.freeSpinDamping;
        }
    }
}
