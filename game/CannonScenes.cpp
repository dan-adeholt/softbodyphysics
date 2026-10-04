#include "CannonScenes.h"
#include <math.h>
#include "Game.h"
#include "GameKeyCode.h"
#include "SceneScript.h"
#include "Shapes.h"
#include "../physics/PhysicsSpace.h"
#include "../containers/Array.h"
#include "../containers/StringBuffer.h"
#include "../utils/MinMax.h"

static const float ballRadius = 22.0f;

static Vector2 shapeCenter(const PhysicsSpace &space, const Shape &shape)
{
    Vector2 center;

    for (int i = shape.start; i < shape.end; i++)
    {
        center += space.points.pos[i];
    }

    return center / static_cast<float>(shape.end - shape.start);
}

static Vector2 shapeVelocity(const PhysicsSpace &space, const Shape &shape)
{
    Vector2 velocity;

    for (int i = shape.start; i < shape.end; i++)
    {
        velocity += space.points.velocity[i];
    }

    return velocity / static_cast<float>(shape.end - shape.start);
}

// Static quads only; the editor can delete shapes, which renumbers the rest
static bool isStaticQuad(const PhysicsSpace &space, int shapeIndex)
{
    return shapeIndex >= 0 && shapeIndex < space.shapes.size() && space.shapes[shapeIndex].isStatic &&
           space.shapes[shapeIndex].end - space.shapes[shapeIndex].start == 4;
}

// Places a shape in its rest pose around center, moving at velocity. The rest pose isn't necessarily
// centered on the origin, so it is placed relative to its own center. Also brings back parked shapes.
static void placeShape(PhysicsSpace &space, int shapeIndex, Vector2 center, Vector2 velocity)
{
    Shape &shape = space.shapes[shapeIndex];
    shape.isStatic = false;

    Vector2 restCenter;

    for (int i = shape.start; i < shape.end; i++)
    {
        restCenter += space.points.shapeOriginalPos[i];
    }

    restCenter /= static_cast<float>(shape.end - shape.start);

    for (int i = shape.start; i < shape.end; i++)
    {
        space.points.pos[i] = center + (space.points.shapeOriginalPos[i] - restCenter);
        space.points.velocity[i] = velocity;
    }
}

// Takes a shape out of play: frozen as a static shape, out of sight at pos, until placeShape brings it
// back. Removing it instead would renumber the shapes after it.
static void parkShape(PhysicsSpace &space, int shapeIndex, Vector2 pos)
{
    Shape &shape = space.shapes[shapeIndex];
    const Vector2 shift = pos - shapeCenter(space, shape);

    for (int i = shape.start; i < shape.end; i++)
    {
        space.points.pos[i] += shift;
        space.points.velocity[i] = Vector2();
    }

    shape.isStatic = true;
}

// Pushes every moving point within radius away from center, less with distance and on heavier points
static void pushAwayFrom(PhysicsSpace &space, Vector2 center, float radius, float impulse, float maxVelocityChange)
{
    for (int s = 0; s < space.shapes.size(); s++)
    {
        const Shape &shape = space.shapes[s];

        if (shape.isStatic)
        {
            continue;
        }

        for (int i = shape.start; i < shape.end; i++)
        {
            const Vector2 offset = space.points.pos[i] - center;
            const float distance = offset.length();

            if (distance >= radius || distance < 0.001f)
            {
                continue;
            }

            const float velocityChange = min(impulse * (1.0f - distance / radius) / space.points.mass[i], maxVelocityChange);
            space.points.velocity[i] += offset * (velocityChange / distance);
        }
    }
}

// A burst t of the way through: a flash that fades as a ring expands to radius
static void drawBurst(SceneCanvas &canvas, Vector2 pos, float radius, float t, CanvasColor flash)
{
    const float fade = 1.0f - clamp(t, 0.0f, 1.0f);
    CanvasColor outer = flash;
    outer.a = 0.55f * fade * fade;

    canvas.disc(pos, radius * (0.35f + 0.4f * t), outer);
    canvas.disc(pos, radius * (0.2f + 0.25f * t), {1.0f, 0.95f, 0.75f, 0.8f * fade * fade});
    canvas.ring(pos, radius * (0.3f + 0.7f * t), 6.0f * fade + 1.0f, {1.0f, 1.0f, 1.0f, 0.9f * fade});
}

// A static base and a barrel pivoting on the middle of its top. The barrel turns to the aim angle,
// so dragging the base moves the whole cannon. Fires soft balls from a pool: new ones until there
// are enough, then the oldest one again.
class Cannon
{
public:
    static const int maxBalls = 32;

    // basePos is the top left corner of the base
    void create(PhysicsSpace &space, Vector2 basePos, float aimAngle)
    {
        Shape base = Shapes::createStaticQuad(space, basePos.x, basePos.y, 90.0f, 90.0f, 1.0f);
        const Vector2 pivotPos(basePos.x + 45.0f, basePos.y);
        const float barrelLength = 130.0f;
        const float barrelWidth = 40.0f;
        Shape barrel = Shapes::createStaticQuad(space, pivotPos.x, pivotPos.y - barrelWidth * 0.5f, barrelLength, barrelWidth, 1.0f);

        baseShapeIndex = base.index;
        barrelShapeIndex = barrel.index;
        angle = aimAngle;

        for (int i = 0; i < 4; i++)
        {
            barrelRestOffsets[i] = space.points.pos[barrel.start + i] - pivotPos;
        }

        updateBarrel(space);
    }

    bool isValid(const PhysicsSpace &space) const
    {
        return isStaticQuad(space, baseShapeIndex) && isStaticQuad(space, barrelShapeIndex);
    }

    Vector2 pivot(const PhysicsSpace &space) const
    {
        const Shape &base = space.shapes[baseShapeIndex];
        return (space.points.pos[base.start] + space.points.pos[base.start + 1]) * 0.5f;
    }

    // Turns towards target, between straight back and 25 degrees down, low enough to hit things right in
    // front of the cannon while the barrel stays clear of the ground
    void aimAt(Vector2 target, const PhysicsSpace &space)
    {
        const Vector2 toTarget = target - pivot(space);
        const float lowest = 25.0f * PI_F / 180.0f;
        const float highest = -170.0f * PI_F / 180.0f;
        angle = atan2f(toTarget.y, toTarget.x);

        if (angle > lowest)
        {
            angle = angle < PI_F * 0.5f ? lowest : highest;
        }

        angle = max(angle, highest);
    }

    // Moves the barrel to the aim angle around the pivot
    void updateBarrel(PhysicsSpace &space) const
    {
        const Shape &barrel = space.shapes[barrelShapeIndex];
        const Vector2 pivotPos = pivot(space);

        for (int i = 0; i < 4; i++)
        {
            space.points.pos[barrel.start + i] = pivotPos + barrelRestOffsets[i].rotate(angle);
        }
    }

    // Where a ball leaves the barrel and in which direction. The barrel's first and last points are
    // at the breech, the middle two at the muzzle.
    void muzzle(const PhysicsSpace &space, Vector2 &spawn, Vector2 &direction) const
    {
        const Shape &barrel = space.shapes[barrelShapeIndex];
        const Vector2 breechPos = (space.points.pos[barrel.start] + space.points.pos[barrel.start + 3]) * 0.5f;
        const Vector2 muzzlePos = (space.points.pos[barrel.start + 1] + space.points.pos[barrel.start + 2]) * 0.5f;
        direction = (muzzlePos - breechPos).normalized();
        spawn = muzzlePos + direction * (ballRadius + 4.0f);
    }

    // Returns the index of the fired ball, or -1
    int fire(Game &game, float speed)
    {
        PhysicsSpace &space = game.physicsSpace();
        Vector2 spawn;
        Vector2 direction;
        muzzle(space, spawn, direction);
        const Vector2 velocity = direction * speed;

        if (numBalls < maxBalls)
        {
            Shape ball = Shapes::createCircle(space, spawn.x, spawn.y, ballRadius, 1.0f);
            launchBall(space, ball.index, spawn, velocity);
            balls[numBalls++] = ball.index;
            game.updateBoundingBoxes();
            return ball.index;
        }

        const int ballIndex = balls[nextBall];
        nextBall = (nextBall + 1) % maxBalls;

        if (ballIndex < space.shapes.size() && space.shapes[ballIndex].end - space.shapes[ballIndex].start == 16)
        {
            launchBall(space, ballIndex, spawn, velocity);
            return ballIndex;
        }

        return -1;
    }

    int numFiredBalls() const
    {
        return numBalls;
    }

    int baseShape() const
    {
        return baseShapeIndex;
    }

    int barrelShape() const
    {
        return barrelShapeIndex;
    }

    int firedBall(int i) const
    {
        return balls[i];
    }

private:
    static void launchBall(PhysicsSpace &space, int ballIndex, Vector2 center, Vector2 velocity)
    {
        placeShape(space, ballIndex, center, velocity);
    }

    int baseShapeIndex = -1;
    int barrelShapeIndex = -1;
    Vector2 barrelRestOffsets[4]; // Barrel corners relative to the pivot, pointing right
    float angle = 0.0f;
    int balls[maxBalls] = {};
    int numBalls = 0;
    int nextBall = 0;
};

// A crosshair at the mouse over empty space, where a click fires. Over a shape the hand cursor shows instead.
static void drawCrosshair(Game &game, SceneCanvas &canvas)
{
    if (!game.mouseOverCanvas() || game.hoveredShapeIndex() != -1)
    {
        return;
    }

    const Vector2 center = game.mouseWorldPos();
    const float pixel = canvas.pixel();
    const Vector2 directions[4] = {Vector2(1.0f, 0.0f), Vector2(-1.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(0.0f, -1.0f)};

    // A white halo under the dark lines keeps it visible over dark shapes
    for (int pass = 0; pass < 2; pass++)
    {
        const CanvasColor color = pass == 0 ? CanvasColor::hex(0xFFFFFF) : CanvasColor::hex(0x22303F);
        const float width = pass == 0 ? 4.5f : 2.0f;
        canvas.ring(center, 11.0f * pixel, width, color);

        for (int d = 0; d < 4; d++)
        {
            canvas.line(center + directions[d] * (5.0f * pixel), center + directions[d] * (17.0f * pixel), width, color);
        }
    }
}

// The downpour scene: a mass of balls, boxes and bridge-like strips in all sizes rains down without end.
// Once landed, rollers roll, hoppers hop and strips crawl the way they were thrown, turning back at the
// walls, while boxes just tumble. When the arena holds as many as it can, each new arrival
// pops the oldest one in an explosion that flings everything near it away.
class DownpourScript : public SceneScript
{
public:
    explicit DownpourScript(Game &game)
    {
        PhysicsSpace &space = game.physicsSpace();

        // Wide walls, and a thick floor that runs under them. With the floor ending at the walls' inner faces,
        // things squeezed out low through a wall had nothing underneath and slid away.
        Shapes::createStaticQuad(space, 4.0f - wallWidth, 0.0f, wallWidth, groundTop, 1.0f);
        Shapes::createStaticQuad(space, arenaRight, 0.0f, wallWidth, groundTop, 1.0f);
        Shapes::createStaticQuad(space, 4.0f - wallWidth, groundTop, arenaRight + wallWidth * 2.0f - 4.0f, floorThickness, 1.0f);
    }

    void update(Game &game, float elapsedMs) override
    {
        PhysicsSpace &space = game.physicsSpace();

        if (game.paused())
        {
            return;
        }

        timeToNextSpawnMs -= elapsedMs;

        if (timeToNextSpawnMs <= 0.0f)
        {
            spawn(game);
            timeToNextSpawnMs = spawnIntervalMs;
        }

        // The shape being dragged with the mouse, if any
        const int draggedShape = game.dragging() ? game.shapeUnderMouse() : -1;

        for (int i = 0; i < things.size(); i++)
        {
            if (things[i].active)
            {
                updateThing(space, things[i], i, elapsedMs, draggedShape);
            }
        }

        for (int i = 0; i < bursts.size(); i++)
        {
            bursts[i].ageMs += elapsedMs;

            if (bursts[i].ageMs >= burstDurationMs)
            {
                bursts.remove(i);
                i--;
            }
        }
    }

    void drawOverlay(Game &game, SceneCanvas &canvas) override
    {
        for (int i = 0; i < bursts.size(); i++)
        {
            drawBurst(canvas, bursts[i].pos, bursts[i].radius, bursts[i].ageMs / burstDurationMs, CanvasColor::hex(bursts[i].color));
        }
    }

    bool shapeFill(int shapeIndex, uint32_t &color) override
    {
        for (int i = 0; i < things.size(); i++)
        {
            if (shapeIndex >= things[i].shapeStart && shapeIndex < things[i].shapeStart + things[i].shapeCount)
            {
                color = kindColors[(int)things[i].kind];
                return true;
            }
        }

        return false;
    }

private:
    enum class Kind
    {
        Roller,  // A ball that keeps rolling once it lands
        Hopper,  // A ball that keeps bouncing along
        Box,     // A box that falls and tumbles like any other
        Strip,   // A strip of bridge segments that crawls once it lands
        Count
    };

    static const int numSizes = 3; // Small, medium and large

    // One or more shapes with their points in one range: a ball or a box, or the segments of a strip,
    // which share points. Moved as a whole by placing a copy of the pose it was made in.
    struct Thing
    {
        Kind kind;
        int size;
        bool active;
        int shapeStart;
        int shapeCount;
        int pointStart;
        int pointEnd;
        int templateStart; // Its points' offsets from its center in the pose it was made in, in templateOffsets
        float speed;
        float direction; // -1 or 1, the way it moves along the ground
        float ageMs;
        float hopCooldownMs;
        float sinceDraggedMs; // Time since it was last dragged with the mouse
    };

    struct Burst
    {
        Vector2 pos;
        float radius;
        uint32_t color;
        float ageMs;
    };

    static constexpr float groundTop = 755.0f;
    static constexpr float arenaRight = 1214.0f;
    static constexpr float wallWidth = 100.0f;
    static constexpr float floorThickness = 250.0f;
    // Things start between the walls, below their tops, so none are thrown over them. Far enough in from the
    // walls that the widest strip starts inside.
    static constexpr float spawnLeft = 130.0f;
    static constexpr float spawnRight = arenaRight - 125.0f;
    static constexpr float spawnY = 80.0f;
    static constexpr float spawnIntervalMs = 140.0f;
    static constexpr float burstDurationMs = 400.0f;
    static constexpr float maxSpeed = 3.0f;   // A safety net; crowds can't fling things faster than this
    static constexpr float explosionRadius = 110.0f; // For a medium thing; scaled by size
    static constexpr float explosionImpulse = 1.0f;  // Velocity change times mass at the center, for a medium thing
    // Gravity is gentle, so this is enough to throw things about the height of the arena
    static constexpr float maxExplosionVelocityChange = 0.5f;
    static constexpr float maxUpwardSpeed = 0.45f; // Enough to rise about the height of the arena
    static constexpr float coastSpeed = 0.35f; // Faster than any thing moves by itself, so it has been flung or knocked; it coasts
    static const int maxThings = 120;
    static constexpr uint32_t kindColors[(int)Kind::Count] = {0xE85D4F, 0xF39A33, 0x9B6BD8, 0x2FA6A0};
    static constexpr float sizeScales[numSizes] = {0.845f, 1.3f, 2.08f};
    static constexpr float massScale = 1.4f; // On every kind's mass
    // Well below the usual stiffness and half the damping, so things visibly squash and wobble. Much softer than
    // about 0.4 stiffness, the bottom of a full arena gets crushed flat and bodies turn inside out.
    static constexpr float stiffness = 0.625f; // On every shape's stiffness
    static constexpr float damping = 0.5f;     // On every shape's damping

    // Small deterministic random numbers, so it rains the same way from a reset
    float random()
    {
        randomState = randomState * 1664525u + 1013904223u;
        return static_cast<float>(randomState >> 8) / 16777216.0f;
    }

    Kind chooseKind()
    {
        const float r = random();
        return r < 0.2f ? Kind::Box : (r < 0.35f ? Kind::Strip : (r < 0.65f ? Kind::Hopper : Kind::Roller));
    }

    // Mostly medium, some small and some large
    int chooseSize()
    {
        const float r = random();
        return r < 0.3f ? 0 : (r < 0.8f ? 1 : 2);
    }

    // Makes the shapes for a new thing around pos and records the pose to place it with later
    Thing create(Game &game, Kind kind, int size, Vector2 pos)
    {
        PhysicsSpace &space = game.physicsSpace();
        const float scale = sizeScales[size];
        Thing thing = {};
        thing.kind = kind;
        thing.size = size;
        thing.shapeStart = space.shapes.size();
        thing.pointStart = space.points.size();

        switch (kind)
        {
        case Kind::Roller:
            Shapes::createCircle(space, pos.x, pos.y, 18.0f * scale, 1.0f * massScale);
            break;
        case Kind::Hopper:
            Shapes::createCircle(space, pos.x, pos.y, 15.0f * scale, 0.8f * massScale);
            break;
        case Kind::Box:
            Shapes::createQuad(space, pos.x - 15.0f * scale, pos.y - 15.0f * scale, 30.0f * scale, 30.0f * scale, 2.0f * massScale);
            break;
        case Kind::Strip:
        {
            const int segments = 3 + size * 2;
            const float segmentWidth = 24.0f * 1.3f;
            Shapes::createBridge(space, pos.x - segmentWidth * static_cast<float>(segments) * 0.5f, pos.y, 0.6f * massScale, segments, segmentWidth, 12.0f * 1.3f);
            break;
        }
        case Kind::Count:
            break;
        }

        thing.shapeCount = space.shapes.size() - thing.shapeStart;
        thing.pointEnd = space.points.size();

        // Softer than the shapes are made by default, and less damped, so they squash and wobble
        for (int s = thing.shapeStart; s < thing.shapeStart + thing.shapeCount; s++)
        {
            space.shapes[s].stiffness *= stiffness;
            space.shapes[s].damping *= damping;
        }

        thing.templateStart = templateOffsets.size();
        const Vector2 center = groupCenter(space, thing);

        for (int i = thing.pointStart; i < thing.pointEnd; i++)
        {
            templateOffsets.push(space.points.pos[i] - center);
        }

        game.updateBoundingBoxes();
        return thing;
    }

    void spawn(Game &game)
    {
        PhysicsSpace &space = game.physicsSpace();
        const Kind kind = chooseKind();
        const int size = chooseSize();

        // From anywhere along the top, thrown in any direction from 20 degrees below horizontal to straight down
        const Vector2 pos(spawnLeft + random() * (spawnRight - spawnLeft), spawnY);
        const float throwAngle = (20.0f + random() * 140.0f) * PI_F / 180.0f;
        const Vector2 throwDirection(cosf(throwAngle), sinf(throwAngle));
        const float throwSpeed = 0.05f + random() * 0.35f;

        // A thing of the same kind and size that is out of play, or a new one, or when the arena is full,
        // the oldest one of that kind and size, popped
        int slot = -1;
        int oldest = -1;

        for (int i = 0; i < things.size() && slot == -1; i++)
        {
            if (things[i].kind != kind || things[i].size != size)
            {
                continue;
            }

            if (!things[i].active)
            {
                slot = i;
            }
            else if (oldest == -1 || things[i].ageMs > things[oldest].ageMs)
            {
                oldest = i;
            }
        }

        if (slot == -1 && things.size() < maxThings)
        {
            things.push(create(game, kind, size, pos));
            slot = things.size() - 1;
        }
        else if (slot == -1 && oldest != -1)
        {
            slot = oldest;
            remove(space, things[slot], slot);
        }

        if (slot == -1)
        {
            return;
        }

        Thing &thing = things[slot];
        thing.active = true;
        thing.speed = (size == 2 ? 0.75f : 1.0f) * 0.15f;
        thing.direction = throwDirection.x < 0.0f ? -1.0f : 1.0f;
        thing.ageMs = 0.0f;
        thing.hopCooldownMs = 0.0f;
        thing.sinceDraggedMs = 1e9f;
        place(space, thing, pos, throwDirection * throwSpeed);
    }

    Vector2 groupCenter(const PhysicsSpace &space, const Thing &thing) const
    {
        Vector2 center;

        for (int i = thing.pointStart; i < thing.pointEnd; i++)
        {
            center += space.points.pos[i];
        }

        return center / static_cast<float>(thing.pointEnd - thing.pointStart);
    }

    Vector2 groupVelocity(const PhysicsSpace &space, const Thing &thing) const
    {
        Vector2 velocity;

        for (int i = thing.pointStart; i < thing.pointEnd; i++)
        {
            velocity += space.points.velocity[i];
        }

        return velocity / static_cast<float>(thing.pointEnd - thing.pointStart);
    }

    // Puts the thing in its original pose around center, moving at velocity, and into play
    void place(PhysicsSpace &space, const Thing &thing, Vector2 center, Vector2 velocity)
    {
        for (int i = thing.pointStart; i < thing.pointEnd; i++)
        {
            space.points.pos[i] = center + templateOffsets[thing.templateStart + (i - thing.pointStart)];
            space.points.velocity[i] = velocity;
        }

        for (int s = thing.shapeStart; s < thing.shapeStart + thing.shapeCount; s++)
        {
            space.shapes[s].isStatic = false;
        }
    }

    // Takes the thing out of play, frozen out of sight. Placed from its template rather than moved, so this
    // also cleans up a thing whose points have gone bad.
    void park(PhysicsSpace &space, Thing &thing, int slot)
    {
        place(space, thing, Vector2(-5000.0f - static_cast<float>(slot) * 150.0f, 0.0f), Vector2());

        for (int s = thing.shapeStart; s < thing.shapeStart + thing.shapeCount; s++)
        {
            space.shapes[s].isStatic = true;
        }

        thing.active = false;
    }

    // Pops the thing in an explosion that flings everything near it away, further for bigger things
    void remove(PhysicsSpace &space, Thing &thing, int slot)
    {
        const Vector2 center = groupCenter(space, thing);
        const float blastRadius = explosionRadius * sizeScales[thing.size];
        park(space, thing, slot);

        if (!isnan(center.x) && !isnan(center.y))
        {
            pushAwayFrom(space, center, blastRadius, explosionImpulse * sizeScales[thing.size], maxExplosionVelocityChange);
            bursts.push({center, blastRadius * 0.8f, kindColors[(int)thing.kind], 0.0f});
        }
    }

    // Steers the thing by changing the velocity of all its points alike, so its shape is left alone. Not
    // while it is dragged or still moving fast after being flung, so it flies like it would in any scene.
    void updateThing(PhysicsSpace &space, Thing &thing, int slot, float elapsedMs, int draggedShape)
    {
        if (thing.shapeStart + thing.shapeCount > space.shapes.size())
        {
            thing.active = false;
            return;
        }

        const Vector2 center = groupCenter(space, thing);

        // A soft body crushed hard enough can go bad; take it out rather than let it linger
        if (isnan(center.x) || isnan(center.y))
        {
            park(space, thing, slot);
            return;
        }

        const Vector2 velocity = groupVelocity(space, thing);
        const float steer = min(1.0f, elapsedMs / 300.0f);
        thing.ageMs += elapsedMs;
        thing.hopCooldownMs -= elapsedMs;

        const bool dragged = draggedShape >= thing.shapeStart && draggedShape < thing.shapeStart + thing.shapeCount;
        thing.sinceDraggedMs = dragged ? 0.0f : thing.sinceDraggedMs + elapsedMs;

        // Pops in quick succession and squeezes in the pile can launch things far out of sight, gravity being
        // gentle, so they don't go up faster than this. Except just after being thrown with the mouse.
        if (thing.sinceDraggedMs > 2000.0f && velocity.y < -maxUpwardSpeed)
        {
            for (int i = thing.pointStart; i < thing.pointEnd; i++)
            {
                space.points.velocity[i].y += -maxUpwardSpeed - velocity.y;
            }
        }

        // Flung or squeezed out over a wall: popped like an old one
        if (!dragged && (center.x < 0.0f || center.x > arenaRight || center.y > groundTop + 50.0f))
        {
            remove(space, thing, slot);
            return;
        }

        if (dragged || velocity.length() > coastSpeed)
        {
            if (!dragged && velocity.length() > maxSpeed)
            {
                const Vector2 change = velocity * (maxSpeed / velocity.length()) - velocity;

                for (int i = thing.pointStart; i < thing.pointEnd; i++)
                {
                    space.points.velocity[i] += change;
                }
            }

            return;
        }

        float lowestY = -1e9f;

        for (int i = thing.pointStart; i < thing.pointEnd; i++)
        {
            lowestY = max(lowestY, space.points.pos[i].y);
        }

        // Turn back at the walls
        if ((center.x < 80.0f && thing.direction < 0.0f) || (center.x > arenaRight - 80.0f && thing.direction > 0.0f))
        {
            thing.direction = -thing.direction;
        }

        const bool onGround = lowestY > groundTop - 8.0f;
        Vector2 change;

        if (thing.kind == Kind::Box)
        {
            return;
        }

        if (onGround)
        {
            change.x = (thing.direction * thing.speed - velocity.x) * steer;

            if (thing.kind == Kind::Hopper && velocity.y > -0.05f && thing.hopCooldownMs <= 0.0f)
            {
                change.y = -0.32f - velocity.y;
                thing.hopCooldownMs = 500.0f;
            }
        }

        const Vector2 newVelocity = velocity + change;

        if (newVelocity.length() > maxSpeed)
        {
            change = newVelocity * (maxSpeed / newVelocity.length()) - velocity;
        }

        for (int i = thing.pointStart; i < thing.pointEnd; i++)
        {
            space.points.velocity[i] += change;
        }
    }

    float timeToNextSpawnMs = 500.0f;
    uint32_t randomState = 12345u;
    Array<Thing> things;
    Array<Vector2> templateOffsets;
    Array<Burst> bursts;
};

// The fortress scene: the cannon on the left and a fortress far to the right, out of view while
// aiming. The camera starts on the fortress, pans to the cannon, follows each shot, then returns. The
// distance from the cannon to the crosshair sets the power, and dots show where the shot will go.
// B switches between balls and bombs, which go off when they hit something or are clicked again.
class FortressScript : public SceneScript
{
public:
    explicit FortressScript(Game &game)
    {
        PhysicsSpace &space = game.physicsSpace();
        const float g = groundTop;
        space.gravity = gravity;

        Shapes::createStaticQuad(space, worldLeft - 40.0f, -1200.0f, 40.0f, g + 1200.0f, 1.0f);
        Shapes::createStaticQuad(space, worldRight, -1200.0f, 40.0f, g + 1200.0f, 1.0f);
        Shapes::createStaticQuad(space, worldLeft - 40.0f, g, worldRight - worldLeft + 80.0f, 200.0f, 1.0f);

        cannon.create(space, Vector2(60.0f, g - 90.0f), -35.0f * PI_F / 180.0f);

        // Two storeys of columns and planks, with blocks inside and a block on top. Heavier than the
        // cannon scene's boxes, so it takes several good shots to bring down.
        const float columnMass = 8.0f;
        const float plankMass = 10.0f;
        const float blockMass = 4.0f;

        addBlock(space, 1740.0f, g - 151.0f, 30.0f, 150.0f, columnMass);
        addBlock(space, 1890.0f, g - 151.0f, 30.0f, 150.0f, columnMass);
        addBlock(space, 2040.0f, g - 151.0f, 30.0f, 150.0f, columnMass);
        addBlock(space, 1805.0f, g - 51.0f, 50.0f, 50.0f, blockMass);
        addBlock(space, 1805.0f, g - 102.0f, 50.0f, 50.0f, blockMass);
        addBlock(space, 1955.0f, g - 51.0f, 50.0f, 50.0f, blockMass);
        addBlock(space, 1955.0f, g - 102.0f, 50.0f, 50.0f, blockMass);
        addBlock(space, 1725.0f, g - 178.0f, 360.0f, 26.0f, plankMass);

        addBlock(space, 1800.0f, g - 299.0f, 30.0f, 120.0f, columnMass);
        addBlock(space, 1980.0f, g - 299.0f, 30.0f, 120.0f, columnMass);
        addBlock(space, 1880.0f, g - 229.0f, 50.0f, 50.0f, blockMass);
        addBlock(space, 1880.0f, g - 280.0f, 50.0f, 50.0f, blockMass);
        addBlock(space, 1785.0f, g - 326.0f, 240.0f, 26.0f, plankMass);

        addBlock(space, 1875.0f, g - 387.0f, 60.0f, 60.0f, blockMass);

        updateCamera(game, 0.0f, true);
    }

    void update(Game &game, float frameMs) override
    {
        PhysicsSpace &space = game.physicsSpace();

        if (!cannon.isValid(space))
        {
            return;
        }

        // Game time stands still while paused, but the camera keeps easing
        const float elapsedMs = game.paused() ? 0.0f : frameMs;
        modeTimeMs += elapsedMs;
        updateCamera(game, frameMs, false);
        updateBlasts(elapsedMs);

        // Aim at the crosshair, harder the further away it is
        const bool aiming = mode == Mode::Aim && game.mouseOverCanvas();

        if (aiming)
        {
            cannon.aimAt(game.mouseWorldPos(), space);
        }

        cannon.updateBarrel(space);

        const float power = clamp((game.mouseWorldPos() - cannon.pivot(space)).length() / fullPowerDistance, 0.2f, 1.0f);
        const float speed = power * maxBallSpeed;
        updateGuideDots(space, aiming, speed);

        switch (mode)
        {
        case Mode::Intro:
            if (modeTimeMs > 1800.0f)
            {
                setMode(Mode::Aim);
            }
            break;
        case Mode::Aim:
            if (game.keyWasPressed(GameKeyCode::B))
            {
                bombSelected = !bombSelected;
            }

            if (!game.paused() && (game.mouseWasClicked() || game.keyWasPressed(GameKeyCode::SPACE)))
            {
                fire(game, speed);
            }
            break;
        case Mode::Flight:
            updateFlight(game, elapsedMs);
            break;
        case Mode::Linger:
            if (modeTimeMs > 1000.0f)
            {
                setMode(Mode::Aim);
            }
            break;
        }

        updateStatus(space);
    }

    void drawOverlay(Game &game, SceneCanvas &canvas) override
    {
        for (int i = 0; i < blasts.size(); i++)
        {
            drawBurst(canvas, blasts[i].pos, blastRadius, blasts[i].ageMs / blastDurationMs, {1.0f, 0.62f, 0.2f, 1.0f});
        }

        // Dots along the coming shot, getting smaller along the way
        const float pixel = canvas.pixel();

        for (int i = 0; i < guideDots.size(); i++)
        {
            const float size = 1.0f - 0.5f * static_cast<float>(i) / static_cast<float>(max(guideDots.size() - 1, 1));
            canvas.disc(guideDots[i], 5.5f * size * pixel, CanvasColor::hex(0x22303F));
            canvas.disc(guideDots[i], 3.8f * size * pixel, CanvasColor::hex(0xFFFFFF));
        }

        drawCrosshair(game, canvas);
    }

    bool shapeFill(int shapeIndex, uint32_t &color) override
    {
        for (int i = 0; i < bombBalls.size(); i++)
        {
            if (bombBalls[i] == shapeIndex)
            {
                color = bombColor;
                return true;
            }
        }

        return false;
    }

    const char *statusText() override
    {
        return status.data;
    }

    bool hidesCursor() override
    {
        return true;
    }

private:
    enum class Mode
    {
        Intro,  // Showing the fortress before the first shot
        Aim,    // At the cannon, ready to fire
        Flight, // Following the last shot
        Linger  // Looking at where it landed before returning
    };

    struct Blast
    {
        Vector2 pos;
        float ageMs;
    };

    static const int maxBlocks = 32;
    static constexpr float worldLeft = -300.0f;
    static constexpr float worldRight = 2900.0f;
    static constexpr float groundTop = 755.0f;
    static constexpr float fortressCenterX = 1900.0f;
    // Three times the usual gravity, so a long shot takes a couple of seconds rather than five. The
    // ball is faster to match, by the square root of that.
    static constexpr float gravity = PhysicsSpace::defaultGravity * 3.0f;
    static constexpr float maxBallSpeed = 0.75f * 1.732f;
    static constexpr float fullPowerDistance = 400.0f; // Crosshair distance from the cannon for full power
    static const uint32_t bombColor = 0x3A4250;
    static constexpr float blastRadius = 220.0f;
    static constexpr float blastImpulse = 4.0f; // Velocity change times mass, at the center of the blast
    static constexpr float blastMaxVelocityChange = 2.0f;
    static constexpr float blastDurationMs = 450.0f;
    static constexpr float bombFuseMs = 4000.0f;

    void addBlock(PhysicsSpace &space, float x, float y, float width, float height, float mass)
    {
        Shape block = Shapes::createQuad(space, x, y, width, height, mass);

        if (numBlocks < maxBlocks)
        {
            blocks[numBlocks] = block.index;
            blockStartCenters[numBlocks] = shapeCenter(space, block);
            numBlocks++;
        }
    }

    void setMode(Mode newMode)
    {
        mode = newMode;
        modeTimeMs = 0.0f;
    }

    bool isBall(const PhysicsSpace &space, int shapeIndex) const
    {
        return shapeIndex >= 0 && shapeIndex < space.shapes.size();
    }

    void fire(Game &game, float speed)
    {
        PhysicsSpace &space = game.physicsSpace();
        followBall = cannon.fire(game, speed);
        shots++;
        lingerAtBlast = false;
        bombLive = bombSelected && followBall != -1;
        bombAgeMs = 0.0f;

        // A ball keeps its bomb colour until it is fired as a plain ball again
        for (int i = 0; i < bombBalls.size(); i++)
        {
            if (bombBalls[i] == followBall)
            {
                bombBalls.remove(i);
                break;
            }
        }

        if (bombLive)
        {
            bombBalls.push(followBall);
            bombPreviousVelocity = shapeVelocity(space, space.shapes[followBall]);
        }

        setMode(Mode::Flight);
    }

    void updateFlight(Game &game, float elapsedMs)
    {
        PhysicsSpace &space = game.physicsSpace();

        // A bomb goes off when it hits something, which shows as a sudden change in its velocity, when
        // clicked again, or when its fuse runs out
        if (bombLive && !game.paused() && isBall(space, followBall))
        {
            // Measured per millisecond so it doesn't depend on the frame rate. Gravity alone changes
            // the velocity by about a tenth of the threshold.
            const Vector2 velocity = shapeVelocity(space, space.shapes[followBall]);
            const float velocityChangeRate = elapsedMs > 0.0f ? (velocity - bombPreviousVelocity).length() / elapsedMs : 0.0f;
            const bool impact = bombAgeMs > 100.0f && velocityChangeRate > 0.004f;
            const bool detonated = game.mouseWasClicked() || game.keyWasPressed(GameKeyCode::SPACE);
            bombAgeMs += elapsedMs;
            bombPreviousVelocity = velocity;

            if (impact || detonated || bombAgeMs > bombFuseMs)
            {
                explodeBomb(space);
                setMode(Mode::Linger);
                return;
            }
        }

        // Until the shot has slowed down or has had long enough. Measured on the ball as a whole, since
        // a rolling ball's points keep moving, and not until it stops, since a ball can roll a long way.
        const bool slowedDown = !isBall(space, followBall) || shapeVelocity(space, space.shapes[followBall]).length() < 0.12f;

        if ((slowedDown && modeTimeMs > 1200.0f) || modeTimeMs > 4500.0f)
        {
            setMode(Mode::Linger);
        }
    }

    // Pushes every moving point near the bomb away from it, less with distance and on heavier points,
    // then parks the bomb until the cannon fires it again
    void explodeBomb(PhysicsSpace &space)
    {
        const Vector2 center = shapeCenter(space, space.shapes[followBall]);
        space.shapes[followBall].isStatic = true; // Not pushed by its own blast; parked below
        pushAwayFrom(space, center, blastRadius, blastImpulse, blastMaxVelocityChange);
        blasts.push({center, 0.0f});
        parkShape(space, followBall, Vector2(worldLeft - 5000.0f - static_cast<float>(numParkedBombs % Cannon::maxBalls) * 100.0f, 0.0f));
        numParkedBombs++;
        bombLive = false;
        lingerAtBlast = true;
        blastPos = center;
    }

    void updateBlasts(float elapsedMs)
    {
        for (int i = 0; i < blasts.size(); i++)
        {
            blasts[i].ageMs += elapsedMs;

            if (blasts[i].ageMs >= blastDurationMs)
            {
                blasts.remove(i);
                i--;
            }
        }
    }

    // Dots along the path the shot will take, until it reaches the ground
    void updateGuideDots(const PhysicsSpace &space, bool aiming, float speed)
    {
        guideDots.clear();

        if (!aiming)
        {
            return;
        }

        Vector2 spawn;
        Vector2 direction;
        cannon.muzzle(space, spawn, direction);
        const Vector2 velocity = direction * speed;

        for (int k = 1; k <= 14; k++)
        {
            const float t = static_cast<float>(k) * 40.0f;
            const Vector2 dot = spawn + velocity * t + Vector2(0.0f, 0.5f * space.gravity * t * t);

            if (dot.y > groundTop)
            {
                break;
            }

            guideDots.push(dot);
        }
    }

    // Where the camera wants to be, as the world x at the middle of the screen
    float cameraTarget(const PhysicsSpace &space, float viewWidth) const
    {
        switch (mode)
        {
        case Mode::Intro:
            return fortressCenterX;
        case Mode::Flight:
        case Mode::Linger:
            if (lingerAtBlast)
            {
                return blastPos.x;
            }

            if (isBall(space, followBall))
            {
                return shapeCenter(space, space.shapes[followBall]).x;
            }
            break;
        case Mode::Aim:
            break;
        }

        // The cannon about a quarter in from the left
        return cannon.pivot(space).x + viewWidth * 0.25f;
    }

    void updateCamera(Game &game, float elapsedMs, bool snap)
    {
        const Vector2 viewSize = game.viewSize();
        const float scale = clamp(viewSize.y / 1000.0f, 0.35f, 1.2f);
        const float viewWidth = viewSize.x / scale;

        float target = cameraTarget(game.physicsSpace(), viewWidth);
        const float minX = worldLeft + viewWidth * 0.5f;
        const float maxX = worldRight - viewWidth * 0.5f;
        target = minX < maxX ? clamp(target, minX, maxX) : (worldLeft + worldRight) * 0.5f;

        // Eases towards the target; following a shot is quicker so it stays in view
        const float easeMs = mode == Mode::Flight ? 120.0f : 350.0f;
        cameraX = snap ? target : cameraX + (target - cameraX) * (1.0f - expf(-elapsedMs / easeMs));

        game.scale() = scale;
        game.offset() = Vector2(viewSize.x * 0.5f - cameraX * scale, viewSize.y - (groundTop + 90.0f) * scale);
    }

    // A block has fallen once it has moved well away from where it started, and stays fallen even if pushed back
    void updateStatus(const PhysicsSpace &space)
    {
        int fallen = 0;

        for (int i = 0; i < numBlocks; i++)
        {
            if (!blockFallen[i] && blocks[i] < space.shapes.size() &&
                (shapeCenter(space, space.shapes[blocks[i]]) - blockStartCenters[i]).length() > 30.0f)
            {
                blockFallen[i] = true;
            }

            fallen += blockFallen[i] ? 1 : 0;
        }

        status.clear();

        if (fallen == numBlocks)
        {
            status.append("Fortress destroyed in %d shots! Reset to play again", shots);
        }
        else
        {
            status.append("Shots: %d     Fortress knocked down: %d of %d     Ammo: %s (B to switch)", shots, fallen, numBlocks,
                          bombSelected ? "bomb" : "ball");
        }
    }

    Cannon cannon;
    Mode mode = Mode::Intro;
    float modeTimeMs = 0.0f;
    float cameraX = 0.0f;
    int followBall = -1;
    int shots = 0;

    int blocks[maxBlocks] = {};
    Vector2 blockStartCenters[maxBlocks];
    bool blockFallen[maxBlocks] = {};
    int numBlocks = 0;

    bool bombSelected = false; // Switched with B
    bool bombLive = false;     // The followed ball is a bomb that hasn't gone off yet
    float bombAgeMs = 0.0f;
    Vector2 bombPreviousVelocity;
    Array<int> bombBalls; // Balls drawn in the bomb colour
    int numParkedBombs = 0;
    bool lingerAtBlast = false;
    Vector2 blastPos;
    Array<Blast> blasts;

    Array<Vector2> guideDots;
    StringBuffer<128> status;
};

void initDownpourScene(Game *game)
{
    game->setScript(new DownpourScript(*game));
}

void initFortressScene(Game *game)
{
    game->setScript(new FortressScript(*game));
}
