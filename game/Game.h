#ifndef __GAME_H
#define __GAME_H

struct ShapeQuad;
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

    void toggleShapeMatchingEnabled();
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
    Array<ShapeQuad> &partialShapes();

private:
    void performIntegration(Range<Shape> &shapeRange,
                            PointMassesRange &points,
                            Range<Spring> &springs,
                            Range<int> &collisionCounterForPoints,
                            bool updateCollisions,
                            ConsoleProfileInfo &profileInfo);

    void updateAfterRewindOrForward();
    void handleCollisions(PointMassesRange &points, const Range<int> &collisionCounterForPoints, float step, ConsoleProfileInfo &profileInfo);

    struct Impl;
    Impl *m_impl;
};

#endif