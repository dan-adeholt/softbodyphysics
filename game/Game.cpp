
#include "./Game.h"
#include "../utils/MinMax.h"
#include "../containers/Range.h"
#include "../containers/Array.h"
#include "./Physics.h"
#include <stdio.h>
#include "Shapes.h"
#include "Scenes.h"
#include <cassert>
#include <cstring>
#include "Game.h"
#include "../utils/Console.h"
#include "../timer.h"

#define NUM_HISTORICAL_STATES 240
#define NUM_SHAPES 40
#define NUM_POINTS 1024

struct HistoricalState
{
    Array<Shape> shapes;
    PointMasses staticPoints;
    PointMasses points;
    Array<Spring> springs;
    Array<StaticJoint> staticJoints;
    Array<Shape> staticShapes;
};

struct Game::Impl
{
    Impl() : historicalIndex(0), historyRewindIndex(0), gravityEnabled(true), collisionsEnabled(true), timeBucket(0.0)
    {
        shapes.reserve(NUM_SHAPES);
        points.reserve(NUM_POINTS);
        staticShapes.reserve(NUM_SHAPES);
        staticPoints.reserve(NUM_POINTS);
        collisionCounterForPoints.reserve(NUM_POINTS);
        boundingBoxes.reserve(NUM_SHAPES);
        staticBoundingBoxes.reserve(NUM_SHAPES);
        sortedBoundingBoxes.reserve(NUM_SHAPES);
        sortedStaticBoundingBoxes.reserve(NUM_SHAPES);
        staticJoints.reserve(NUM_POINTS / 2);
        springs.reserve(NUM_POINTS / 2);
        mouseJoint.pointIndex = -1;
        mouseJoint.position = Vector2::zero();
    }

    void clear()
    {
        shapes.clear();
        points.clear();
        staticShapes.clear();
        staticPoints.clear();
        springs.clear();
        staticJoints.clear();
        collisionCounterForPoints.clear();
        boundingBoxes.clear();
        staticBoundingBoxes.clear();
        sortedBoundingBoxes.clear();
        sortedStaticBoundingBoxes.clear();
        mouseJoint.pointIndex = -1;
        gravityEnabled = true;
    }

    HistoricalState history[NUM_HISTORICAL_STATES];
    int historicalIndex;
    int historyRewindIndex;
    Array<Shape> shapes;
    PointMasses points;
    Array<Shape> staticShapes;
    PointMasses staticPoints;
    Array<Spring> springs;
    Array<StaticJoint> staticJoints;
    Array<int> collisionCounterForPoints;
    Array<ShapeBoundingBox> boundingBoxes;
    Array<ShapeBoundingBox> staticBoundingBoxes;
    Array<ShapeBoundingBox> sortedBoundingBoxes;
    Array<ShapeBoundingBox> sortedStaticBoundingBoxes;

    Array<PointDerivative> rk1;
    Array<PointDerivative> rk2;
    Array<PointDerivative> rk3;
    Array<PointDerivative> rk4;
    PointMasses rkTemp;
    Array<PointDerivative> rkEmptyDerivatives;

    StaticJoint mouseJoint;
    bool gravityEnabled;
    bool collisionsEnabled;
    double timeBucket;
};

Game::Game() : m_impl(new Game::Impl)
{
}

void Game::init(const char *sceneType)
{
    for (int i = 0; i < SceneDefinition::numScenes; i++)
    {
        const SceneDefinition &def = SceneDefinition::allScenes[i];
        if (strcmp(def.name, sceneType) == 0)
        {
            def.initFunc(this);
            return;
        }
    }

    assert("Invalid scene type passed!");
}

Game::~Game()
{
    delete m_impl;
}

float terminalVelocity = 1000.0f;

void Game::getDynamicShapes(Range<Shape> &shapes) const
{
    shapes = m_impl->shapes.range(0, m_impl->shapes.size());
}

void Game::getStaticShapes(Range<Shape> &shapes) const
{
    shapes = m_impl->staticShapes.range(0, m_impl->staticShapes.size());
}

void Game::getSprings(Range<Spring> &springs)
{
    springs = m_impl->springs.range(0, m_impl->springs.size());
}

void Game::getDynamicPoints(PointMassesRange &pointMasses) const
{
    pointMasses = m_impl->points.range(0, m_impl->points.size());
}

void Game::getStaticPoints(PointMassesRange &pointMasses) const
{
    pointMasses = m_impl->staticPoints.range(0, m_impl->staticPoints.size());
}

void sortBoundingBoxes(Array<ShapeBoundingBox> &sortedBoundingBoxes)
{
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    for (int i = 1; i < sortedBoundingBoxes.size(); i++)
    {
        ShapeBoundingBox item = sortedBoundingBoxes[i];
        int j = i - 1;
        while (j >= 0 && sortedBoundingBoxes[j].x1 > item.x1)
        {
            sortedBoundingBoxes[j + 1] = sortedBoundingBoxes[j];
            j--;
        }
        sortedBoundingBoxes[j + 1] = item;
    }
}

void updateSortedBoundingBoxes(Array<ShapeBoundingBox> &sortedBoundingBoxes, Array<ShapeBoundingBox> &boundingBoxes)
{
    for (int i = 0; i < sortedBoundingBoxes.size(); i++)
    {
        ShapeBoundingBox &sortedBox = sortedBoundingBoxes[i];
        const ShapeBoundingBox &box = boundingBoxes[sortedBox.shapeIndex];
        sortedBox.x1 = box.x1;
        sortedBox.y1 = box.y1;
        sortedBox.x2 = box.x2;
        sortedBox.y2 = box.y2;
    }
}

void Game::handleCollisions(PointMassesRange &points, const Range<int> &collisionCounterForPoints, double step)
{
    // For a broad phase collision detection, sort using insertion sort along a single axis
    Array<ShapeBoundingBox> &boundingBoxes = m_impl->boundingBoxes;
    Array<ShapeBoundingBox> &staticBoundingBoxes = m_impl->staticBoundingBoxes;
    Array<ShapeBoundingBox> &sortedBoundingBoxes = m_impl->sortedBoundingBoxes;
    Array<ShapeBoundingBox> &sortedStaticBoundingBoxes = m_impl->sortedStaticBoundingBoxes;

    calculateBoundingBoxes(boundingBoxes, m_impl->shapes, m_impl->points);
    calculateBoundingBoxes(staticBoundingBoxes, m_impl->staticShapes, m_impl->staticPoints);

    if (m_impl->sortedBoundingBoxes.size() == 0)
    {
        for (int i = 0; i < boundingBoxes.size(); i++)
        {
            ShapeBoundingBox &box = boundingBoxes[i];
            Shape &shape = m_impl->shapes[box.shapeIndex];
            sortedBoundingBoxes.push(box);
        }
    }
    else
    {
        updateSortedBoundingBoxes(sortedBoundingBoxes, boundingBoxes);
    }

    if (m_impl->sortedStaticBoundingBoxes.size() == 0)
    {
        for (int i = 0; i < staticBoundingBoxes.size(); i++)
        {
            ShapeBoundingBox &box = staticBoundingBoxes[i];
            Shape &shape = m_impl->staticShapes[box.shapeIndex];
            sortedStaticBoundingBoxes.push(box);
        }
    }
    else
    {
        updateSortedBoundingBoxes(sortedStaticBoundingBoxes, staticBoundingBoxes);
    }

    sortBoundingBoxes(sortedBoundingBoxes);
    sortBoundingBoxes(sortedStaticBoundingBoxes);

    int staticIndex = 0;
    // Insertion sort - O(n^2), but since the movements are relatively stable it should be fine
    // TODO: Pre-sort using something better the first time it is run
    for (int i = 0; i < sortedBoundingBoxes.size(); i++)
    {
        const ShapeBoundingBox &box = sortedBoundingBoxes[i];
        const Shape &shape1 = m_impl->shapes[box.shapeIndex];
        Range<int> collisionRange1 = m_impl->collisionCounterForPoints.range(shape1);

        for (int j = i + 1; j < sortedBoundingBoxes.size(); j++)
        {
            const ShapeBoundingBox &otherBox = sortedBoundingBoxes[j];

            if (otherBox.x1 < box.x2)
            {
                // Due to the sorted nature of the bounding boxes, we already know that
                // otherBox.x1 >= box.x1, so we only need to check the other axis

                if (otherBox.y1 <= box.y2 && otherBox.y2 >= box.y1)
                {
                    // Check for collision
                    const Shape &shape2 = m_impl->shapes[otherBox.shapeIndex];
                    Range<int> collisionRange2 = m_impl->collisionCounterForPoints.range(shape2);
                    calculateCollisions(m_impl->points.range(shape1), m_impl->points.range(shape2), box, otherBox, collisionRange1, collisionRange2, step);
                    calculateCollisions(m_impl->points.range(shape2), m_impl->points.range(shape1), otherBox, box, collisionRange2, collisionRange1, step);
                }
            }
            else
            {
                break;
            }
        }

        while (staticIndex < sortedStaticBoundingBoxes.size() && sortedStaticBoundingBoxes[staticIndex].x2 < box.x1)
        {
            staticIndex++;
        }

        for (int j = staticIndex; j < sortedStaticBoundingBoxes.size(); j++)
        {
            const ShapeBoundingBox &staticBox = sortedStaticBoundingBoxes[j];
            if (staticBox.x1 < box.x2)
            {
                const Shape &staticShape = m_impl->staticShapes[staticBox.shapeIndex];
                Range<int> collisionRange1 = m_impl->collisionCounterForPoints.range(shape1);
                calculateStaticCollisions(m_impl->staticPoints.range(staticShape), m_impl->points.range(shape1), staticBox, box, collisionRange1, step);
                // Check for collision
            }
            else
            {
                break;
            }
        }
    }
}

void Game::loadFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file for reading");
        return;
    }

    int numPoints, numShapes, numSprings;
    // Read the number of points and shapes
    fscanf(file, "%d %d %d\n", &numPoints, &numShapes, &numSprings);

    m_impl->shapes.clear();
    m_impl->shapes.reserve(numShapes);
    m_impl->points.clear();
    m_impl->points.reserve(numPoints);

    // Read points
    for (size_t i = 0; i < numPoints; ++i)
    {
        float x, y;
        float velocityX, velocityY;
        float mass;

        fscanf(file, "%f %f %f %f %f\n", &mass, &x, &y, &velocityX, &velocityY);
        m_impl->points.pos.push({x, y});
        m_impl->points.velocity.push({velocityX, velocityY});
        m_impl->points.mass.push(mass);
        m_impl->points.acceleration.push({0.0f, 0.0f});
    }

    // Read shapes
    for (size_t i = 0; i < numShapes; ++i)
    {
        Shape shape;
        fscanf(file, "%d %d\n", &shape.start, &shape.end);
        m_impl->shapes.push(shape);
    }

    // Read springs
    for (size_t i = 0; i < numSprings; ++i)
    {
        Spring spring;
        fscanf(file, "%d %d %f %f %f\n", &spring.pointA, &spring.pointB, &spring.length, &spring.stiffness, &spring.damping);
        m_impl->springs.push(spring);
    }

    fclose(file);
}

void Game::dumpToFile(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Failed to open file for writing");
        return;
    }

    // Dump the number of points and shapes first
    fprintf(file, "%d %d %d\n", m_impl->points.size(), m_impl->shapes.size(), m_impl->springs.size());

    // Dump points
    for (size_t i = 0; i < m_impl->points.size(); ++i)
    {
        Vector2 pos = m_impl->points.pos[i];

        Vector2 velocity = m_impl->points.velocity[i];
        float mass = m_impl->points.mass[i];

        fprintf(file, "%f %f %f %f %f\n", mass, pos.x, pos.y, velocity.x, velocity.y);
    }

    // Dump shapes
    for (size_t i = 0; i < m_impl->shapes.size(); ++i)
    {
        fprintf(file, "%d %d\n", m_impl->shapes[i].start, m_impl->shapes[i].end);
    }

    // Dump springs
    for (size_t i = 0; i < m_impl->springs.size(); ++i)
    {
        fprintf(file, "%d %d %f %f %f\n", m_impl->springs[i].pointA, m_impl->springs[i].pointB, m_impl->springs[i].length, m_impl->springs[i].stiffness, m_impl->springs[i].damping);
    }

    fclose(file);
}

void Game::updateAfterRewindOrForward()
{
    m_impl->shapes.clear();
    m_impl->points.clear();
    m_impl->springs.clear();
    m_impl->staticJoints.clear();
    m_impl->staticPoints.clear();
    m_impl->staticShapes.clear();
    m_impl->shapes.append(m_impl->history[m_impl->historyRewindIndex].shapes);
    m_impl->staticShapes.append(m_impl->history[m_impl->historyRewindIndex].staticShapes);
    m_impl->points.append(m_impl->history[m_impl->historyRewindIndex].points);
    m_impl->staticPoints.append(m_impl->history[m_impl->historyRewindIndex].staticPoints);
    m_impl->springs.append(m_impl->history[m_impl->historyRewindIndex].springs);
    m_impl->staticJoints.append(m_impl->history[m_impl->historyRewindIndex].staticJoints);
}

void Game::rewindHistory()
{
    m_impl->historyRewindIndex--;
    if (m_impl->historyRewindIndex < 0)
    {
        m_impl->historyRewindIndex = NUM_HISTORICAL_STATES - 1;
    }

    updateAfterRewindOrForward();
}

void Game::forwardHistory()
{
    if (m_impl->historyRewindIndex == m_impl->historicalIndex - 1)
    {
        return;
    }

    m_impl->historyRewindIndex = (m_impl->historyRewindIndex + 1) % NUM_HISTORICAL_STATES;
    updateAfterRewindOrForward();
}

void Game::setGravityEnabled(bool gravityEnabled)
{
    m_impl->gravityEnabled = gravityEnabled;
}

void Game::setCollisionsEnabled(bool collisionsEnabled)
{
    m_impl->collisionsEnabled = collisionsEnabled;
}

const double step = 1.0f;

void Game::performRK4Step(PointMassesRange &initialState, Range<Spring> &springs, double dt, Array<PointDerivative> &derivatives, Array<PointDerivative> &outDerivatives)
{
    if (m_impl->rkTemp.pos.size() != initialState.size())
    {
        m_impl->rkTemp.pos.fill(Vector2(), initialState.size());
        m_impl->rkTemp.velocity.fill(Vector2(), initialState.size());
    }
    m_impl->rkTemp.mass.clear();
    m_impl->rkTemp.mass.append(initialState.mass);
    m_impl->rkTemp.acceleration.fill(Vector2(), initialState.size());

    outDerivatives.clear();
    PointDerivative outDerivative;

    if (m_impl->gravityEnabled)
    {
        outDerivative.acceleration = Vector2(0.0f, 0.00015f); // Gravity
    }
    outDerivatives.fill(outDerivative, initialState.size());

    Vector2 *posOut = &m_impl->rkTemp.pos[0];
    Vector2 *velOut = &m_impl->rkTemp.velocity[0];
    PointDerivative *outDerivativeOut = &outDerivatives[0];

    for (int i = 0; i < initialState.size(); i++)
    {
        PointDerivative derivative = derivatives[i];
        Vector2 originalVelocity = initialState.velocity[i];
        outDerivativeOut->velocity = originalVelocity;
        *outDerivativeOut++;
        *posOut++ = initialState.pos[i] + derivative.velocity * dt;
        *velOut++ = initialState.velocity[i] + derivative.acceleration * dt;
    }

    PointMassesRange state = m_impl->rkTemp.range();
    Range<PointDerivative> derivativeRange = outDerivatives.range();
    applySpringDerivatives(state, springs, derivativeRange);
}

void Game::performRK4Integration(
    PointMassesRange &points,
    Range<Spring> &springs,
    Range<int> &collisionCounterForPoints,
    ConsoleProfileInfo &profileInfo)
{
    if (m_impl->rkEmptyDerivatives.size() != points.size())
    {
        m_impl->rkEmptyDerivatives.fill(PointDerivative(), m_impl->points.size());
    }

    performRK4Step(points, springs, 0.0, m_impl->rkEmptyDerivatives, m_impl->rk1);
    performRK4Step(points, springs, step * 0.5, m_impl->rk1, m_impl->rk2);
    performRK4Step(points, springs, step * 0.5, m_impl->rk2, m_impl->rk3);
    performRK4Step(points, springs, step, m_impl->rk3, m_impl->rk4);

    for (int i = 0; i < m_impl->points.size(); i++)
    {
        PointDerivative &rk1 = m_impl->rk1[i];
        PointDerivative &rk2 = m_impl->rk2[i];
        PointDerivative &rk3 = m_impl->rk3[i];
        PointDerivative &rk4 = m_impl->rk4[i];

        Vector2 deltaVelocity = (rk1.velocity + (rk2.velocity + rk3.velocity) * 2.0f + rk4.velocity) * 1.0f / 6.0f;
        Vector2 deltaAcceleration = (rk1.acceleration + (rk2.acceleration + rk3.acceleration) * 2.0f + rk4.acceleration) * 1.0f / 6.0f;
        points.pos[i] += deltaVelocity * step;
        points.velocity[i] += deltaAcceleration * step;
    }

    for (int i = 0; i < m_impl->staticJoints.size(); i++)
    {
        StaticJoint &joint = m_impl->staticJoints[i];
        points.pos[joint.pointIndex] = joint.position;
        points.velocity[joint.pointIndex] = Vector2();
    }

    if (m_impl->mouseJoint.pointIndex != -1)
    {
        points.pos[m_impl->mouseJoint.pointIndex] = m_impl->mouseJoint.position;
        points.velocity[m_impl->mouseJoint.pointIndex] = Vector2();
        points.acceleration[m_impl->mouseJoint.pointIndex] = Vector2();
    }

    for (int i = 0; i < points.size(); i++)
    {
        points.acceleration[i] = Vector2();
    }

    if (m_impl->collisionsEnabled)
    {
        Timer collisionsTimer;
        handleCollisions(points, collisionCounterForPoints, step);
        profileInfo.collisionTimeMillis = collisionsTimer.elapsedMillis();
    }

    for (int i = 0; i < points.size(); i++)
    {
        points.velocity[i] += (points.acceleration[i]) * step;
        points.pos[i] += points.velocity[i] * step;
    }
}

void Game::update(double elapsedTimeMilliseconds, ConsoleProfileInfo &profileInfo)
{
    m_impl->timeBucket += elapsedTimeMilliseconds;
    profileInfo.elapsedStepTimeMillis = elapsedTimeMilliseconds;
    if (m_impl->timeBucket >= step)
    {
        Console::clearFrame();
    }

    Timer updateTimer;
    int numIterations = 0;

    while (m_impl->timeBucket > step)
    {
        PointMassesRange points = m_impl->points.range();
        Range<Spring> springs = m_impl->springs.range();
        m_impl->collisionCounterForPoints.fill(0, m_impl->points.size());
        Range<int> collisionCounterForPoints = m_impl->collisionCounterForPoints.range();

        performRK4Integration(points, springs, collisionCounterForPoints, profileInfo);

        m_impl->timeBucket -= step;
        numIterations++;

        if (updateTimer.elapsedMillis() > elapsedTimeMilliseconds)
        {
            Console::log("WARNING: Physics update cannot keep up with rendering time, dropping physics frames");
            break;
        }
    }

    profileInfo.numPhysicsSteps = numIterations;
    profileInfo.numSprings = m_impl->springs.size();
    profileInfo.physicsTimeMillis = updateTimer.elapsedMillis() / numIterations;

    m_impl->history[m_impl->historicalIndex].staticShapes.clear();
    m_impl->history[m_impl->historicalIndex].shapes.clear();
    m_impl->history[m_impl->historicalIndex].points.clear();
    m_impl->history[m_impl->historicalIndex].springs.clear();
    m_impl->history[m_impl->historicalIndex].staticJoints.clear();
    m_impl->history[m_impl->historicalIndex].shapes.append(m_impl->shapes);
    m_impl->history[m_impl->historicalIndex].staticShapes.append(m_impl->staticShapes);
    m_impl->history[m_impl->historicalIndex].points.append(m_impl->points);
    m_impl->history[m_impl->historicalIndex].springs.append(m_impl->springs);
    m_impl->history[m_impl->historicalIndex].staticJoints.append(m_impl->staticJoints);

    m_impl->historyRewindIndex = m_impl->historicalIndex;
    m_impl->historicalIndex = (m_impl->historicalIndex + 1) % NUM_HISTORICAL_STATES;
}

void Game::mouseButtonDown(int x, int y)
{
    Console::log("Mouse down: %d %d", x, y);
    for (int i = 0; i < m_impl->points.size(); i++)
    {
        Vector2 pos = m_impl->points.pos[i];
        if (Vector2::vec2distance((float)x, (float)y, pos.x, pos.y) < 20.0f)
        {
            StaticJoint joint = {i, Vector2(x, y)};
            m_impl->mouseJoint.pointIndex = i;
            m_impl->mouseJoint.position = Vector2(x, y);
            break;
        }
    }
}

void Game::mouseButtonUp(int x, int y)
{
    m_impl->mouseJoint.pointIndex = -1;
}

void Game::mouseMove(int x, int y)
{
    if (m_impl->mouseJoint.pointIndex != -1)
    {
        int i = m_impl->mouseJoint.pointIndex;
        m_impl->points.pos[i] = Vector2((float)x, (float)y);
        m_impl->points.velocity[i] = Vector2();
        m_impl->points.acceleration[i] = Vector2();
        m_impl->mouseJoint.position = Vector2((float)x, (float)y);
    }
}

Array<Shape> &Game::shapes()
{
    return m_impl->shapes;
}

Array<Shape> &Game::staticShapes()
{
    return m_impl->staticShapes;
}

PointMasses &Game::points()
{
    return m_impl->points;
}

PointMasses &Game::staticPoints()
{
    return m_impl->staticPoints;
}

Array<Spring> &Game::springs()
{
    return m_impl->springs;
}

Array<StaticJoint> &Game::staticJoints()
{
    return m_impl->staticJoints;
}

void Game::testSpringPerformance(int iterations)
{
    m_impl->rkEmptyDerivatives.fill(PointDerivative(), m_impl->points.size());
    auto points = m_impl->points.range();
    Range<Spring> springs = m_impl->springs.range();
    performRK4Step(points, springs, 0.0, m_impl->rkEmptyDerivatives, m_impl->rk1);
    auto derivativeRange = m_impl->rk1.range();
    for (int i = 0; i < iterations; i++)
    {
        applySpringDerivatives(points, springs, derivativeRange);
    }
}

void Game::testRK4Performance(int iterations)
{
    m_impl->rkEmptyDerivatives.fill(PointDerivative(), m_impl->points.size());
    auto points = m_impl->points.range();
    Range<Spring> springs = m_impl->springs.range();

    for (int i = 0; i < iterations; i++)
    {
        performRK4Step(points, springs, 0.0, m_impl->rkEmptyDerivatives, m_impl->rk1);
    }

    auto derivativeRange = m_impl->rk1.range();
    applySpringDerivatives(points, springs, derivativeRange);
    // performRK4Step(points, springs, 0.0, m_impl->rkEmptyDerivatives, m_impl->rk1);
}

void Game::clear()
{
    m_impl->clear();
}