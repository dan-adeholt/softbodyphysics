#ifndef __SCENE_SCRIPT_H
#define __SCENE_SCRIPT_H

#include <stdint.h>
#include "../math/Vector2.h"
#include "../containers/Array.h"

class Game;
struct GeometryLayer;
struct PhysicsSpace;

struct CanvasColor
{
    float r;
    float g;
    float b;
    float a;

    // From 0xRRGGBB
    static CanvasColor hex(uint32_t rgb, float alpha = 1.0f);
};

// Between colours a and b, as 0xRRGGBB, t of the way to b
uint32_t mixHex(uint32_t a, uint32_t b, float t);

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

// Pushes every moving point within radius away from center, less with distance and on heavier points
void pushAwayFrom(PhysicsSpace &space, Vector2 center, float radius, float impulse, float maxVelocityChange);

// A burst t of the way through: a flash that fades as a ring expands to radius
void drawBurst(SceneCanvas &canvas, Vector2 pos, float radius, float t, CanvasColor flash);

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

    // A left click on the canvas that didn't grab a point or a shape, at pos in world coordinates
    virtual void clickedEmptySpace(Game &game, Vector2 pos) {}

    // The mouse moving to pos while the button that clicked empty space is still held
    virtual void draggedOverEmptySpace(Game &game, Vector2 pos) {}

    // Whether the scene has controls of its own, such as sliders for its rules, for drawControls to draw
    virtual bool hasControls() const { return false; }

    // The scene's own controls, as ImGui widgets, in a panel the editor shows next to the scene
    virtual void drawControls(Game &game) {}
};

#endif
