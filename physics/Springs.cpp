#include "Springs.h"
#include "../tasks/Scheduler.h"
#include "../timer.h"
#include "../utils/MinMax.h"
#include "../utils/Console.h"

float springFactor = 0.085f * 0.001f;

struct SpringJobData
{
    Range<Shape> shapes;
    Range<Spring> springs;
    PointMassesRange points;
    Range<PointDerivative> derivatives;
    Range<ShapeQuad> partialShapes;
    bool enableShapeMatching;
};

void applySpringDerivatives(Range<Shape> &shapes, PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives, Range<ShapeQuad> partialShapes, bool enableShapeMatching)
{

    for (int i = 0; i < springs.size; i++)
    {
        const Spring &spring = springs[i];

        Vector2 p0(points.pos[spring.pointA]);
        Vector2 p1(points.pos[spring.pointB]);

        Vector2 direction(p1 - p0);
        float offsetLength = direction.length();

        if (offsetLength > 0.001f)
        {
            float delta = (offsetLength - spring.length);

            float springDamping = spring.damping;
            float springStiffness = spring.stiffness;

            float springForce = delta * springStiffness;
            Vector2 directionNormalized = direction / offsetLength;

            Vector2 dv(points.velocity[spring.pointB] - points.velocity[spring.pointA]);
            float dampForce = directionNormalized.dot(dv * springDamping);
            float combinedForce = (springForce + dampForce) * springFactor;

            Vector2 force(directionNormalized * combinedForce);

            float p1Mass = points.mass[spring.pointA];
            float p2Mass = points.mass[spring.pointB];

            PointDerivative &derivative1 = derivatives[spring.pointA];
            PointDerivative &derivative2 = derivatives[spring.pointB];

            derivative1.acceleration += force / p1Mass;
            derivative2.acceleration -= force / p2Mass;
        }
    }

    if (!enableShapeMatching)
    {
        return;
    }

    for (int i = 0; i < shapes.size; i++)
    {
        const Shape &shape = shapes[i];

        if (shape.subShapeSpan.isValid())
        {
            Range<ShapeQuad> subShapes = partialShapes.slice(shape.subShapeSpan);
            for (int j = 0; j < subShapes.size; j++)
            {
                const ShapeQuad &subShape = subShapes[j];
                for (int k = 0; k < subShape.size; k++)
                {
                    int index = subShape.indices[k];
                    PointDerivative &derivative = derivatives[index];
                    Vector2 force = (subShape.shapePos[k] - points.pos[index]) * 0.00004f;
                    derivative.acceleration += force / points.mass[index];
                }
            }
        }
        else
        {
            Vector2 avgVelocity;

            for (int j = shape.start; j < shape.end; j++)
            {
                avgVelocity += points.velocity[j];
            }

            avgVelocity /= (shape.end - shape.start);

            for (int j = shape.start; j < shape.end; j++)
            {
                PointDerivative &derivative = derivatives[j];
                float springStiffness = 0.1250f;
                float springDamping = 800.9f;

                Vector2 p0(points.shapePos[j]);
                Vector2 p1(points.pos[j]);
                Vector2 direction = p0 - p1;
                float offsetLength = direction.length();

                if (offsetLength > 0.001f)
                {

                    // Console::log("Shape: %d %d", shape.start, shape.end);
                    Vector2 directionNormalized = direction.normalized();

                    Vector2 velocityAlongSpringAxis = directionNormalized * (points.velocity[j] - avgVelocity).dot(directionNormalized);
                    Vector2 force = (p0 - p1) * 0.00015f;
                    Vector2 acceleration = force;
                    // Console::log("Accel %f %f", acceleration.x, acceleration.y);

                    derivative.acceleration += acceleration;
                    derivative.acceleration += (velocityAlongSpringAxis * -0.0025f) / points.mass[j];
                    // derivative.acceleration -= (points.velocity[j] - avgVelocity) * 0.000025f;
                }
            }
        }
    }
}

void springJob(void *data)
{
    SpringJobData *jobData = (SpringJobData *)data;
    applySpringDerivatives(jobData->shapes, jobData->points, jobData->springs, jobData->derivatives, jobData->partialShapes, jobData->enableShapeMatching);
}

void Springs::performThreadedSpringDerivatives(Range<Shape> shapeRange, PointMassesRange points, Range<Spring> springs, Range<PointDerivative> derivatives, Range<ShapeQuad> partialShapes, bool enableShapeMatching, ConsoleProfileInfo &profileInfo)
{
    Timer springsTimer;
    SpringJobData springRanges[Scheduler::maxNumThreads];
    Task tasks[Scheduler::maxNumThreads];

    int numTasks = Scheduler::numTasks;
    int batchSize = springs.size / numTasks;

    while (batchSize * numTasks < springs.size)
    {
        batchSize++;
    }

    int curStart = 0;
    int numThreads = numTasks;

    // Ensure that no job chunks span across the same shape, because that would
    // cause point mass calculations from different threads to interfere with each other
    for (int i = 0; i < numTasks; i++)
    {
        int curEnd = min(curStart + batchSize, springs.size);
        int startShapeIndex = springs[curStart].shapeIndex;
        int curShapeIndex = springs[curEnd - 1].shapeIndex;

        while (curEnd < springs.size && springs[curEnd].shapeIndex == curShapeIndex)
        {
            curEnd++;
        }

        int endShapeIndex = springs[curEnd - 1].shapeIndex + 1;

        springRanges[i] = {
            shapeRange.slice(startShapeIndex, endShapeIndex),
            springs.slice(curStart, curEnd),
            points,
            derivatives,
            partialShapes,
            enableShapeMatching};

        tasks[i].function = springJob;
        tasks[i].data = &springRanges[i];

        curStart = curEnd;
        if (curStart == springs.size)
        {
            numThreads = i + 1;
            break;
        }
    }

    Scheduler::instance->schedule(tasks, numThreads);

    profileInfo.springsTimeMillis += springsTimer.elapsedMillis();
}
