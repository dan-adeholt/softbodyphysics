#ifndef __GAME_H
#define __GAME_H

struct PointMass;
struct Spring;
struct Shape;
struct StaticJoint;
struct ConsoleProfileInfo;

template <typename T>
class Range;

template <typename T>
class Array;

class Game
{
public:
    Game();
    ~Game();

    void clear();
    void init(const char *sceneType);
    void getSprings(Range<Spring> &springs);
    void getDynamicPoints(Range<PointMass> &pointMasses) const;
    void getStaticPoints(Range<PointMass> &pointMasses) const;

    void getDynamicShapes(Range<Shape> &shapes) const;
    void getStaticShapes(Range<Shape> &shapes) const;

    void update(double elapsedTimeMilliseconds, ConsoleProfileInfo &profileInfo);

    void loadFromFile(const char *path);
    void dumpToFile(const char *path);
    void rewindHistory();
    void forwardHistory();

    void setGravityEnabled(bool gravityEnabled);
    void setCollisionsEnabled(bool collisionsEnabled);

    void mouseButtonDown(int x, int y);
    void mouseButtonUp(int x, int y);
    void mouseMove(int x, int y);

    Array<Shape> &shapes();
    Array<Shape> &staticShapes();
    Array<PointMass> &points();
    Array<PointMass> &staticPoints();
    Array<Spring> &springs();
    Array<StaticJoint> &staticJoints();

private:
    void updateAfterRewindOrForward();
    void handleCollisions(Range<PointMass> &points, const Range<int> &collisionCounterForPoints, double step);

    struct Impl;
    Impl *m_impl;
};

#endif