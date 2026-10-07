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

// How a scene is seen at an angle, like a strategy game's map, rather than straight from above. The physics
// still happens on a flat ground; the ground is turned around the vertical and tilted away, and things standing
// on it rise towards the top of the screen. Orthographic, so sizes don't change with distance. Positions in the
// view are what the usual zoom and pan apply to.
struct ViewProjection
{
    float turn = 0.0f;              // Around the vertical, in radians
    float elevation = PI_F * 0.5f;  // How high the view is above the horizon, in radians: straight down at 90 degrees
    float pickHeight = 0.0f;        // How high above the ground the mouse grabs things, about where their middles are drawn
    float tallest = 0.0f;           // The height of the tallest thing, so fitting the view leaves room for it

    // Where a point height above the ground at ground lands in the view
    Vector2 toView(Vector2 ground, float height = 0.0f) const;
    // The point on the ground that a point height above lands on view at
    Vector2 toGround(Vector2 view, float height = 0.0f) const;
    // A move across the view as a move across the ground
    Vector2 toGroundMove(Vector2 viewMove) const;
    // The ground turned, but not tilted, so a shape drawn with it stands upright, facing the viewer
    Vector2 turned(Vector2 groundOffset) const;
    // Seen from above, a direction on the ground towards the viewer
    Vector2 towardsViewer() const;
};

// Draws over the scene in world coordinates. Line widths are in screen pixels, so they stay the
// same when zooming; pixel() converts other sizes.
class SceneCanvas
{
public:
    SceneCanvas(GeometryLayer &layer, float scale);

    // World units per screen pixel
    float pixel() const;

    // While set, positions given to disc, ring and line are on the ground, and are drawn lying on it as the
    // projection shows it, a disc as an ellipse. Everything else, and anything drawn without one, is in view
    // coordinates.
    void setGround(const ViewProjection *projection);

    void disc(Vector2 center, float radius, CanvasColor color);
    void ring(Vector2 center, float radius, float widthPixels, CanvasColor color);
    void line(Vector2 from, Vector2 to, float widthPixels, CanvasColor color);

    // Filled as a fan from the middle of the points, so for shapes that are about convex
    void polygon(const Array<Vector2> &points, CanvasColor color);
    // Shaded from light, a point inside, out to rim at the edges
    void shadedPolygon(const Array<Vector2> &points, Vector2 light, CanvasColor lightColor, CanvasColor rimColor);
    // A closed outline around the points
    void outline(const Array<Vector2> &points, float widthPixels, CanvasColor color);

private:
    Vector2 onGround(Vector2 pos) const;

    GeometryLayer &layer;
    float scale;
    const ViewProjection *ground = nullptr;
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

    // How the scene is seen, if at an angle rather than from straight above. Mouse positions are taken back
    // through it onto the ground, where the physics is.
    virtual const ViewProjection *viewProjection() const { return nullptr; }

    // Draws the whole scene, in view coordinates, instead of the usual shapes and grid. Returns false to leave it
    // to the usual drawing; drawOverlay isn't called when it returns true.
    virtual bool drawScene(Game &game, SceneCanvas &canvas) { return false; }
};

#endif
