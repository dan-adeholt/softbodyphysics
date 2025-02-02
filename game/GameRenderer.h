#ifndef __GAME_RENDERER_H
#define __GAME_RENDERER_H

struct SDL_Renderer;
class Game;
struct ConsoleProfileInfo;

template <typename T>
struct Range;
struct Shape;
struct PointMassesRange;
struct Spring;
struct PhysicsSpace;
struct ShapeMatchDragData;

struct GameRenderer
{
    GameRenderer(SDL_Renderer *renderer);
    ~GameRenderer();

    void renderGame(SDL_Renderer *renderer, Game &game, bool renderShapeMatching, ConsoleProfileInfo &profileInfo);

private:
    void renderShapes(SDL_Renderer *renderer, int selectedShapeIndex, Range<Shape> shapes, PointMassesRange &pointMasses, PhysicsSpace &space, bool renderShapeMatching, float scale, const ShapeMatchDragData &dragData);
    void renderSprings(SDL_Renderer *renderer, Range<Spring> springs, const PointMassesRange &points, float scale);

    // Disable copy constructor and assignment
    GameRenderer(const GameRenderer &);
    GameRenderer &operator=(const GameRenderer &);

    struct Impl;
    Impl *m;
};

#endif