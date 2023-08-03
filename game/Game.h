#ifndef __GAME_H
#define __GAME_H

struct PointMass;

template <typename T>
class Range;

class Game
{
public:
    Game();
    ~Game();

    void getPointMasses(Range<PointMass> &pointMasses) const;
    void update(double elapsedTimeMilliseconds);

private:
    void handleCollisions(Range<PointMass> &points);
    void handleGravity(Range<PointMass> &points, double elapsedTimeMilliseconds);

    struct Impl;
    Impl *m_impl;
};

#endif