#ifndef __GAME_H
#define __GAME_H

struct CollisionGridSimple;
struct SceneDefinition;
struct PhysicsSpace;
struct ConsoleProfileInfo;
struct ShapeBoundingBox;
class Vector2;
struct ShapeMatchDragData;
struct AddSubShapeData;
struct OrientedBoundingBox;

enum class GameKeyCode : int;

template <typename T>
struct Range;

template <typename T>
class Array;

struct GameRenderSettings
{
    bool renderSprings = true;
    bool renderShapeJoints = true;
    bool renderPoints = true;
    bool renderShapeLines = true;
    bool renderTriangles = false;
    bool renderVelocityVectors = false;
    bool renderShapeMatching = false;
    bool renderPointIndices = false;
    bool clearDebugGeometryWhenPaused = false;
};

class Game
{
public:
    Game(const char *appPath, const char *filePath);
    Game(const Game &other) = delete;
    ~Game();

    void clear();
    void init(const char *sceneType);
    void init(const SceneDefinition &sceneDefinition);

    void update(double elapsedTimeMilliseconds, bool singleStep, ConsoleProfileInfo &profileInfo);

    void rewindHistory();
    void forwardHistory();

    void onMouseDown(int button, float x, float y, bool shiftDown);
    void onMouseUp(int button, float x, float y, bool shiftDown);
    void onMouseMove(float x, float y, float relativeX, float relativeY);
    void mouseWheel(float x, float y);

    void keyDown(GameKeyCode keyCode, int modState, ConsoleProfileInfo &profileInfo);
    void keyUp(GameKeyCode keyCode, int modState, ConsoleProfileInfo &profileInfo);

    bool keyWasPressed(GameKeyCode keyCode);

    Array<ShapeBoundingBox> &shapeBoundingBoxes();

    Array<OrientedBoundingBox> &shapeOrientedBoundingBoxes();

    PhysicsSpace &physicsSpace();
    const ShapeMatchDragData &shapeMatchDragData();
    int &simulationSpeed();
    bool paused();

    void setShouldQuit();
    bool shouldQuit();

    void updateBoundingBoxes(bool clear = false);

    void saveToFile();
    void setPaused(bool paused = true);
    void togglePaused();

    const char *
    currentSceneName();

    Vector2 &offset();
    float &scale();

    void scheduleCallback(void (*function)(Game *, void *), void *args, float delay);
    void scheduleFrameCallback(void (*function)(Game *, void *), void *args);

    int selectedShapeIndex() const;
    void setSelectedShapeIndex(int index);

    void runFor(int timeMillis, bool pauseAfter = false);

    void setStopPointWhenDragging(bool stopPointWhenDragging);

    GameRenderSettings renderSettings();
    void setRenderSettings(const GameRenderSettings &settings);

    const char *title() const;

    const AddSubShapeData &addSubShapeData() const;

    const Vector2 &mousePos() const;

    CollisionGridSimple &collisionGrid() const;

    Range<int> collisionCandidates() const;

    void testCollisionPerformance(ConsoleProfileInfo &profileInfo, int numIterations) const;

private:
    void updateAfterRewindOrForward();

    struct Impl;
    Impl *m;
};

#endif