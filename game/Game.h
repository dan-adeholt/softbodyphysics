#ifndef __GAME_H
#define __GAME_H

struct PointMass;
struct Spring;
struct Shape;

template <typename T>
class Range;

class Game
{
public:
    Game();
    ~Game();

    void getSprings(Range<Spring>& springs);
    void getDynamicPoints(Range<PointMass> &pointMasses) const;
    void getStaticPoints(Range<PointMass> &pointMasses) const;

    void getDynamicShapes(Range<Shape>& shapes) const;
    void getStaticShapes(Range<Shape>& shapes) const;

    void update(double elapsedTimeMilliseconds);

    void loadFromFile(const char* path);
    void dumpToFile(const char* path);
    void rewindHistory();
    void forwardHistory();
private:
    void updateAfterRewindOrForward();
    void handleCollisions(Range<PointMass> &points, const Range<int> &collisionCounterForPoints);
    void handleGravity(Range<PointMass> &points, double elapsedTimeMilliseconds);

    struct Impl;
    Impl *m_impl;
};

#endif