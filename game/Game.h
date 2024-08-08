#ifndef __GAME_H
#define __GAME_H

struct PointMassesRange;
struct PointMasses;
struct Spring;
struct Shape;
struct StaticJoint;
struct ConsoleProfileInfo;
struct PointDerivative;

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
    void getDynamicPoints(PointMassesRange &pointMasses) const;
    void getStaticPoints(PointMassesRange &pointMasses) const;

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

    int nextShapeIndex();

    int nextStaticShapeIndex();

    Array<Shape> &shapes();
    Array<Shape> &staticShapes();
    PointMasses &points();
    PointMasses &staticPoints();
    Array<Spring> &springs();
    Array<StaticJoint> &staticJoints();

    void testRK4Performance(int iterations);
    void testRK4PreparePerformance(int iterations);
    void testSpringPerformance(int iterations);

private:
    void performThreadedSpringDerivatives(PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives, ConsoleProfileInfo &profileInfo);

    void performRK4Integration(PointMassesRange &points,
                               Range<Spring> &springs,
                               Range<int> &collisionCounterForPoints,
                               bool updateCollisions,
                               ConsoleProfileInfo &profileInfo);
    void prepareRK4Step(PointMassesRange &initialState, Range<Spring> &springs, double dt, Array<PointDerivative> &derivatives, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo);
    void updateRK4Springs(Range<Spring> &springs, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo);
    void updateAfterRewindOrForward();
    void handleCollisions(PointMassesRange &points, const Range<int> &collisionCounterForPoints, double step, ConsoleProfileInfo &profileInfo);

    struct Impl;
    Impl *m_impl;
};

#endif