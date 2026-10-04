#include "ChaosScene.h"
#include <math.h>
#include "Game.h"
#include "SceneScript.h"
#include "Shapes.h"
#include "../physics/PhysicsSpace.h"
#include "../containers/Array.h"
#include "../utils/MinMax.h"

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

// Between colours a and b, as 0xRRGGBB, t of the way to b
static uint32_t mixHex(uint32_t a, uint32_t b, float t)
{
    uint32_t mixed = 0;

    for (int shift = 0; shift <= 16; shift += 8)
    {
        const float from = static_cast<float>((a >> shift) & 0xFF);
        const float to = static_cast<float>((b >> shift) & 0xFF);
        mixed |= static_cast<uint32_t>(from + (to - from) * t + 0.5f) << shift;
    }

    return mixed;
}

// The chaos scene: a mass of balls, boxes and bridge-like strips in all sizes rains down without end.
// Once landed, rollers roll, hoppers hop and strips crawl the way they were thrown, turning back at the
// walls, while boxes just tumble. When the arena holds as many as it can, each new arrival
// pops the oldest one in an explosion of sparkles that flings everything near it away. New arrivals fade
// in from white.
class ChaosScript : public SceneScript
{
public:
    explicit ChaosScript(Game &game)
    {
        PhysicsSpace &space = game.physicsSpace();

        // Wide walls, and a thick floor that runs under them. With the floor ending at the walls' inner faces,
        // things squeezed out low through a wall had nothing underneath and slid away.
        Shapes::createStaticQuad(space, 4.0f - wallWidth, 0.0f, wallWidth, groundTop, 1.0f);
        Shapes::createStaticQuad(space, arenaRight, 0.0f, wallWidth, groundTop, 1.0f);
        Shapes::createStaticQuad(space, 4.0f - wallWidth, groundTop, arenaRight + wallWidth * 2.0f - 4.0f, floorThickness, 1.0f);

        // A first lot straight away, spread over the arena so none start inside each other: columns far enough
        // apart for the widest strip, rows for the biggest ball
        for (int row = 0; row < startRows; row++)
        {
            for (int column = 0; column < startColumns; column++)
            {
                const float jitter = (random() - 0.5f) * 20.0f;
                const Vector2 pos(spawnLeft + static_cast<float>(column) * (spawnRight - spawnLeft) / static_cast<float>(startColumns - 1) + jitter,
                                  spawnY + 20.0f + static_cast<float>(row) * 95.0f);
                spawnAt(game, pos);
            }
        }
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

        // Sparkles slow down and sink a little
        for (int i = 0; i < sparkles.size(); i++)
        {
            Sparkle &sparkle = sparkles[i];
            sparkle.ageMs += elapsedMs;

            if (sparkle.ageMs >= sparkle.lifeMs)
            {
                sparkles.remove(i);
                i--;
                continue;
            }

            sparkle.pos += sparkle.velocity * elapsedMs;
            sparkle.velocity = sparkle.velocity * expf(-elapsedMs / 250.0f) + Vector2(0.0f, 0.0002f * elapsedMs);
        }
    }

    void drawOverlay(Game &game, SceneCanvas &canvas) override
    {
        for (int i = 0; i < bursts.size(); i++)
        {
            drawBurst(canvas, bursts[i].pos, bursts[i].radius, bursts[i].ageMs / burstDurationMs, CanvasColor::hex(bursts[i].color));
        }

        // Sparkles from explosions, shrinking and fading as they go
        for (int i = 0; i < sparkles.size(); i++)
        {
            const Sparkle &sparkle = sparkles[i];
            const float t = sparkle.ageMs / sparkle.lifeMs;
            CanvasColor color = CanvasColor::hex(sparkle.color);
            color.a = 1.0f - t;
            canvas.disc(sparkle.pos, sparkle.sizePixels * (1.0f - 0.5f * t) * canvas.pixel(), color);
        }
    }

    bool shapeFill(int shapeIndex, uint32_t &color) override
    {
        for (int i = 0; i < things.size(); i++)
        {
            if (shapeIndex >= things[i].shapeStart && shapeIndex < things[i].shapeStart + things[i].shapeCount)
            {
                // New arrivals start out white and fade into their colour, eased in and out
                if (things[i].active && things[i].ageMs < appearDurationMs)
                {
                    const float t = things[i].ageMs / appearDurationMs;
                    color = mixHex(0xFFFFFF, kindColors[(int)things[i].kind], t * t * (3.0f - 2.0f * t));
                }
                else
                {
                    color = kindColors[(int)things[i].kind];
                }
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

    // A small dot thrown out from an exploding thing's outline
    struct Sparkle
    {
        Vector2 pos;
        Vector2 velocity;
        float sizePixels;
        float ageMs;
        float lifeMs;
        uint32_t color;
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
    static constexpr float spawnIntervalMs = 140.0f / 1.5f;
    static const int startColumns = 5; // 30 things to start with
    static const int startRows = 6;
    static constexpr float burstDurationMs = 400.0f;
    static constexpr float appearDurationMs = 400.0f;
    static const int sparklesPerExplosion = 16;
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

    // From anywhere along the top
    void spawn(Game &game)
    {
        spawnAt(game, Vector2(spawnLeft + random() * (spawnRight - spawnLeft), spawnY));
    }

    // A random thing at pos, thrown in any direction from 20 degrees below horizontal to straight down
    void spawnAt(Game &game, Vector2 pos)
    {
        PhysicsSpace &space = game.physicsSpace();
        const Kind kind = chooseKind();
        const int size = chooseSize();
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

    // Sparkles thrown outwards from points on the thing's outline, the same size whatever its size
    void addSparkles(const PhysicsSpace &space, const Thing &thing)
    {
        const Vector2 center = groupCenter(space, thing);
        const int numPoints = thing.pointEnd - thing.pointStart;
        const uint32_t color = mixHex(kindColors[(int)thing.kind], 0xFFFFFF, 0.35f);

        for (int k = 0; k < sparklesPerExplosion; k++)
        {
            const Vector2 from = space.points.pos[thing.pointStart + static_cast<int>(random() * static_cast<float>(numPoints)) % numPoints];
            const float angle = random() * 2.0f * PI_F;
            const Vector2 outwards = ((from - center).normalized() + Vector2(cosf(angle), sinf(angle)) * 0.6f).normalized();
            sparkles.push({from, outwards * (0.15f + random() * 0.25f), 2.0f + random() * 2.5f, 0.0f, 350.0f + random() * 300.0f, color});
        }
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
        const bool valid = !isnan(center.x) && !isnan(center.y);

        if (valid)
        {
            addSparkles(space, thing);
        }

        park(space, thing, slot);

        if (valid)
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
    Array<Sparkle> sparkles;
};

void initChaosScene(Game *game)
{
    game->setScript(new ChaosScript(*game));
}
