
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
#include "../tasks/Scheduler.h"

#define NUM_HISTORICAL_STATES 240

struct Game::Impl
{
    Impl() : historicalIndex(0), historyRewindIndex(0), timeBucket(0.0)
    {
    }

    void clear()
    {
        physicsSpace.clear();
        collisionSolver.clear();
    }

    PhysicsSpace history[NUM_HISTORICAL_STATES];
    int historicalIndex;
    int historyRewindIndex;

    PhysicsSpace physicsSpace;
    PhysicsCollisionSolver collisionSolver;
    PhysicsIntegrator integrator;

    double timeBucket;
};

Game::Game() : m(new Game::Impl)
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
    delete m;
}

float terminalVelocity = 1000.0f;

void Game::updateAfterRewindOrForward()
{
    m->physicsSpace.assign(m->history[m->historyRewindIndex]);
}

void Game::rewindHistory()
{
    m->historyRewindIndex--;
    if (m->historyRewindIndex < 0)
    {
        m->historyRewindIndex = NUM_HISTORICAL_STATES - 1;
    }

    updateAfterRewindOrForward();
}

void Game::forwardHistory()
{
    if (m->historyRewindIndex == m->historicalIndex - 1)
    {
        return;
    }

    m->historyRewindIndex = (m->historyRewindIndex + 1) % NUM_HISTORICAL_STATES;
    updateAfterRewindOrForward();
}

void Game::update(double elapsedTimeMilliseconds, ConsoleProfileInfo &profileInfo)
{
    m->timeBucket += elapsedTimeMilliseconds;
    profileInfo.elapsedStepTimeMillis = elapsedTimeMilliseconds;
    if (m->timeBucket >= physicsStep)
    {
        Console::clearFrame();
    }

    Timer updateTimer;
    int numIterations = 0;

    while (m->timeBucket > physicsStep)
    {
        m->integrator.performIntegration(m->physicsSpace, profileInfo);

        bool updateCollisions = numIterations % 8 == 0;

        if (m->physicsSpace.collisionsEnabled && updateCollisions)
        {
            m->collisionSolver.handleCollisions(m->physicsSpace, physicsStep, profileInfo);
        }

        m->timeBucket -= physicsStep;
        numIterations++;

        if (updateTimer.elapsedMillis() > elapsedTimeMilliseconds)
        {
            Console::log("WARNING: Physics update cannot keep up with rendering time, dropping physics frames");
            break;
        }
    }

    profileInfo.numPhysicsSteps = numIterations;
    profileInfo.numSprings = m->physicsSpace.springs.size();
    profileInfo.physicsTimeMillis = updateTimer.elapsedMillis() / numIterations;
    profileInfo.springsTimeMillis /= numIterations;

    m->history[m->historicalIndex].assign(m->physicsSpace);

    m->historyRewindIndex = m->historicalIndex;
    m->historicalIndex = (m->historicalIndex + 1) % NUM_HISTORICAL_STATES;
}

void Game::mouseButtonDown(int x, int y)
{
    for (int i = 0; i < m->physicsSpace.points.size(); i++)
    {
        Vector2 pos = m->physicsSpace.points.pos[i];
        if (Vector2::vec2distance((float)x, (float)y, pos.x, pos.y) < 20.0f)
        {
            m->physicsSpace.mouseJoint.pointIndex = i;
            m->physicsSpace.mouseJoint.position = Vector2(x, y);
            return;
        }
    }

    for (int i = 0; i < m->collisionSolver.boundingBoxes.size(); i++)
    {
        ShapeBoundingBox &box = m->collisionSolver.boundingBoxes[i];
        if (box.includes(Vector2((float)x, (float)y)))
        {

            Shape &shape = m->physicsSpace.shapes[box.shapeIndex];
            if (shape.subShapes.isValid())
            {
                for (int j = 0; j < shape.subShapes.size; j++)
                {
                    ShapeQuad &quad = shape.subShapes[j];
                    if (quad.isPointInQuad(Vector2((float)x, (float)y)))
                    {
                        m->physicsSpace.draggingShapeIndex = box.shapeIndex;
                        m->physicsSpace.draggingSubShapeIndex = j;
                        break;
                    }
                }
            }
            else
            {
                m->physicsSpace.draggingShapeIndex = box.shapeIndex;
                break;
            }
        }
    }
}

void Game::mouseButtonUp(int x, int y)
{
    m->physicsSpace.mouseJoint.pointIndex = -1;
    m->physicsSpace.draggingShapeIndex = -1;
    m->physicsSpace.draggingSubShapeIndex = -1;
}

void Game::mouseMove(int x, int y, int relativeX, int relativeY)
{
    if (m->physicsSpace.mouseJoint.pointIndex != -1)
    {
        int i = m->physicsSpace.mouseJoint.pointIndex;
        m->physicsSpace.points.pos[i] = Vector2((float)x, (float)y);
        m->physicsSpace.points.velocity[i] = Vector2();
        m->physicsSpace.points.acceleration[i] = Vector2();
        m->physicsSpace.mouseJoint.position = Vector2((float)x, (float)y);
    }

    if (m->physicsSpace.draggingShapeIndex != -1)
    {
        Shape &shape = m->physicsSpace.shapes[m->physicsSpace.draggingShapeIndex];
        Vector2 delta((float)relativeX, (float)relativeY);

        if (shape.subShapes.isValid())
        {
            if (m->physicsSpace.draggingSubShapeIndex != -1)
            {
                ShapeQuad &quad = shape.subShapes[m->physicsSpace.draggingSubShapeIndex];
                for (int i = 0; i < quad.size; i++)
                {
                    quad.shapePos[i] += delta;
                }
            }
        }
        else
        {
            for (int i = shape.start; i < shape.end; i++)
            {
                m->physicsSpace.points.shapePos[i] += delta;
            }
        }
    }
}

PhysicsSpace &Game::physicsSpace()
{
    return m->physicsSpace;
}

void Game::clear()
{
    m->clear();
}