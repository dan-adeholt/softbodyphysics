
#include "./Game.h"
#include "../utils/MinMax.h"
#include "../containers/Range.h"
#include "../containers/Array.h"
#include "../physics/Physics.h"
#include "../physics/PhysicsSpace.h"
#include "../physics/PhysicsIntegrator.h"
#include "../physics/PhysicsCollisionSolver.h"
#include <stdio.h>
#include "Shapes.h"
#include "Scenes.h"
#include <cassert>
#include <cstring>
#include "Game.h"
#include "../utils/Console.h"
#include "../timer.h"
#include "../tasks/Scheduler.h"

#define NUM_HISTORICAL_STATES 1000

struct ScheduledCallback
{
    void (*function)(Game *, void *);
    void *args;
    float delay;
};

struct Game::Impl
{
    Impl() : historicalIndex(0), historyRewindIndex(0), selectedShapeIndex(152), timeBucket(0.0), iterationNumber(0), simulationSpeed(100), paused(false), currentSceneName(nullptr), offset(0, 0), scale(1.0f), panning(false)
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

    int selectedShapeIndex;
    double timeBucket;
    int iterationNumber;
    int simulationSpeed;
    bool paused;
    const char *currentSceneName;
    Vector2 offset;
    float scale;
    bool panning;
    Array<ScheduledCallback> scheduledCallbacks;
};

Game::Game() : m(new Game::Impl)
{
}

void Game::init(const SceneDefinition &scene)
{
    clear();
    PhysicsSpace &space = physicsSpace();
    space.collisionsEnabled = true;
    space.gravityEnabled = true;
    scene.initFunc(this);
    m->currentSceneName = scene.name;
    m->timeBucket = 0.0;
}

void Game::init(const char *sceneType)
{
    for (int i = 0; i < SceneDefinition::numScenes; i++)
    {
        const SceneDefinition &def = SceneDefinition::allScenes[i];
        if (strcmp(def.name, sceneType) == 0)
        {
            init(def);
            return;
        }
    }
}

Game::~Game()
{
    delete m;
}

float terminalVelocity = 1000.0f;

void Game::updateAfterRewindOrForward()
{
    m->physicsSpace.assign(m->history[m->historyRewindIndex]);
    ConsoleProfileInfo profileInfo;
    m->collisionSolver.updateBoundingBoxes(m->physicsSpace, profileInfo);
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

    for (int i = 0; i < m->scheduledCallbacks.size(); i++)
    {
        ScheduledCallback &callback = m->scheduledCallbacks[i];
        callback.delay -= (float)elapsedTimeMilliseconds;
        if (callback.delay <= 0.0f)
        {
            callback.function(this, callback.args);
            m->scheduledCallbacks.remove(i);
            i--;
        }
    }

    profileInfo.elapsedStepTimeMillis = elapsedTimeMilliseconds;
    profileInfo.springsTimeMillis = 0;
    if (m->timeBucket >= physicsStep)
    {
        Console::clearFrame();
    }

    Timer updateTimer;
    int numIterations = 0;
    int prevStateIndex = m->historicalIndex - 1;
    if (prevStateIndex < 0)
    {
        prevStateIndex = NUM_HISTORICAL_STATES - 1;
    }
    PhysicsSpace &prevSpace = m->history[prevStateIndex];

    while (m->timeBucket > physicsStep)
    {
        m->integrator.performIntegration(m->physicsSpace, profileInfo);
        m->iterationNumber++;

        if (m->physicsSpace.collisionsEnabled && m->iterationNumber % 8 == 0)
        {
            m->collisionSolver.handleCollisions(m->physicsSpace, prevSpace, physicsStep, profileInfo);
        }

        if (m->simulationSpeed != 100)
        {
            float slowdown = 1.0f - (float)m->simulationSpeed / 100.0f;
            m->timeBucket -= (physicsStep + physicsStep * slowdown * 5.0f);
        }
        else
        {
            m->timeBucket -= physicsStep;
        }

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

void Game::mouseButtonDown(int button, int x, int y, bool shiftDown)
{
    if (button == 2)
    {
        m->panning = true;
        return;
    }

    Vector2 translatedPos = Vector2((float)x, (float)y) - m->offset;
    for (int i = 0; i < m->physicsSpace.points.size(); i++)
    {
        Vector2 pos = m->physicsSpace.points.pos[i];
        if (Vector2::vec2distance(translatedPos.x, translatedPos.y, pos.x, pos.y) < 20.0f)
        {
            m->physicsSpace.mouseJoint.pointIndex = i;
            m->physicsSpace.mouseJoint.position = translatedPos;

            if (shiftDown)
            {
                for (int j = 0; j < m->physicsSpace.shapes.size(); j++)
                {
                    const Shape &shape = m->physicsSpace.shapes[j];

                    if (i >= shape.start && i < shape.end)
                    {
                        m->selectedShapeIndex = j;
                        break;
                    }
                }
            }
            return;
        }
    }

    int test = m->collisionSolver.boundingBoxes.size();

    int testedBoxes = 0;
    for (int i = 0; i < m->collisionSolver.boundingBoxes.size(); i++)
    {

        ShapeBoundingBox &box = m->collisionSolver.boundingBoxes[i];
        if (box.includes(translatedPos))
        {

            Shape &shape = m->physicsSpace.shapes[box.shapeIndex];
            if (shape.subShapeSpan.isValid())
            {
                Range<ShapeQuad> subshape = m->physicsSpace.partialShapes.range(shape.subShapeSpan);

                for (int j = 0; j < subshape.size; j++)
                {
                    ShapeQuad &quad = subshape[j];
                    if (quad.isPointInQuad(translatedPos))
                    {
                        if (shiftDown)
                        {
                            m->selectedShapeIndex = box.shapeIndex;
                        }

                        m->physicsSpace.draggingShapeIndex = box.shapeIndex;
                        m->physicsSpace.draggingSubShapeIndex = j;
                        break;
                    }
                }
            }
            else
            {
                if (shiftDown)
                {
                    m->selectedShapeIndex = box.shapeIndex;
                }

                m->physicsSpace.draggingShapeIndex = box.shapeIndex;
                break;
            }
        }

        testedBoxes++;
    }

    Console::log("Tested %d boxes", testedBoxes);
}

void Game::mouseWheel(int x, int y)
{
    float zoomFactor = y > 0 ? 0.9f : 1.1f;
    Vector2 mousePos((float)x, (float)y);
    Vector2 beforeZoom = (mousePos - m->offset) / m->scale;
    m->scale *= zoomFactor;
    Vector2 afterZoom = (mousePos - m->offset) / m->scale;
    m->offset += (afterZoom - beforeZoom) * m->scale;
}

Array<ShapeBoundingBox> &Game::shapeBoundingBoxes()
{
    return m->collisionSolver.boundingBoxes;
}

void Game::mouseButtonUp(int button, int x, int y, bool shiftDown)
{
    if (button == 2)
    {
        m->panning = false;
        return;
    }

    m->physicsSpace.mouseJoint.pointIndex = -1;
    m->physicsSpace.draggingShapeIndex = -1;
    m->physicsSpace.draggingSubShapeIndex = -1;
}

void Game::mouseMove(int x, int y, int relativeX, int relativeY)
{
    if (m->panning)
    {
        m->offset.x += relativeX;
        m->offset.y += relativeY;
        return;
    }

    if (m->physicsSpace.mouseJoint.pointIndex != -1)
    {
        int i = m->physicsSpace.mouseJoint.pointIndex;
        m->physicsSpace.points.pos[i] = Vector2((float)x, (float)y);
        m->physicsSpace.points.velocity[i] = Vector2();
        m->physicsSpace.mouseJoint.position = Vector2((float)x, (float)y);
    }

    if (m->physicsSpace.draggingShapeIndex != -1)
    {
        Shape &shape = m->physicsSpace.shapes[m->physicsSpace.draggingShapeIndex];
        Vector2 delta((float)relativeX, (float)relativeY);

        if (shape.subShapeSpan.isValid())
        {
            if (m->physicsSpace.draggingSubShapeIndex != -1)
            {
                Range<ShapeQuad> subshape = m->physicsSpace.partialShapes.range(shape.subShapeSpan);

                ShapeQuad &quad = subshape[m->physicsSpace.draggingSubShapeIndex];
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

const char *Game::currentSceneName()
{
    return m->currentSceneName;
}

int &Game::simulationSpeed()
{
    return m->simulationSpeed;
}

void Game::setPaused()
{
    m->paused = true;
    updateBoundingBoxes();
}

void Game::togglePaused()
{
    m->paused = !m->paused;
    updateBoundingBoxes();
}

bool Game::paused()
{
    return m->paused;
}

void Game::updateBoundingBoxes()
{
    ConsoleProfileInfo profileInfo;
    m->collisionSolver.updateBoundingBoxes(m->physicsSpace, profileInfo);
}

void Game::clear()
{
    m->clear();
}

Vector2 &Game::offset()
{
    return m->offset;
}

float &Game::scale()
{
    return m->scale;
}

void Game::scheduleCallback(void (*function)(Game *, void *), void *args, float delay)
{
    m->scheduledCallbacks.push({function, args, delay});
}

int Game::selectedShapeIndex() const
{
    return m->selectedShapeIndex;
}
