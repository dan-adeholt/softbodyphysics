#ifndef __SCENE_SCRIPT_H
#define __SCENE_SCRIPT_H

#include <stdint.h>
#include "../math/Vector2.h"
#include "../containers/Array.h"

class Game;
struct GeometryLayer;

struct CanvasColor
{
    float r;
    float g;
    float b;
    float a;

    // From 0xRRGGBB
    static CanvasColor hex(uint32_t rgb, float alpha = 1.0f);
};

// Draws over the scene in world coordinates. Line widths are in screen pixels, so they stay the
// same when zooming; pixel() converts other sizes.
class SceneCanvas
{
public:
    SceneCanvas(GeometryLayer &layer, float scale);

    // World units per screen pixel
    float pixel() const;

    void disc(Vector2 center, float radius, CanvasColor color);
    void ring(Vector2 center, float radius, float widthPixels, CanvasColor color);
    void line(Vector2 from, Vector2 to, float widthPixels, CanvasColor color);

private:
    GeometryLayer &layer;
    float scale;
    Array<Vector2> ringPoints;
};

// Code that belongs to one scene, such as the rules of a game played in it. A scene creates one in
// its init function and hands it to Game::setScript; Game deletes it when another scene loads.
class SceneScript
{
public:
    virtual ~SceneScript() = default;

    // Every frame, after the physics steps, including while paused
    virtual void update(Game &game, float elapsedMs) {}

    // Drawn on top of the shapes
    virtual void drawOverlay(Game &game, SceneCanvas &canvas) {}

    // A fill colour for a shape instead of its usual one, as 0xRRGGBB
    virtual bool shapeFill(int shapeIndex, uint32_t &color) { return false; }
};

#endif
