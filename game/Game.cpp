
#include "./Game.h"
#include "../utils/MinMax.h"
#include "../containers/Range.h"
#include "../containers/Array.h"
#include "../physics/Physics.h"
#include "../physics/PhysicsSpace.h"
#include "../physics/PhysicsSpaceStorage.h"
#include "../physics/Integrator.h"
#include "../physics/ShapeUtils.h"
#include "../physics/CollisionSolver.h"
#include "../containers/StringBuffer.h"
#include "SceneScript.h"
#include <stdio.h>
#include <math.h>
#include "Shapes.h"
#include "Scenes.h"
#include <assert.h>
#include <string.h>
#include "Game.h"
#include "../utils/Console.h"
#include "../timer.h"
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
    Impl(const char *appPathArg, const char *filePathArg)
    {
        appPath.append(appPathArg);
        filePath.append(filePathArg);
    }

    void clear()
    {
        physicsSpace.clear();
        collisionSolver.clear();
        integrator.clear();
    }

    // How the scene's script has it seen, if at an angle
    const ViewProjection *projection() const
    {
        return script != nullptr ? script->viewProjection() : nullptr;
    }

    // Where the mouse is over the scene. In a scene seen at an angle, at the height things are grabbed at, so
    // a ball standing on the ground is grabbed by its body rather than by the ground behind it.
    Vector2 translatedMousePos()
    {
        const Vector2 view = (mousePos - offset) / scale;
        const ViewProjection *viewProjection = projection();
        return viewProjection != nullptr ? viewProjection->toGround(view, viewProjection->pickHeight) : view;
    }

    // Where the mouse is on the ground itself
    Vector2 groundMousePos()
    {
        const Vector2 view = (mousePos - offset) / scale;
        const ViewProjection *viewProjection = projection();
        return viewProjection != nullptr ? viewProjection->toGround(view) : view;
    }

    // A mouse move, in screen pixels, as a move over the scene
    Vector2 translatedMouseMove(float relativeX, float relativeY)
    {
        const Vector2 view = Vector2(relativeX, relativeY) / scale;
        const ViewProjection *viewProjection = projection();
        return viewProjection != nullptr ? viewProjection->toGroundMove(view) : view;
    }

    // Scene scripts keep the indices of the shapes and points they made. When something else changes the
    // shapes, as when one is deleted or pasted, a level is loaded or history is rewound to before a shape was
    // added, those indices go stale, so the script ends and the scene carries on without it.
    void rememberScriptLayout()
    {
        scriptShapeCount = physicsSpace.shapes.size();
        scriptPointCount = physicsSpace.points.size();
    }

    void endScriptIfLayoutChanged()
    {
        if (script != nullptr && (physicsSpace.shapes.size() != scriptShapeCount || physicsSpace.points.size() != scriptPointCount))
        {
            delete script;
            script = nullptr;
            Console::log("The scene's shapes were changed, so its script has stopped");
        }
    }

    // The selected shape, or -1. The stored index can outlive the shape, as when shapes are deleted or a
    // smaller scene or level is loaded.
    int selectedShape() const
    {
        return selectedShapeIndex >= 0 && selectedShapeIndex < physicsSpace.shapes.size() ? selectedShapeIndex : -1;
    }

    // The point within a few pixels of pos, which a click grabs on its own, or -1
    int pointAt(Vector2 pos)
    {
        for (int i = 0; i < physicsSpace.points.size(); i++)
        {
            Vector2 pointPos = physicsSpace.points.pos[i];
            if (Vector2::vec2distance(pos.x, pos.y, pointPos.x, pointPos.y) < (4.0f / scale))
            {
                return i;
            }
        }

        return -1;
    }

    int shapeOfPoint(int pointIndex)
    {
        for (int j = 0; j < physicsSpace.shapes.size(); j++)
        {
            const Shape &shape = physicsSpace.shapes[j];

            if (pointIndex >= shape.start && pointIndex < shape.end)
            {
                return j;
            }
        }

        return -1;
    }

    // The shape that pos is inside, which a click drags as a whole, or -1
    int shapeAt(Vector2 pos)
    {
        Array<ShapeBoundingBox> &boxes = collisionSolver.boundingBoxes();

        for (int i = 0; i < boxes.size(); i++)
        {
            ShapeBoundingBox &box = boxes[i];
            if (box.includes(pos))
            {
                Shape &shape = physicsSpace.shapes[box.shapeIndex];

                if (!CollisionSolver::isPointOutsideShape(-1, pos.x, pos.y, box, physicsSpace.points.range(), shape))
                {
                    return box.shapeIndex;
                }
            }
        }

        return -1;
    }

    PhysicsSpace history[NUM_HISTORICAL_STATES];
    int historicalIndex = 0;
    int historyRewindIndex = 0;

    PhysicsSpace physicsSpace;
    CollisionSolver collisionSolver;
    Integrator integrator;

    int selectedShapeIndex = -1;
    double timeBucket = 0.0;
    double lastElapsedTimeMilliseconds = 0.0;
    int iterationNumber = 0;
    int simulationSpeed = 100;
    bool paused = false;
    bool debugDraw = false;
    bool antiAliasing = true;
    const char *currentSceneName = nullptr;
    Vector2 offset;
    float scale = 1.0f;
    bool panning = false;
    Array<ScheduledCallback> scheduledCallbacks;
    Array<FrameCallback> frameCallbacks;
    Array<int> collisionCandidates;

    bool stopPointWhenDragging = true;
    bool quit = false;
    StringBuffer<512> appPath;
    StringBuffer<512> filePath;

    bool keyState[(size_t)GameKeyCode::NUM_KEY_CODES] = {};
    bool keyPressedState[(size_t)GameKeyCode::NUM_KEY_CODES] = {};
    int modKeyState = 0;
    GameRenderSettings renderSettings;
    StringBuffer<512> title;
    ShapeMatchDragData shapeMatchDragData;
    int copyShapeIndex = -1;
    Vector2 mousePos;
    bool fitViewRequested = false;
    AddSubShapeData addSubshapeData;
    int hoveredShapeIndex = -1;
    bool draggingOverEmptySpace = false; // Since a left click that didn't grab anything, until it is released
    SceneScript *script = nullptr;
    int scriptShapeCount = 0;
    int scriptPointCount = 0;
};

Game::Game(const char *appPath, const char *filePath) : m(new Game::Impl(appPath, filePath))
{
    m->title.append(filePath);
}

float maxLength = -10000.0f;

void Game::init(const SceneDefinition &scene)
{
    clear();
    PhysicsSpace &space = physicsSpace();
    space.collisionsEnabled = true;
    space.gravityEnabled = true;
    space.springsEnabled = true;
    space.shapeMatchingEnabled = true;

    m->shapeMatchDragData = ShapeMatchDragData();
    m->renderSettings = GameRenderSettings();
    m->stopPointWhenDragging = true;
    m->frameCallbacks.clear();
    delete m->script;
    m->script = nullptr;
    m->draggingOverEmptySpace = false;
    m->selectedShapeIndex = -1;
    m->copyShapeIndex = -1;
    m->scale = 1.0f;
    m->offset = Vector2(400, 0);
    m->historicalIndex = 0;
    m->paused = false;
    m->iterationNumber = 0;
    scene.initFunc(this);
    m->rememberScriptLayout();
    m->currentSceneName = scene.name;
    m->timeBucket = 0.0;
    m->title.clear();
    m->title.append(scene.name);
    maxLength = -100000.0f;
}

void Game::init(const char *sceneType)
{
    const SceneDefinition *def = SceneDefinition::getDefinitionFromName(sceneType);
    if (def != nullptr)
    {
        init(*def);
        return;
    }
}

Game::~Game()
{
    delete m->script;
    delete m;
}

float terminalVelocity = 1000.0f;

const char *Game::title() const
{
    return m->title.data;
}

const AddSubShapeData &Game::addSubShapeData() const
{
    return m->addSubshapeData;
}

const Vector2 &Game::mousePos() const
{
    return m->mousePos;
}

CollisionGridSimple &Game::collisionGrid() const
{
    return m->collisionSolver.grid();
}

Range<int> Game::collisionCandidates() const
{

    return m->collisionCandidates.range();
}

void Game::testCollisionPerformance(ConsoleProfileInfo &profileInfo, int numIterations) const
{
    {
        Timer timer;
        int rejects = m->collisionSolver.testBoundingBoxesPerformance(m->physicsSpace, profileInfo, numIterations);
        printf("** Bbox run time: %.1lf ms\t => %d\n", timer.elapsedMillis(), rejects);
    }

    {
        Timer timer;
        int rejects = m->collisionSolver.testAlignedBoundingBoxesPerformance(m->physicsSpace, profileInfo, numIterations);
        printf("** OABB boxes run time: %.1lf ms => %d\n", timer.elapsedMillis(), rejects);
    }

    {
        Timer timer;
        int rejects = m->collisionSolver.testKdopPerformance(m->physicsSpace, profileInfo, numIterations);
        printf("** KDop run time: %.1lf ms => %d\n", timer.elapsedMillis(), rejects);
    }

    {
        Timer timer;
        int rejects = m->collisionSolver.testSimpleOverlapPerformance(m->physicsSpace, profileInfo, numIterations);
        printf("** Simple overlap run time: %.1lf ms\t=> %d\n", timer.elapsedMillis(), rejects);
    }

    {
        Timer timer;
        int rejects = m->collisionSolver.testOverlapPerformance(m->physicsSpace, profileInfo, numIterations);
        printf("** Hard overlap run time: %.1lf ms\t=> %d\n", timer.elapsedMillis(), rejects);
    }

    {
        Timer timer;
        m->collisionSolver.testCollisionPerformance(m->physicsSpace, profileInfo, numIterations, false);
        printf("** Collision run time: %.1lf ms\t\n", timer.elapsedMillis());
    }

    {
        Timer timer;
        m->collisionSolver.testCollisionPerformance(m->physicsSpace, profileInfo, numIterations, true);
        printf("** Collision midpoint run time: %.1lf ms\t\n", timer.elapsedMillis());
    }
}

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
    m->lastElapsedTimeMilliseconds = elapsedTimeMilliseconds;
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
    double maxDuration = physicsStep * 0.75f;

    Timer updateTimer;
    profileInfo.collisionTimeMillis = 0;
    profileInfo.collisionHandlingTimeMillis = 0;
    profileInfo.collisionGridUpdateTimeMillis = 0;

    while (m->timeBucket > physicsStep || runSingleStep)
    {
        profileInfo.slowdownFactor = 1.0;

        Timer stepTimer;

        m->integrator.performIntegration(m->physicsSpace, m->shapeMatchDragData, profileInfo);
        m->iterationNumber++;

        bool hasWheelMotors = m->physicsSpace.wheelMotors.size() > 0;
        if (m->physicsSpace.collisionsEnabled && (hasWheelMotors || m->iterationNumber % 2 == 0))
        {
            m->collisionSolver.handleCollisions(m->physicsSpace, profileInfo);
        }

        if (hasWheelMotors)
        {
            m->integrator.applyWheelMotorTraction(m->physicsSpace);
            m->integrator.dampWheelMotors(m->physicsSpace);
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
        if (runSingleStep)
        {
            runSingleStep = false;
            break;
        }
    }

    if (m->selectedShape() != -1)
    {
        m->collisionSolver.getCollisionCandidates(m->selectedShape(), m->collisionCandidates, m->physicsSpace);
    }
    else
    {
        m->collisionCandidates.clear();
    }

    int nextHistoricalIndex = (m->historicalIndex + 1) % NUM_HISTORICAL_STATES;

    profileInfo.numPhysicsSteps = numIterations;
    profileInfo.physicsTimeMillis = updateTimer.elapsedMillis();

    m->history[m->historicalIndex].assign(m->physicsSpace);
    m->historyRewindIndex = m->historicalIndex;
    m->historicalIndex = nextHistoricalIndex;

    for (int i = 0; i < m->frameCallbacks.size(); i++)
    {
        FrameCallback &callback = m->frameCallbacks[i];
        callback.function(this, callback.args);
    }

    m->endScriptIfLayoutChanged();

    if (m->script != nullptr)
    {
        m->script->update(*this, (float)elapsedTimeMilliseconds);
        m->rememberScriptLayout();
    }

    for (int i = 0; i < (int)GameKeyCode::NUM_KEY_CODES; i++)
    {
        m->keyPressedState[i] = false;
    }
}

void Game::onMouseDown(int button, float x, float y, bool shiftDown)
{
    m->mousePos = Vector2(x, y);
    if (button == 2)
    {
        m->panning = true;
        return;
    }

    if (m->addSubshapeData.active)
    {
        m->addSubshapeData.sourcePos = m->translatedMousePos();
        m->addSubshapeData.mouseDown = true;
        return;
    }

    Vector2 translatedPos = m->translatedMousePos();
    int pointIndex = m->pointAt(translatedPos);

    if (pointIndex != -1)
    {
        if (shiftDown)
        {
            int shapeIndex = m->shapeOfPoint(pointIndex);

            if (shapeIndex != -1)
            {
                m->selectedShapeIndex = shapeIndex;
            }
        }
        else
        {
            m->physicsSpace.mouseJoint.pointIndex = pointIndex;
            m->physicsSpace.mouseJoint.position = translatedPos;
        }

        return;
    }

    int shapeIndex = m->shapeAt(translatedPos);

    if (shapeIndex != -1)
    {
        m->selectedShapeIndex = shapeIndex;
        m->shapeMatchDragData.dragShapeIndex = shapeIndex;
        m->shapeMatchDragData.center = translatedPos;
    }
    else if (button == 1 && script() != nullptr)
    {
        m->script->clickedEmptySpace(*this, m->groundMousePos());
        m->rememberScriptLayout();
        m->draggingOverEmptySpace = true;
    }
}

void Game::onMouseUp(int button, float x, float y, bool shiftDown)
{

    m->mousePos = Vector2((float)x, (float)y);
    m->physicsSpace.mouseJoint.pointIndex = -1;
    m->shapeMatchDragData.dragShapeIndex = -1;
    m->draggingOverEmptySpace = false;

    if (button == 2)
    {
        m->panning = false;
        return;
    }

    if (m->addSubshapeData.active && m->addSubshapeData.mouseDown)
    {

        Range<Vector2> points(m->addSubshapeData.points, m->addSubshapeData.numPoints);
        Shapes::addSubshapeToShape(m->physicsSpace, m->addSubshapeData.sourcePos, points);
        updateBoundingBoxes();

        float signX = m->addSubshapeData.repeatX < 0 ? -1.0f : 1.0f;
        float signY = m->addSubshapeData.repeatY < 0 ? -1.0f : 1.0f;

        for (int i = 0; i < abs(m->addSubshapeData.repeatX); i++)
        {
            float repeatedOffsetX = static_cast<float>(i) * gridSize * signX;
            Shapes::addSubshapeToShape(m->physicsSpace, m->addSubshapeData.sourcePos + Vector2(repeatedOffsetX, 0.0f), points);
        }

        for (int i = 0; i < abs(m->addSubshapeData.repeatY); i++)
        {
            float repeatedOffsetY = signY * static_cast<float>(i) * gridSize;
            Shapes::addSubshapeToShape(m->physicsSpace, m->addSubshapeData.sourcePos + Vector2(0.0f, repeatedOffsetY), points);
        }

        m->addSubshapeData.repeatX = 0;
        m->addSubshapeData.repeatY = 0;
        m->addSubshapeData.mouseDown = false;
        m->addSubshapeData.active = false;

        updateBoundingBoxes();
    }

    if (paused())
    {
        updateBoundingBoxes();
    }
}

void Game::onMouseMove(float x, float y, float relativeX, float relativeY)
{
    m->mousePos = Vector2(x, y);

    if (m->panning)
    {
        m->offset.x += relativeX;
        m->offset.y += relativeY;
        return;
    }
    Vector2 translatedPos = m->translatedMousePos();

    if (m->draggingOverEmptySpace && script() != nullptr)
    {
        m->script->draggedOverEmptySpace(*this, m->groundMousePos());
        m->rememberScriptLayout();
    }

    if (m->addSubshapeData.active && m->addSubshapeData.mouseDown)
    {
        Vector2 delta = translatedPos - m->addSubshapeData.sourcePos;

        if (m->addSubshapeData.shape == AddSubshapeShape::SUBSHAPE_RECT)
        {

            float magY = delta.y;
            float magX = delta.x;

            if (abs(magX) > abs(magY))
            {
                m->addSubshapeData.repeatX = (int)ceilf(magX / gridSize);
                m->addSubshapeData.repeatY = 0;
            }
            else
            {
                m->addSubshapeData.repeatY = (int)ceilf(magY / gridSize);
                m->addSubshapeData.repeatX = 0;
            }
        }
        else
        {
            m->addSubshapeData.repeatX = 0;
            m->addSubshapeData.repeatY = 0;
        }
    }

    if (m->physicsSpace.mouseJoint.pointIndex != -1)
    {
        int i = m->physicsSpace.mouseJoint.pointIndex;
        m->physicsSpace.points.pos[i] = translatedPos;

        if (paused())
        {
            m->physicsSpace.points.shapeOriginalPos[i] += m->translatedMouseMove(relativeX, relativeY);
        }

        if (m->stopPointWhenDragging)
        {
            m->physicsSpace.points.velocity[i] = Vector2();
        }

        m->physicsSpace.mouseJoint.position = translatedPos;
        m->physicsSpace.triangulate();
    }

    if (m->shapeMatchDragData.dragShapeIndex != -1)
    {
        Shape &shape = m->physicsSpace.shapes[m->shapeMatchDragData.dragShapeIndex];
        Vector2 delta = m->translatedMouseMove(relativeX, relativeY);

        if (shape.isStatic || paused())
        {
            for (int i = shape.start; i < shape.end; i++)
            {
                m->physicsSpace.points.pos[i] += delta;
            }
        }
        else
        {
            m->shapeMatchDragData.center += delta;
        }
    }
}

void Game::mouseWheel(float x, float y)
{
    float zoomFactor = y > 0 ? 0.9f : 1.1f;
    Vector2 mousePos(x, y);
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
    int nudge = modState & (int)GameModkey::Shift ? 10 : 1;
    switch (keyCode)
    {
    case GameKeyCode::LEFT:
        m->offset += Vector2(static_cast<float>(nudge), 0.0f);
        break;
    case GameKeyCode::RIGHT:
        m->offset += Vector2(static_cast<float>(-nudge), 0.0f);
        break;
    case GameKeyCode::UP:
        m->offset += Vector2(0.0f, static_cast<float>(nudge));
        break;
    case GameKeyCode::DOWN:
        m->offset += Vector2(0.0f, static_cast<float>(-nudge));
        break;
    case GameKeyCode::BACKSPACE:
        if (m->selectedShape() != -1)
        {
            m->physicsSpace.removeShape(m->selectedShape());
            // The shapes after it have moved down
            m->selectedShapeIndex = -1;
            m->copyShapeIndex = -1;
            updateBoundingBoxes(true);
        }
        break;
    case GameKeyCode::F1:
        PhysicsSpaceStorage::dumpToUnitTest(m->physicsSpace, scale(), offset());
        break;
    case GameKeyCode::F2:
        if (m->selectedShape() != -1)
        {
            PhysicsSpaceStorage::dumpToPrefab(m->physicsSpace, m->selectedShape());
        }
        break;
    case GameKeyCode::F5:
        togglePaused();
        break;
    case GameKeyCode::F9:
        for (int i = 0; i < m->physicsSpace.points.velocity.size(); i++)
        {
            m->physicsSpace.points.velocity[i] = Vector2();
        }
        break;
    case GameKeyCode::PLUS:
        m->scale *= 1.1f;
        break;
    case GameKeyCode::MINUS:
        m->scale *= 0.9f;
        break;
    case GameKeyCode::F10:
        m->collisionSolver.toggleCollisionGrid();
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
        StringBuffer<512> savePath;
        savePath.append("%s/%s", m->appPath.data, "dump.txt");

        if (keyCode == GameKeyCode::F6)
        {
            PhysicsSpaceStorage::dumpToFile(physicsSpace(), savePath.data);
            printf("Wrote to %s\n", savePath.data);
        }
        else
        {
            printf("Attempting read from %s\n", savePath.data);
            PhysicsSpaceStorage::loadFromFile(physicsSpace(), savePath.data);
            printf("Read from %s\n", savePath.data);
        }
    }
    break;
    case GameKeyCode::C:
    {
        if (modState & (int)GameModkey::Ctrl || modState & (int)GameModkey::Meta)
        {
            m->copyShapeIndex = m->selectedShape();
        }
    }
    break;
    case GameKeyCode::V:
    {
        if (modState & (int)GameModkey::Ctrl || modState & (int)GameModkey::Meta)
        {
            PhysicsSpace &space(m->physicsSpace);

            if (m->copyShapeIndex != -1 && m->copyShapeIndex < space.shapes.size())
            {
                const Shape &copyShape = space.shapes[m->copyShapeIndex];
                ShapeProperties averages = ShapeUtils::getShapeProperties(space.points.range(), copyShape);

                Shape newShape = space.pasteShape(averages.center.x + 30.0f, averages.center.y + 30.0f, m->copyShapeIndex, space);
                m->copyShapeIndex = newShape.index;
                m->selectedShapeIndex = newShape.index;
            }
        }
    }
    break;
    case GameKeyCode::S:
    {
        if (modState & (int)GameModkey::Ctrl || modState & (int)GameModkey::Meta)
        {
            saveToFile();
        }
        else if (m->selectedShape() != -1)
        {
            Shape &shape = m->physicsSpace.shapes[m->selectedShape()];
            shape.isStatic = !shape.isStatic;
        }
    }

    break;
    case GameKeyCode::O:
    {
        if (m->selectedShape() != -1)
        {
            Shapes::snapToGrid(m->physicsSpace, m->selectedShape());
        }
    }
    break;
    case GameKeyCode::K:
        if (!m->addSubshapeData.active)
        {
            m->addSubshapeData.active = true;
        }

        m->addSubshapeData.shape = (AddSubshapeShape)((m->addSubshapeData.shape + 1) % (int)AddSubshapeShape::NUM_SUBSHAPES);
        m->addSubshapeData.repeatX = 0;
        m->addSubshapeData.repeatY = 0;

        switch (m->addSubshapeData.shape)
        {
        case AddSubshapeShape::SUBSHAPE_RECT:
            m->addSubshapeData.numPoints = 4;
            m->addSubshapeData.points[0] = Vector2(0, 0);
            m->addSubshapeData.points[1] = Vector2(gridSize, 0.0f);
            m->addSubshapeData.points[2] = Vector2(gridSize, gridSize);
            m->addSubshapeData.points[3] = Vector2(0, gridSize);
            break;
        case AddSubshapeShape::SUBSHAPE_TRIANGLE_1:
            m->addSubshapeData.numPoints = 3;
            m->addSubshapeData.points[0] = Vector2(gridSize, 0.0f);
            m->addSubshapeData.points[1] = Vector2(gridSize, gridSize);
            m->addSubshapeData.points[2] = Vector2(0, gridSize);
            break;
        case AddSubshapeShape::SUBSHAPE_TRIANGLE_2:
            m->addSubshapeData.numPoints = 3;
            m->addSubshapeData.points[0] = Vector2(gridSize, gridSize);
            m->addSubshapeData.points[1] = Vector2(0, gridSize);
            m->addSubshapeData.points[2] = Vector2(0, 0);
            break;
        case AddSubshapeShape::SUBSHAPE_TRIANGLE_3:
            m->addSubshapeData.numPoints = 3;
            m->addSubshapeData.points[0] = Vector2(0, gridSize);
            m->addSubshapeData.points[1] = Vector2(0, 0);
            m->addSubshapeData.points[2] = Vector2(gridSize, 0.0f);
            break;
        case AddSubshapeShape::SUBSHAPE_TRIANGLE_4:
            m->addSubshapeData.numPoints = 3;
            m->addSubshapeData.points[0] = Vector2(0, 0);
            m->addSubshapeData.points[1] = Vector2(gridSize, 0.0f);
            m->addSubshapeData.points[2] = Vector2(gridSize, gridSize);
            break;
        default:
            break;
        }

        break;
    case GameKeyCode::L:
    {
        m->addSubshapeData = AddSubShapeData();
        m->addSubshapeData.active = true;
        break;
    }
    case GameKeyCode::P:
    {
        Vector2 newPos = m->translatedMousePos();

        if (m->selectedShape() != -1)
        {
            Shapes::addPointToShape(m->physicsSpace, m->selectedShape(), newPos.x, newPos.y);
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
    // keyPressedState is left for keyWasPressed until the end of the update, so a tap
    // that goes down and up within one frame isn't lost
    m->keyState[(size_t)keyCode] = false;
    m->modKeyState = modState;
}

bool Game::keyWasPressed(GameKeyCode keyCode)
{
    return m->keyPressedState[(size_t)keyCode];
}

bool Game::keyIsPressed(GameKeyCode keyCode)
{
    return m->keyState[(size_t)keyCode];
}

Array<ShapeBoundingBox> &Game::shapeBoundingBoxes()
{
    return m->collisionSolver.boundingBoxes();
}

Array<OrientedBoundingBox> &Game::shapeOrientedBoundingBoxes()
{
    return m->collisionSolver.orientedBoundingBoxes();
}

PhysicsSpace &Game::physicsSpace()
{
    return m->physicsSpace;
}

const ShapeMatchDragData &Game::shapeMatchDragData()
{
    return m->shapeMatchDragData;
}

const char *Game::currentSceneName()
{
    return m->currentSceneName == nullptr ? m->filePath.data : m->currentSceneName;
}

int &Game::simulationSpeed()
{
    return m->simulationSpeed;
}

void Game::saveToFile()
{
    StringBuffer<512> savePath;
    savePath.append("%s/%s", m->appPath.data, m->filePath.data);
    PhysicsSpaceStorage::dumpToFile(physicsSpace(), savePath.data);
}

void Game::setPaused(bool paused)
{
    m->paused = paused;
    updateBoundingBoxes();
}

void Game::togglePaused()
{
    setPaused(!m->paused);
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

    for (int i = 0; i < m->physicsSpace.shapes.size(); i++)
    {
        Shape &shape = m->physicsSpace.shapes[i];
        shape.index = i;
    }
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
    return m->selectedShape();
}

void Game::setSelectedShapeIndex(int index)
{
    m->selectedShapeIndex = index;
}

bool Game::dragging() const
{
    return m->physicsSpace.mouseJoint.pointIndex != -1 || m->shapeMatchDragData.dragShapeIndex != -1;
}

int Game::shapeUnderMouse()
{
    if (m->physicsSpace.mouseJoint.pointIndex != -1)
    {
        return m->shapeOfPoint(m->physicsSpace.mouseJoint.pointIndex);
    }

    if (m->shapeMatchDragData.dragShapeIndex != -1)
    {
        return m->shapeMatchDragData.dragShapeIndex;
    }

    // Panning and placing subshapes don't drag anything
    if (m->panning || m->addSubshapeData.active)
    {
        return -1;
    }

    Vector2 translatedPos = m->translatedMousePos();
    int pointIndex = m->pointAt(translatedPos);

    return pointIndex != -1 ? m->shapeOfPoint(pointIndex) : m->shapeAt(translatedPos);
}

int Game::hoveredShapeIndex() const
{
    return m->hoveredShapeIndex;
}

void Game::setHoveredShapeIndex(int index)
{
    m->hoveredShapeIndex = index;
}

void Game::setScript(SceneScript *script)
{
    delete m->script;
    m->script = script;
    m->rememberScriptLayout();
}

void Game::requestFitView()
{
    m->fitViewRequested = true;
}

bool Game::takeFitViewRequest()
{
    const bool requested = m->fitViewRequested;
    m->fitViewRequested = false;
    return requested;
}

SceneScript *Game::script() const
{
    m->endScriptIfLayoutChanged();
    return m->script;
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

bool &Game::debugDraw()
{
    return m->debugDraw;
}

bool &Game::antiAliasing()
{
    return m->antiAliasing;
}

double Game::lastElapsedTimeMilliseconds() const
{
    return m->lastElapsedTimeMilliseconds;
}
