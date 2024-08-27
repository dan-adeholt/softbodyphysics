#ifndef __GAME_H
#define __GAME_H

struct SceneDefinition;
struct PhysicsSpace;
struct ConsoleProfileInfo;

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

    void mouseButtonDown(int x, int y);
    void mouseButtonUp(int x, int y);
    void mouseMove(int x, int y, int relativeX, int relativeY);

    PhysicsSpace &physicsSpace();

    int &timeSkip();
    bool &paused();

    const char *currentSceneName();

private:
    void updateAfterRewindOrForward();

    struct Impl;
    Impl *m;
};

#endif