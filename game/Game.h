#ifndef __GAME_H
#define __GAME_H

struct SceneDefinition;
struct PhysicsSpace;
struct ConsoleProfileInfo;
struct ShapeBoundingBox;
class Vector2;

template <typename T>
struct Range;

template <typename T>
class Array;

class Game
{
public:
    Game();
    ~Game();

    void clear();
    void init(const char *sceneType);
    void init(const SceneDefinition &sceneDefinition);

    void update(double elapsedTimeMilliseconds, ConsoleProfileInfo &profileInfo);

    void rewindHistory();
    void forwardHistory();

    void mouseButtonDown(int button, int x, int y, bool shiftDown);
    void mouseButtonUp(int button, int x, int y, bool shiftDown);
    void mouseMove(int x, int y, int relativeX, int relativeY);
    void mouseWheel(int x, int y);

    Array<ShapeBoundingBox> &shapeBoundingBoxes();

    PhysicsSpace &physicsSpace();

    int &simulationSpeed();
    bool paused();

    void updateBoundingBoxes();


    void setPaused();
    void togglePaused();

    const char *currentSceneName();

    Vector2 &offset();
    float &scale();

    void scheduleCallback(void (*function)(Game *, void *), void *args, float delay);

    int selectedShapeIndex() const;

private:
    void updateAfterRewindOrForward();

    struct Impl;
    Impl *m;
};

#endif