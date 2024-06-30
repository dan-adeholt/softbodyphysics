
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

#define NUM_HISTORICAL_STATES 240
#define NUM_SHAPES 40
#define NUM_POINTS 1024

struct HistoricalState
{
    Array<Shape> shapes;
    Array<PointMass> staticPoints;
    Array<PointMass> points;
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
        ;
        gravityEnabled = true;
    }

    HistoricalState history[NUM_HISTORICAL_STATES];
    int historicalIndex;
    int historyRewindIndex;
    Array<Shape> shapes;
    Array<PointMass> points;
    Array<Shape> staticShapes;
    Array<PointMass> staticPoints;
    Array<Spring> springs;
    Array<StaticJoint> staticJoints;
    Array<int> collisionCounterForPoints;
    Array<ShapeBoundingBox> boundingBoxes;
    Array<ShapeBoundingBox> staticBoundingBoxes;
    Array<ShapeBoundingBox> sortedBoundingBoxes;
    Array<ShapeBoundingBox> sortedStaticBoundingBoxes;
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

void Game::getDynamicPoints(Range<PointMass> &pointMasses) const
{
    pointMasses = m_impl->points.range(0, m_impl->points.size());
}

void Game::getStaticPoints(Range<PointMass> &pointMasses) const
{
    pointMasses = m_impl->staticPoints.range(0, m_impl->staticPoints.size());
}

void Game::handleGravity(Range<PointMass> &points, double elapsedTimeMilliseconds)
{
    if (m_impl->gravityEnabled)
    {
        // Apply gravity
        for (int i = 0; i < points.size; i++)
        {
            points[i].force.y += 1.0f * points[i].mass;
        }
    }
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

void Game::handleCollisions(Range<PointMass> &points, const Range<int> &collisionCounterForPoints)
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
                    calculateCollisions(m_impl->points.range(shape1), m_impl->points.range(shape2), box, otherBox, collisionRange1, collisionRange2);
                    calculateCollisions(m_impl->points.range(shape2), m_impl->points.range(shape1), otherBox, box, collisionRange2, collisionRange1);
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
                calculateStaticCollisions(m_impl->staticPoints.range(staticShape), m_impl->points.range(shape1), staticBox, box, collisionRange1);
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
        PointMass point;
        fscanf(file, "%f %f %f %f %f\n", &point.mass, &point.pos.x, &point.pos.y, &point.velocity.x, &point.velocity.y);
        m_impl->points.push(point);
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
        const PointMass &point = m_impl->points[i];
        fprintf(file, "%f %f %f %f %f\n", point.mass, point.pos.x, point.pos.y, point.velocity.x, point.velocity.y);
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

double step = 0.05f;

void Game::update(double elapsedTimeMilliseconds)
{
    m_impl->timeBucket += elapsedTimeMilliseconds;
    if (m_impl->timeBucket >= step)
    {
        Console::clearFrame();
    }

    while (m_impl->timeBucket > step)
    {
        Range<PointMass> points = m_impl->points.range();
        Range<Spring> springs = m_impl->springs.range();
        m_impl->collisionCounterForPoints.fill(0, m_impl->points.size());
        Range<int> collisionCounterForPoints = m_impl->collisionCounterForPoints.range();

        for (int i = 0; i < m_impl->points.size(); i++)
        {
            PointMass &point = m_impl->points[i];
            point.force = Vector2::zero();
        }

        if (m_impl->gravityEnabled)
        {
            applyGravity(points);
        }

        applySprings(points, springs, collisionCounterForPoints);

        for (int i = 0; i < m_impl->staticJoints.size(); i++)
        {
            StaticJoint &joint = m_impl->staticJoints[i];
            PointMass &point = m_impl->points[joint.pointIndex];
            point.pos = joint.position;
            point.velocity = Vector2::zero();
        }

        if (m_impl->mouseJoint.pointIndex != -1)
        {
            PointMass &point = m_impl->points[m_impl->mouseJoint.pointIndex];
            point.pos = m_impl->mouseJoint.position;
            point.velocity = Vector2::zero();
            point.force = Vector2::zero();
        }

        if (m_impl->collisionsEnabled)
        {
            handleCollisions(points, collisionCounterForPoints);
        }

        for (int i = 0; i < m_impl->points.size(); i++)
        {
            PointMass &point = m_impl->points[i];
            point.velocity += (point.force / point.mass) * 0.0001f;
            point.pos.x += point.velocity.x;
            point.pos.y += point.velocity.y;
        }

        m_impl->timeBucket -= step;
    }

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
        PointMass &point = m_impl->points[i];
        if (point.pos.distance(Vector2(x, y)) < 10.0f)
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
        PointMass &point = m_impl->points[m_impl->mouseJoint.pointIndex];
        point.pos = Vector2(x, y);
        point.velocity = Vector2::zero();
        point.force = Vector2::zero();
        m_impl->mouseJoint.position = Vector2(x, y);
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

Array<PointMass> &Game::points()
{
    return m_impl->points;
}

Array<PointMass> &Game::staticPoints()
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

void Game::clear()
{
    m_impl->clear();
}