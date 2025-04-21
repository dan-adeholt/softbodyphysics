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
    PointMassesRange points;
    Range<PointDerivative> derivatives;
    bool enableShapeMatching;
    ShapeMatchDragData dragData;
};

void applySpringDerivatives(
    Range<Shape> &shapes,
    Range<ShapeProperties> &shapeProperties,
    PointMassesRange &points,
    Range<PointDerivative> derivatives,
    bool enableShapeMatching,
    const ShapeMatchDragData &dragData)
{
    if (!enableShapeMatching)
    {
        return;
    }

    for (int i = 0; i < shapes.size; i++)
    {
        const Shape &shape = shapes[i];

        const ShapeProperties averages = ShapeUtils::getShapeProperties(points, shape);

        // Shape is being dragged == enable shape matching in order to drag the shape
        if (shape.isStatic)
        {
            continue;
        }

        ShapeVelocities averageVelocity = ShapeUtils::getAverageShapeVelocity(points, shape);

        for (ShapeIterator s(shape); s.isValid(); s.next())
        {
            int j = s.index();
            PointDerivative &derivative = derivatives[j];

            Vector2 p0 = ShapeUtils::getShapePos(points, shape, j, averages, dragData);
            Vector2 p1(points.pos[j]);

            Vector2 relPos = points.pos[j] - averageVelocity.centerOfMass;
            Vector2 rotationVelocity = Vector2(-relPos.y, relPos.x) * averageVelocity.angularVelocity;
            Vector2 targetVelocity = averageVelocity.centerOfMassVelocity + rotationVelocity;
            Vector2 direction = p0 - p1;

            float offsetLength = direction.length();

            float shapeStiffness = shape.stiffness * baseStiffness;
            float shapeDamping = shape.damping * baseDamping;

            float stiffness = shape.index == dragData.dragShapeIndex ? 0.0025f : shapeStiffness;
            float damping = shape.index == dragData.dragShapeIndex ? 0.015f : shapeDamping;

            if (offsetLength > 0.001f)
            {
                Vector2 directionNormalized = direction.normalized();
                Vector2 velocityAlongSpringAxis = directionNormalized * (points.velocity[j] - averageVelocity.centerOfMassVelocity).dot(directionNormalized);

                Vector2 force = direction * stiffness;

                derivative.acceleration += force;
                Vector2 velocityDifference = targetVelocity - points.velocity[j];

                derivative.acceleration += velocityDifference * damping;

                if (shape.index == dragData.dragShapeIndex)
                {
                    derivative.acceleration -= points.velocity[j] * 0.025f;
                }
            }
        }
    }
}

void springJob(void *data)
{
    SpringJobData *jobData = (SpringJobData *)data;
    applySpringDerivatives(jobData->shapes, jobData->shapeProperties, jobData->points, jobData->derivatives, jobData->enableShapeMatching, jobData->dragData);
}

void Springs::performThreadedSpringDerivatives(Range<Shape> shapeRange,
                                               Range<ShapeProperties> shapeProperties,
                                               PointMassesRange points,
                                               Range<PointDerivative> derivatives,
                                               bool enableShapeMatching,
                                               const ShapeMatchDragData &dragData,
                                               ConsoleProfileInfo &profileInfo)
{

    Timer springsTimer;
    SpringJobData springRanges[Scheduler::maxNumThreads];
    Task tasks[Scheduler::maxNumThreads];

    int numTasks = Scheduler::numTasks;
    int batchSize = shapeRange.size / numTasks;

    while (batchSize * numTasks < shapeRange.size)
    {
        batchSize++;
    }

    int curStart = 0;
    int numThreads = numTasks;

    // Ensure that no job chunks span across the same shape, because that would
    // cause point mass calculations from different threads to interfere with each other
    for (int i = 0; i < numTasks; i++)
    {
        int curEnd = min(curStart + batchSize, shapeRange.size);

        springRanges[i] = {
            shapeRange.slice(curStart, curEnd),
            shapeProperties,
            points,
            derivatives,
            enableShapeMatching,
            dragData};

        tasks[i].function = springJob;
        tasks[i].data = &springRanges[i];

        curStart = curEnd;
        if (curStart == shapeRange.size)
        {
            numThreads = i + 1;
            break;
        }
    }

    Scheduler::instance->schedule(tasks, numThreads);

    profileInfo.springsTimeMillis += springsTimer.elapsedMillis();
}
