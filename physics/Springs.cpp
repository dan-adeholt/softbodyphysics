#include "Springs.h"
#include "ShapeUtils.h"
#include "../tasks/Scheduler.h"
#include "../timer.h"
#include "../utils/MinMax.h"
#include "../utils/Console.h"
#include <stdio.h>

float springFactor = 0.085f * 0.001f;

struct SpringJobData
{
    Range<Shape> shapes;
    Range<ShapeProperties> shapeProperties;
    Range<Spring> springs;
    PointMassesRange points;
    Range<PointDerivative> derivatives;
    bool enableShapeMatching;
    ShapeMatchDragData dragData;
};

void applySpringDerivatives(
    Range<Shape> &shapes,
    Range<ShapeProperties> &shapeProperties,
    PointMassesRange &points,
    Range<Spring> &springs,
    Range<PointDerivative> derivatives,
    bool enableShapeMatching,
    const ShapeMatchDragData &dragData)
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

        const ShapeProperties averages = ShapeUtils::getShapeProperties(points, shape);
        bool shapeIsBeingDragged = dragData.dragShapeIndex == shape.index;

        // Shape is being dragged == enable shape matching in order to drag the shape
        if ((shape.disableShapeMatching && !shapeIsBeingDragged) || shape.isStatic)
        {
            continue;
        }

        for (ShapeIterator s(shape); s.isValid(); s.next())
        {
            int j = s.index();
            PointDerivative &derivative = derivatives[j];
            float springStiffness = 0.1250f;
            float springDamping = 800.9f;

            Vector2 p0 = ShapeUtils::getShapePos(points, shape, j, averages, dragData);
            Vector2 p1(points.pos[j]);
            Vector2 direction = p0 - p1;
            float offsetLength = direction.length();

            if (offsetLength > 0.001f)
            {

                // Console::log("Shape: %d %d", shape.start, shape.end);
                Vector2 directionNormalized = direction.normalized();
                Vector2 averageVelocity = ShapeUtils::getAverageShapeVelocity(points, shape);
                Vector2 velocityAlongSpringAxis = directionNormalized * (points.velocity[j] - averageVelocity).dot(directionNormalized);
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

void springJob(void *data)
{
    SpringJobData *jobData = (SpringJobData *)data;
    applySpringDerivatives(jobData->shapes, jobData->shapeProperties, jobData->points, jobData->springs, jobData->derivatives, jobData->enableShapeMatching, jobData->dragData);
}

void Springs::performThreadedSpringDerivatives(Range<Shape> shapeRange,
                                               Range<ShapeProperties> shapeProperties,
                                               PointMassesRange points,
                                               Range<Spring> springs,
                                               Range<PointDerivative> derivatives,
                                               bool enableShapeMatching,
                                               const ShapeMatchDragData &dragData,
                                               ConsoleProfileInfo &profileInfo)
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
        int startShapeIndex = i == 0 ? 0 : springs[curStart].shapeIndex;
        int curShapeParentIndex = shapeRange[springs[curEnd - 1].shapeIndex].parentIndex;
        int curShapeIndex = springs[curEnd - 1].shapeIndex;

        while (curEnd < springs.size && (springs[curEnd].shapeIndex == curShapeIndex || (shapeRange[springs[curEnd].shapeIndex].parentIndex != -1 && shapeRange[springs[curEnd].shapeIndex].parentIndex == curShapeParentIndex)))
        {
            curEnd++;
        }

        int endShapeIndex = curEnd < springs.size ? springs[curEnd].shapeIndex : shapeRange.size;

        springRanges[i] = {
            shapeRange.slice(startShapeIndex, endShapeIndex),
            shapeProperties,
            springs.slice(curStart, curEnd),
            points,
            derivatives,
            enableShapeMatching,
            dragData};

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
