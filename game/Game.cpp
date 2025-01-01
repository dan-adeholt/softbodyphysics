
#include "./Game.h"
#include "../utils/MinMax.h"
#include "../containers/Range.h"
#include "../containers/Array.h"
#include "../physics/Physics.h"
#include "../physics/PhysicsSpace.h"
#include "../physics/PhysicsSpaceStorage.h"
#include "../physics/Integrator.h"
#include "../physics/CollisionSolver.h"
#include <stdio.h>
#include "Shapes.h"
#include "Scenes.h"
#include <cassert>
#include <cstring>
#include "Game.h"
#include "../utils/Console.h"
#include "../timer.h"
#include "../tasks/Scheduler.h"
#include "GameKeyCode.h"

#define NUM_HISTORICAL_STATES 1000

struct ScheduledCallback
{
    void (*function)(Game *, void *);
    void *args;
    float delay;
};

struct FrameCallback
{
    void (*function)(Game *, void *);
    void *args;
};

struct Game::Impl
{
    Impl(const char *path)
    {
        if (path)
        {
            strncpy(this->path, path, sizeof(this->path) - 1);
        }
        else
        {
            this->path[0] = '\0';
        }
    }

    void clear()
    {
        physicsSpace.clear();
        collisionSolver.clear();
    }

    PhysicsSpace history[NUM_HISTORICAL_STATES];
    PhysicsSpace lastCollisionSpace;
    bool hasLastCollisionSpace = false;
    int historicalIndex = 0;
    int historyRewindIndex = 0;

    PhysicsSpace physicsSpace;
    CollisionSolver collisionSolver;
    Integrator integrator;

    int selectedShapeIndex = 0;
    double timeBucket = 0.0;
    int iterationNumber = 0;
    int simulationSpeed = 100;
    bool paused = false;
    const char *currentSceneName = nullptr;
    Vector2 offset;
    float scale = 1.0f;
    bool panning = false;
    Array<ScheduledCallback> scheduledCallbacks;
    Array<FrameCallback> frameCallbacks;

    bool stopPointWhenDragging = true;
    bool quit = false;
    char path[512] = {};
    bool keyState[(size_t)GameKeyCode::NUM_KEY_CODES] = {};
    bool keyPressedState[(size_t)GameKeyCode::NUM_KEY_CODES] = {};
    int modKeyState = 0;
    GameRenderSettings renderSettings;
};

Game::Game(const char *path) : m(new Game::Impl(path))
{
}

float maxLength = -10000.0f;

void Game::init(const SceneDefinition &scene)
{
    clear();
    PhysicsSpace &space = physicsSpace();
    space.collisionsEnabled = true;
    space.gravityEnabled = true;
    space.springsEnabled = true;
    m->hasLastCollisionSpace = false;
    m->renderSettings = GameRenderSettings();
    m->stopPointWhenDragging = true;
    m->frameCallbacks.clear();
    m->scale = 1.0f;
    m->offset = Vector2(400, 0);
    m->paused = false;
    scene.initFunc(this);
    m->currentSceneName = scene.name;
    m->timeBucket = 0.0;
    maxLength = -100000.0f;
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

void Game::update(double elapsedTimeMilliseconds, bool singleStep, ConsoleProfileInfo &profileInfo)
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
    if (m->timeBucket >= physicsStep || singleStep)
    {
        Console::clearFrame();
    }

    int numIterations = 0;
    int prevStateIndex = m->historicalIndex - 1;
    if (prevStateIndex < 0)
    {
        prevStateIndex = NUM_HISTORICAL_STATES - 1;
    }

    bool runSingleStep = singleStep;
    double maxDuration = physicsStep * 0.5f;

    Timer updateTimer;

    while (m->timeBucket > physicsStep || runSingleStep)
    {
        profileInfo.slowdownFactor = 1.0;

        Timer stepTimer;

        m->integrator.performIntegration(m->physicsSpace, profileInfo);
        m->iterationNumber++;
        bool runCollisionSolver = m->physicsSpace.collisionsEnabled; // && m->iterationNumber % 8 == 0;

        if (runCollisionSolver)
        {
            // Console::clearCollisionFrame();
            if (m->hasLastCollisionSpace)
            {
                m->collisionSolver.handleCollisions(m->physicsSpace, m->lastCollisionSpace, profileInfo);
            }

            m->hasLastCollisionSpace = true;
            // Console::log("Max length: %f", maxLength);

            m->lastCollisionSpace.assign(m->physicsSpace);
        }

        if (Console::isDebugger())
        {
            Console::setDebugger(false);
            setPaused();
            break;
        }

        double slowdownFactor = 1.0f;

        double ratio = stepTimer.elapsedMillis() / maxDuration;
        if (ratio > 1.0f)
        {
            slowdownFactor = ratio;
            profileInfo.slowdownFactor = 1.0f / slowdownFactor;
        }

        if (m->simulationSpeed != 100)
        {
            float slowdown = 1.0f - (float)m->simulationSpeed / 100.0f;
            m->timeBucket -= (physicsStep + physicsStep * slowdown * 5.0f);
        }
        else
        {
            m->timeBucket -= physicsStep * slowdownFactor;
        }

        numIterations++;

        // Run until we've done a collision pass
        if (runSingleStep && runCollisionSolver)
        {
            runSingleStep = false;
            break;
        }
    }

    int nextHistoricalIndex = (m->historicalIndex + 1) % NUM_HISTORICAL_STATES;

    profileInfo.numPhysicsSteps = numIterations;
    profileInfo.numSprings = m->physicsSpace.springs.size();
    profileInfo.physicsTimeMillis = updateTimer.elapsedMillis() / numIterations;
    profileInfo.springsTimeMillis /= numIterations;

    m->history[m->historicalIndex].assign(m->physicsSpace);
    m->historyRewindIndex = m->historicalIndex;
    m->historicalIndex = nextHistoricalIndex;

    for (int i = 0; i < m->frameCallbacks.size(); i++)
    {
        FrameCallback &callback = m->frameCallbacks[i];
        callback.function(this, callback.args);
    }

    for (int i = 0; i < (int)GameKeyCode::NUM_KEY_CODES; i++)
    {
        m->keyPressedState[i] = false;
    }
}

void Game::mouseButtonDown(int button, int x, int y, bool shiftDown)
{
    if (button == 2)
    {
        m->panning = true;
        return;
    }

    Vector2 translatedPos = (Vector2((float)x, (float)y) - m->offset) / m->scale;
    for (int i = 0; i < m->physicsSpace.points.size(); i++)
    {
        Vector2 pos = m->physicsSpace.points.pos[i];
        if (Vector2::vec2distance(translatedPos.x, translatedPos.y, pos.x, pos.y) < (20.0f / m->scale))
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

    Array<ShapeBoundingBox> &boxes = m->collisionSolver.boundingBoxes();
    int test = boxes.size();

    int testedBoxes = 0;
    for (int i = 0; i < boxes.size(); i++)
    {
        ShapeBoundingBox &box = boxes[i];
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
}

void Game::mouseWheel(int x, int y)
{
    float zoomFactor = y > 0 ? 0.9f : 1.1f;
    Vector2 mousePos((float)x, (float)y);
    Vector2 beforeZoom = (mousePos - m->offset) / m->scale;
    m->scale *= zoomFactor;
    Vector2 afterZoom = (mousePos - m->offset) / m->scale;
    m->offset += (afterZoom - beforeZoom) * m->scale;

    Console::log("Zoom factor: %.2f => %.2f %.2f", m->scale, m->offset.x, m->offset.y);
}

void Game::keyDown(GameKeyCode keyCode, int modState, ConsoleProfileInfo &profileInfo)
{
    m->keyState[(size_t)keyCode] = true;
    m->keyPressedState[(size_t)keyCode] = true;
    m->modKeyState = modState;

    switch (keyCode)
    {
    case GameKeyCode::BACKSPACE:
        m->physicsSpace.removeShape(m->selectedShapeIndex);
        updateBoundingBoxes(true);
        break;
    case GameKeyCode::F1:
        PhysicsSpaceStorage::dumpToUnitTest(m->physicsSpace);
        break;
    case GameKeyCode::F2:
        physicsSpace().shapeMatchingEnabled = !physicsSpace().shapeMatchingEnabled;
        break;
    case GameKeyCode::F5:
        togglePaused();
        break;
    case GameKeyCode::F8:
        update(1000.0 / 120.0, true, profileInfo);
        updateBoundingBoxes();
        break;
    case GameKeyCode::F3:
        rewindHistory();
        break;
    case GameKeyCode::F4:
        forwardHistory();
        break;
    case GameKeyCode::F7:
    case GameKeyCode::F6:
    {
        char buf[500];
        snprintf(buf, sizeof(buf), "%s%s", m->path, "dump.txt");

        if (keyCode == GameKeyCode::F6)
        {
            PhysicsSpaceStorage::dumpToFile(physicsSpace(), buf);
            printf("Wrote to %s\n", buf);
        }
        else
        {
            printf("Attempting read from %s\n", buf);
            PhysicsSpaceStorage::loadFromFile(physicsSpace(), buf);
            printf("Read from %s\n", buf);
        }
    }
    break;
    case GameKeyCode::R:
        maxLength = -10000.0f;
        break;
    case GameKeyCode::ESCAPE:
        setShouldQuit();
        break;
    default:
        break;
    }
}

void Game::keyUp(GameKeyCode keyCode, int modState, ConsoleProfileInfo &profileInfo)
{
    m->keyState[(size_t)keyCode] = false;
    m->keyPressedState[(size_t)keyCode] = false;
    m->modKeyState = modState;
}

bool Game::keyWasPressed(GameKeyCode keyCode)
{
    return m->keyPressedState[(size_t)keyCode];
}

Array<ShapeBoundingBox> &Game::shapeBoundingBoxes()
{
    return m->collisionSolver.boundingBoxes();
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

    Vector2 translatedPos = (Vector2((float)x, (float)y) - m->offset) / m->scale;

    if (m->physicsSpace.mouseJoint.pointIndex != -1)
    {
        int i = m->physicsSpace.mouseJoint.pointIndex;
        m->physicsSpace.points.pos[i] = translatedPos;

        if (m->stopPointWhenDragging)
        {
            m->physicsSpace.points.velocity[i] = Vector2();
        }

        m->physicsSpace.mouseJoint.position = translatedPos;
    }

    float translatedRelativeX = (float)relativeX / m->scale;
    float translatedRelativeY = (float)relativeY / m->scale;

    if (m->physicsSpace.draggingShapeIndex != -1)
    {
        Shape &shape = m->physicsSpace.shapes[m->physicsSpace.draggingShapeIndex];
        Vector2 delta(translatedRelativeX, translatedRelativeY);

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

PhysicsSpace &Game::lastCollisionSpace()
{
    return m->lastCollisionSpace;
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

    if (m->paused)
    {
        Scheduler::instance->stop();
    }
    else
    {
        Scheduler::instance->start();
    }

    updateBoundingBoxes();
}

bool Game::paused()
{
    return m->paused;
}

void Game::updateBoundingBoxes(bool clear)
{
    ConsoleProfileInfo profileInfo;
    if (clear)
    {
        m->collisionSolver.clear();
    }

    m->collisionSolver.updateBoundingBoxes(m->physicsSpace, profileInfo);
}

void Game::setShouldQuit()
{
    m->quit = true;
}

bool Game::shouldQuit()
{
    return m->quit;
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

void Game::scheduleFrameCallback(void (*function)(Game *, void *), void *args)
{
    m->frameCallbacks.push({function, args});
}

int Game::selectedShapeIndex() const
{
    return m->selectedShapeIndex;
}

void Game::runFor(int timeMillis, bool pauseAfter)
{
    double elapsedTime = 0.0;
    while (elapsedTime < timeMillis)
    {
        double stepTime = min(timeMillis - elapsedTime, 16.0);
        ConsoleProfileInfo profileInfo;
        update(stepTime, false, profileInfo);
        elapsedTime += stepTime;
    }
    if (pauseAfter)
    {
        setPaused();
    }
}

void Game::setStopPointWhenDragging(bool stopPointWhenDragging)
{
    m->stopPointWhenDragging = stopPointWhenDragging;
}

GameRenderSettings Game::renderSettings()
{
    return m->renderSettings;
}

void Game::setRenderSettings(const GameRenderSettings &settings)
{
    m->renderSettings = settings;
}
