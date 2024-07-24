#ifndef __GAME_RENDERER_H
#define __GAME_RENDERER_H

struct SDL_Renderer;
struct Game;
struct ConsoleProfileInfo;

template <typename T>
class Range;
struct Shape;
struct PointMassesRange;
struct Spring;

struct GameRenderer
{
    GameRenderer(SDL_Renderer *renderer);
    ~GameRenderer();

    void renderGame(SDL_Renderer *renderer, Game &game, ConsoleProfileInfo &profileInfo);

private:
    void renderShapes(SDL_Renderer *renderer, Range<Shape> shapes, PointMassesRange &pointMasses);
    void renderSprings(SDL_Renderer *renderer, Range<Spring> springs, const PointMassesRange &points);

    // Disable copy constructor and assignment
    GameRenderer(const GameRenderer &);
    GameRenderer &operator=(const GameRenderer &);

    struct Impl;
    Impl *m;
};

#endif