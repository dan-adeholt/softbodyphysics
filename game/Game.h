#ifndef __GAME_H
#define __GAME_H

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

    void update(double elapsedTimeMilliseconds, ConsoleProfileInfo &profileInfo);

    void rewindHistory();
    void forwardHistory();

    void mouseButtonDown(int x, int y);
    void mouseButtonUp(int x, int y);
    void mouseMove(int x, int y);

    PhysicsSpace &physicsSpace();

private:
    void updateAfterRewindOrForward();

    struct Impl;
    Impl *m;
};

#endif