#ifndef __GAME_H
#define __GAME_H

struct SceneDefinition;
struct PhysicsSpace;
struct ConsoleProfileInfo;
struct ShapeBoundingBox;
class Vector2;

enum class GameKeyCode : int;

template <typename T>
struct Range;

template <typename T>
class Array;

struct GameRenderSettings
{
    bool renderSprings = true;
    bool renderPoints = true;
    bool renderShapeLines = true;
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

    void mouseButtonDown(int button, int x, int y, bool shiftDown);
    void mouseButtonUp(int button, int x, int y, bool shiftDown);
    void mouseMove(int x, int y, int relativeX, int relativeY);
    void mouseWheel(int x, int y);

    void keyDown(GameKeyCode keyCode, int modState, ConsoleProfileInfo &profileInfo);
    void keyUp(GameKeyCode keyCode, int modState, ConsoleProfileInfo &profileInfo);

    bool keyWasPressed(GameKeyCode keyCode);

    Array<ShapeBoundingBox> &shapeBoundingBoxes();

    PhysicsSpace &physicsSpace();
    PhysicsSpace &lastCollisionSpace();

    int &simulationSpeed();
    bool paused();

    void setShouldQuit();
    bool shouldQuit();

    void updateBoundingBoxes(bool clear = false);

    void saveToFile();
    void setPaused(bool paused = true);
    void togglePaused();

    const char *currentSceneName();

    Vector2 &offset();
    float &scale();

    void scheduleCallback(void (*function)(Game *, void *), void *args, float delay);
    void scheduleFrameCallback(void (*function)(Game *, void *), void *args);

    int selectedShapeIndex() const;

    void runFor(int timeMillis, bool pauseAfter = false);

    void setStopPointWhenDragging(bool stopPointWhenDragging);

    GameRenderSettings renderSettings();
    void setRenderSettings(const GameRenderSettings &settings);

    const char *title() const;

private:
    void updateAfterRewindOrForward();

    struct Impl;
    Impl *m;
};

#endif