#include "BoidsScene.h"
#include <math.h>
#include "Game.h"
#include "SceneScript.h"
#include "Shapes.h"
#include "../physics/PhysicsSpace.h"
#include "../containers/Array.h"
#include "../utils/MinMax.h"
#include "../imgui/imgui.h"

// Whether the segment from e to l passes through the circle at c with radius r. From
// http://stackoverflow.com/questions/1073336/circle-line-collision-detection
static bool lineIntersectsCircle(Vector2 e, Vector2 l, Vector2 c, float r)
{
    const Vector2 d = l - e;
    const Vector2 f = e - c;
    const float a = d.dot(d);
    const float b = 2.0f * f.dot(d);
    const float discriminant = b * b - 4.0f * a * (f.dot(f) - r * r);

    if (a == 0.0f || discriminant < 0.0f)
    {
        return false;
    }

    // Either solution may be on or off the segment. A start inside the circle with the end past it
    // misses, like a segment that falls short or starts past it.
    const float root = sqrtf(discriminant);
    const float t1 = (-b - root) / (2.0f * a);
    const float t2 = (-b + root) / (2.0f * a);
    return (t1 >= 0.0f && t1 <= 1.0f) || (t2 >= 0.0f && t2 <= 1.0f);
}

// A ring t of the way through growing from radius to twice that, fading as it goes
static void drawRipple(SceneCanvas &canvas, Vector2 pos, float radius, float t, CanvasColor color)
{
    const float fade = 1.0f - clamp(t, 0.0f, 1.0f);
    color.a *= fade * fade;
    canvas.ring(pos, radius * (1.0f + t), 1.0f + 4.0f * fade, color);
}

// The numbers for the flocking rules that can be changed with sliders as the scene runs. Kept when the scene
// is reset. Most start out as the original's, scaled up to these boids' size. The original kept 20 boids apart
// in plenty of room; packed this close, its separation drowned out the other rules, so it is much weaker here,
// alignment stronger, and resting and fear of the hunters rarer, so the flock holds together.
struct BoidSettings
{
    float cohesion = 0.01f;           // Towards the middle of the neighbours, per unit of distance from it
    float alignment = 0.5f;           // Towards the neighbours' average velocity
    float separation = 0.1f;          // Away from boids that are too close, per unit of distance
    float separationDistance = 56.0f; // Closer than this is too close
    float neighbourDistance = 280.0f; // Boids this near are neighbours, for cohesion and alignment
    float maxSpeed = 14.0f;           // Distance per tick
    float turnLimitDegrees = 18.0f;   // Per tick
    float restChance = 0.002f;        // That a boid stops to rest, each tick
    // How much each tick starts from how the boid's body is really moving rather than how it meant to. Below 1,
    // a boid's intended velocity drifts from its body's, and the flock lines up in intention only.
    float bumpCarry = 1.0f;
    float hunterFear = 0.5f;          // On how hard boids keep away from the hunters
    // On the bodies' stiffness and damping as made: how hard their points are pulled back into a circle, and how
    // fast their wobbling dies down. Stiffness does most of it. At 0.6 and 0.5, as these once were, explosions and
    // crowding flung points out of line so that, one frame in 20, a body was a quarter of its radius out of round;
    // at 1 and 1 a sixth, at 2 and 2 a tenth. In between, they still squash on a hard hit but read as balls.
    // Stable up to 16 and beyond.
    float bodyStiffness = 1.5f;
    float bodyDamping = 1.75f;
};

static BoidSettings settings;

// How the isometric version is seen, from its sliders. Kept when the scene is reset. As in the classic isometric
// strategy games: the ground turned 45 degrees and seen from 30 degrees above the horizon, so it is drawn half as
// tall as it is wide.
struct IsoView
{
    float turn = PI_F * 0.25f;
    float elevation = PI_F / 6.0f;
};

static IsoView isoView;

// Through the points p1 to p2, curving to meet p0 and p3
static Vector2 catmullRom(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t)
{
    const float t2 = t * t;
    const float t3 = t2 * t;
    return (p1 * 2.0f + (p2 - p0) * t + (p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * t2 + (p1 * 3.0f - p0 - p2 * 3.0f + p3) * t3) * 0.5f;
}

// The boids scene: a port of the boids from an old JavaScript simulation, after
// http://www.kfish.org/boids/pseudocode.html, where each boid is a soft body ball. A flock keeps apart,
// lines up with and moves towards its neighbours, steers around rocks and flees from two bigger hunters
// that chase the nearest of them. Every so often one stops to rest a while. The rules run in ticks, as in
// the original, and decide how fast each boid wants to go; every frame its points are nudged together
// towards that velocity, so the bodies themselves keep apart, squash and bounce off each other. A hunter
// that catches one rests after; the one caught flashes and bolts. Clicking empty space sets off an
// explosion there that flings the boids near it away, and dragging from it sets off a trail of them.
//
// The isometric version is the same flock on the same flat ground, seen at an angle like a strategy game's
// map: the boids are balls standing on a tiled floor, the rocks domes and the walls blocks.
class BoidsScript : public SceneScript
{
public:
    BoidsScript(Game &game, bool isometric) : isometric(isometric)
    {
        PhysicsSpace &space = game.physicsSpace();
        space.gravityEnabled = false;
        firstWallShape = space.shapes.size();

        Shapes::createStaticQuad(space, arenaLeft - wallWidth, arenaTop - wallWidth, wallWidth, arenaBottom - arenaTop + wallWidth * 2.0f, 1.0f);
        Shapes::createStaticQuad(space, arenaRight, arenaTop - wallWidth, wallWidth, arenaBottom - arenaTop + wallWidth * 2.0f, 1.0f);
        Shapes::createStaticQuad(space, arenaLeft, arenaTop - wallWidth, arenaRight - arenaLeft, wallWidth, 1.0f);
        Shapes::createStaticQuad(space, arenaLeft, arenaBottom, arenaRight - arenaLeft, wallWidth, 1.0f);

        const float rocks[][3] = {{330.0f, 250.0f, 70.0f}, {870.0f, 210.0f, 55.0f}, {620.0f, 500.0f, 85.0f}, {1010.0f, 560.0f, 45.0f}, {200.0f, 590.0f, 50.0f}};

        for (const auto &rock : rocks)
        {
            Obstacle obstacle = {};
            obstacle.pos = Vector2(rock[0], rock[1]);
            obstacle.radius = rock[2];
            obstacle.shapeIndex = space.shapes.size();
            Shapes::createCircle(space, obstacle.pos.x, obstacle.pos.y, obstacle.radius, 1.0f);
            space.shapes[obstacle.shapeIndex].isStatic = true;
            obstacles.push(obstacle);
        }

        firstBoidShape = space.shapes.size();

        for (int i = 0; i < numPrey + numPredators; i++)
        {
            const bool predator = i >= numPrey;
            Boid boid = {};
            boid.type = predator ? Type::Predator : Type::Prey;
            boid.radius = predator ? predatorRadius : preyRadius * (0.85f + random() * 0.3f);
            boid.color = predator ? predatorColor : preyColors[i % numPreyColors];
            boid.pos = freePosition(boid.radius);
            boid.pointStart = space.points.size();

            const float mass = predator ? predatorMass : preyMass;
            Shapes::createCircle(space, boid.pos.x, boid.pos.y, boid.radius, mass);
            boid.pointEnd = space.points.size();

            const Shape &shape = space.shapes[firstBoidShape + i];
            baseStiffness = shape.stiffness;
            baseDamping = shape.damping;

            boid.templateStart = templateOffsets.size();

            for (int p = boid.pointStart; p < boid.pointEnd; p++)
            {
                templateOffsets.push(space.points.pos[p] - boid.pos);
            }

            const float angle = random() * 2.0f * PI_F;
            boid.velocity = Vector2::fromAngle(angle) * (2.8f + random() * 2.8f);
            boid.heading = Vector2::fromAngle(angle);
            boids.push(boid);
        }

        applyBodySettings(space);
        updateProjection();
        game.updateBoundingBoxes();
    }

    void update(Game &game, float elapsedMs) override
    {
        PhysicsSpace &space = game.physicsSpace();
        applyBodySettings(space);

        if (game.paused())
        {
            return;
        }

        // The shape being dragged with the mouse, if any
        const int draggedShape = game.dragging() ? game.shapeUnderMouse() : -1;

        for (int i = 0; i < obstacles.size(); i++)
        {
            updateObstacle(space, obstacles[i]);
        }

        for (int i = 0; i < boids.size(); i++)
        {
            Boid &boid = boids[i];
            boid.pos = center(space, boid);
            boid.dragged = draggedShape == firstBoidShape + i;

            // A soft body crushed hard enough can go bad, and one can be squeezed or thrown out of the arena;
            // put it back somewhere free
            if (isnan(boid.pos.x) || isnan(boid.pos.y) || (!boid.dragged && !insideArena(boid.pos)))
            {
                place(space, boid, freePosition(boid.radius));
            }
        }

        // The rules run in ticks of their own, as in the original. A long frame runs a few, not all of them.
        timeToNextTickMs -= elapsedMs;

        for (int ticks = 0; timeToNextTickMs <= 0.0f; ticks++)
        {
            if (ticks < 3)
            {
                tick(space);
            }

            timeToNextTickMs += tickMs;
        }

        const float steer = min(1.0f, elapsedMs / steerTimeMs);
        const float flungSteer = min(1.0f, elapsedMs / flungSteerTimeMs);
        const float turn = min(1.0f, elapsedMs / headingTimeMs);
        const float fearChange = min(1.0f, elapsedMs / fearTimeMs);

        for (int i = 0; i < boids.size(); i++)
        {
            Boid &boid = boids[i];
            boid.flashMs = max(0.0f, boid.flashMs - elapsedMs);
            boid.fear += ((boid.predatorCloseBy ? 1.0f : 0.0f) - boid.fear) * fearChange;

            if (!boid.idle && boid.velocity.length() > 0.01f)
            {
                boid.heading = Vector2::lerp(boid.heading, boid.velocity.normalized(), turn).normalized();
            }

            if (boid.dragged)
            {
                continue;
            }

            // Every point alike, so the body's shape is left to the physics. One flung faster than it can
            // swim slows down gradually, so it flies a while first.
            const Vector2 target = boid.idle ? Vector2() : boid.velocity / tickMs;
            const Vector2 velocity = groupVelocity(space, boid);
            const Vector2 change = (target - velocity) * (velocity.length() > settings.maxSpeed / tickMs ? flungSteer : steer);

            for (int p = boid.pointStart; p < boid.pointEnd; p++)
            {
                space.points.velocity[p] += change;
            }
        }

        for (int i = 0; i < ripples.size(); i++)
        {
            ripples[i].ageMs += elapsedMs;

            if (ripples[i].ageMs >= ripples[i].durationMs)
            {
                ripples.remove(i);
                i--;
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

        // Sparkles slow down as they go
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
            sparkle.velocity = sparkle.velocity * expf(-elapsedMs / 250.0f);
        }
    }

    void drawOverlay(Game &game, SceneCanvas &canvas) override
    {
        for (int i = 0; i < ripples.size(); i++)
        {
            const Ripple &ripple = ripples[i];
            drawRipple(canvas, ripple.pos, ripple.radius, ripple.ageMs / ripple.durationMs, CanvasColor::hex(ripple.color, 0.9f));
        }

        for (int i = 0; i < bursts.size(); i++)
        {
            drawBurst(canvas, bursts[i].pos, explosionRadius * 0.75f, bursts[i].ageMs / burstDurationMs, CanvasColor::hex(0xFFB040));
        }

        // Shrinking and fading as they go
        for (int i = 0; i < sparkles.size(); i++)
        {
            const Sparkle &sparkle = sparkles[i];
            const float t = sparkle.ageMs / sparkle.lifeMs;
            CanvasColor color = CanvasColor::hex(sparkle.color);
            color.a = 1.0f - t;
            canvas.disc(sparkle.pos, sparkle.sizePixels * (1.0f - 0.5f * t) * canvas.pixel(), color);
        }

        for (int i = 0; i < boids.size(); i++)
        {
            drawEyes(canvas, boids[i]);
        }
    }

    void clickedEmptySpace(Game &game, Vector2 pos) override
    {
        explode(game, pos);
    }

    void draggedOverEmptySpace(Game &game, Vector2 pos) override
    {
        if ((pos - lastExplosionPos).length() >= dragExplosionSpacing)
        {
            explode(game, pos);
        }
    }

    bool hasControls() const override
    {
        return true;
    }

    const ViewProjection *viewProjection() const override
    {
        return isometric ? &projection : nullptr;
    }

    bool drawScene(Game &game, SceneCanvas &canvas) override
    {
        if (!isometric)
        {
            return false;
        }

        drawIsometric(game, canvas);
        return true;
    }

    void drawControls(Game &game) override
    {
        if (isometric)
        {
            ImGui::SeparatorText("View");
            bool changed = ImGui::SliderAngle("Turn", &isoView.turn, -90.0f, 90.0f, "%.0f deg");
            changed |= ImGui::SliderAngle("Tilt", &isoView.elevation, 15.0f, 90.0f, "%.0f deg");

            if (changed)
            {
                updateProjection();
                game.requestFitView();
            }
        }

        ImGui::SeparatorText("Flocking");
        ImGui::SliderFloat("Alignment", &settings.alignment, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Cohesion", &settings.cohesion, 0.0f, 0.05f, "%.3f");
        ImGui::SliderFloat("Separation", &settings.separation, 0.0f, 8.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat("Personal space", &settings.separationDistance, 20.0f, 120.0f, "%.0f");
        ImGui::SliderFloat("Neighbour range", &settings.neighbourDistance, 50.0f, 500.0f, "%.0f");

        ImGui::SeparatorText("Movement");
        ImGui::SliderFloat("Top speed", &settings.maxSpeed, 2.0f, 30.0f, "%.1f");
        ImGui::SliderFloat("Turn limit", &settings.turnLimitDegrees, 2.0f, 90.0f, "%.0f deg");
        ImGui::SliderFloat("Resting", &settings.restChance, 0.0f, 0.05f, "%.3f");
        ImGui::SliderFloat("Bump carry-over", &settings.bumpCarry, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Fear of hunters", &settings.hunterFear, 0.0f, 3.0f, "%.2f");

        ImGui::SeparatorText("Bodies");
        ImGui::SliderFloat("Stiffness", &settings.bodyStiffness, 0.1f, 8.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat("Damping", &settings.bodyDamping, 0.1f, 8.0f, "%.2f", ImGuiSliderFlags_Logarithmic);

        ImGui::Spacing();

        if (ImGui::Button("Defaults"))
        {
            settings = BoidSettings();
        }

        ImGui::SameLine();

        // The original JavaScript simulation's numbers, made for 20 boids with lots of room
        if (ImGui::Button("Original"))
        {
            settings = BoidSettings();
            settings.alignment = 0.125f;
            settings.separation = 8.0f;
            settings.restChance = 0.01f;
            settings.bumpCarry = 0.5f;
            settings.hunterFear = 1.0f;
        }
    }

    bool shapeFill(int shapeIndex, uint32_t &color) override
    {
        const int boidIndex = shapeIndex - firstBoidShape;

        if (boidIndex >= 0 && boidIndex < boids.size())
        {
            const Boid &boid = boids[boidIndex];

            if (boid.type == Type::Predator)
            {
                // Darker while resting after a catch
                color = boid.idle ? mixHex(boid.color, 0x5A2A3A, 0.35f) : boid.color;
            }
            else
            {
                color = mixHex(boid.color, fearColor, boid.fear * 0.8f);
            }

            // Caught ones flash white and fade back
            if (boid.flashMs > 0.0f)
            {
                const float t = boid.flashMs / flashDurationMs;
                color = mixHex(color, 0xFFFFFF, t * t);
            }

            return true;
        }

        for (int i = 0; i < obstacles.size(); i++)
        {
            if (obstacles[i].shapeIndex == shapeIndex)
            {
                color = rockColor;
                return true;
            }
        }

        return false;
    }

private:
    enum class Type
    {
        Prey,
        Predator
    };

    struct Boid
    {
        Type type;
        int pointStart;
        int pointEnd;
        int templateStart; // Its points' offsets from its center in the pose it was made in, in templateOffsets
        float radius;
        uint32_t color;
        Vector2 pos;      // Its body's center
        Vector2 velocity; // How far it wants to move each tick
        Vector2 heading;  // The way its eyes face, turning smoothly
        bool idle;        // Resting where it is
        int idleTicks;
        bool predatorCloseBy;
        bool dragged;
        float fear; // 0 to 1, following predatorCloseBy
        float flashMs;
    };

    // A rock's position and size, read from its shape every frame, as it can be dragged around
    struct Obstacle
    {
        Vector2 pos;
        float radius;
        int shapeIndex;
    };

    struct Burst
    {
        Vector2 pos;
        float ageMs;
    };

    // A small dot thrown out from an explosion
    struct Sparkle
    {
        Vector2 pos;
        Vector2 velocity;
        float sizePixels;
        float ageMs;
        float lifeMs;
        uint32_t color;
    };

    struct Ripple
    {
        Vector2 pos;
        float radius;
        uint32_t color;
        float ageMs;
        float durationMs;
    };

    static constexpr float arenaLeft = 4.0f;
    static constexpr float arenaTop = 4.0f;
    static constexpr float arenaRight = 1214.0f;
    static constexpr float arenaBottom = 755.0f;
    static constexpr float wallWidth = 100.0f;

    static const int numPrey = 150;
    static const int numPredators = 2;
    static const int numPreyColors = 5;
    static constexpr uint32_t preyColors[numPreyColors] = {0x3FA7D6, 0x2BB3A3, 0x5BC0EB, 0x4A86D9, 0x36C2B4};
    static constexpr uint32_t predatorColor = 0xE5484D;
    static constexpr uint32_t fearColor = 0xFFC94A;
    static constexpr uint32_t rockColor = 0x55606E;
    static constexpr float preyRadius = 14.0f;
    static constexpr float predatorRadius = 26.0f;
    static constexpr float preyMass = 1.0f;
    static constexpr float predatorMass = 2.5f;

    // The original's rules and numbers. It ran its rules every third frame at 60 frames per second, and its
    // boids had a radius of 10 pixels, so distances and speeds here are scaled up by sizeScale.
    static constexpr float tickMs = 50.0f;
    static constexpr float sizeScale = preyRadius / 10.0f;
    static constexpr float bounceBack = 5.0f * sizeScale;
    static constexpr float borderMargin = 60.0f; // How near the walls they turn back
    static constexpr float accelerationMax = 10.0f * sizeScale;
    static constexpr float obstacleAvoidanceDistanceBoost = 1400.0f * sizeScale;
    static constexpr float obstacleAvoidanceDistance = 4.0f; // In ticks of movement ahead
    static constexpr float obstacleAvoidanceFactor = 0.04f;
    static const int idleMaxTicks = 50;

    static constexpr float predatorChaseFactor = 0.01f;
    static constexpr float predatorAccelerationMax = 3.0f * sizeScale;
    static constexpr float predatorSeparationDistance = 90.0f * sizeScale;
    static constexpr float predatorSeparationFactor = 3.0f;
    static constexpr float predatorAvoidanceFactor = 0.03f;
    static constexpr float predatorCloseDistance = 80.0f * sizeScale;
    static const int predatorIdleMaxTicks = 80;
    static constexpr float predatorIdleProbability = 0.015f;

    // Not in the original
    static constexpr float steerTimeMs = 120.0f;  // About how long a body takes to reach the velocity it wants
    static constexpr float headingTimeMs = 120.0f;
    static constexpr float fearTimeMs = 250.0f;
    static constexpr float catchDistance = 4.0f;  // Gap between a hunter's body and its prey's that counts as caught
    static const int restTicksAfterCatch = 30;
    static constexpr float flashDurationMs = 600.0f;
    static constexpr float flungSteerTimeMs = 700.0f; // Steering's slower while flung faster than a boid swims

    // Clicks set off explosions like in the chaos scene
    static constexpr float explosionRadius = 220.0f;
    static constexpr float explosionImpulse = 3.0f; // Velocity change times mass at the center
    static constexpr float explosionMaxVelocityChange = 1.0f;
    static constexpr float dragExplosionSpacing = 40.0f; // How far the mouse moves between explosions while dragging
    static constexpr float burstDurationMs = 400.0f;
    static const int explosionSparkles = 30;

    // Small deterministic random numbers, so the flock moves the same way from a reset
    float random()
    {
        randomState = randomState * 1664525u + 1013904223u;
        return static_cast<float>(randomState >> 8) / 16777216.0f;
    }

    bool insideArena(Vector2 pos) const
    {
        return pos.x > arenaLeft && pos.x < arenaRight && pos.y > arenaTop && pos.y < arenaBottom;
    }

    // Somewhere in the arena clear of the rocks and the boids so far, for a body of the given radius
    Vector2 freePosition(float radius)
    {
        Vector2 pos;

        for (int attempt = 0; attempt < 200; attempt++)
        {
            pos = Vector2(arenaLeft + borderMargin + random() * (arenaRight - arenaLeft - borderMargin * 2.0f),
                          arenaTop + borderMargin + random() * (arenaBottom - arenaTop - borderMargin * 2.0f));
            bool free = true;

            for (int i = 0; i < obstacles.size() && free; i++)
            {
                free = (pos - obstacles[i].pos).length() > (obstacles[i].radius + radius) * 1.2f;
            }

            for (int i = 0; i < boids.size() && free; i++)
            {
                free = (pos - boids[i].pos).length() > (boids[i].radius + radius) * 1.2f;
            }

            if (free)
            {
                break;
            }
        }

        return pos;
    }

    Vector2 center(const PhysicsSpace &space, const Boid &boid) const
    {
        Vector2 sum;

        for (int p = boid.pointStart; p < boid.pointEnd; p++)
        {
            sum += space.points.pos[p];
        }

        return sum / static_cast<float>(boid.pointEnd - boid.pointStart);
    }

    // Where the rock's shape is now, and how big: its points' center and their average distance from it
    void updateObstacle(const PhysicsSpace &space, Obstacle &obstacle) const
    {
        if (obstacle.shapeIndex >= space.shapes.size())
        {
            return;
        }

        const Shape &shape = space.shapes[obstacle.shapeIndex];
        const float numPoints = static_cast<float>(shape.end - shape.start);
        Vector2 sum;

        for (int p = shape.start; p < shape.end; p++)
        {
            sum += space.points.pos[p];
        }

        const Vector2 pos = sum / numPoints;
        float distanceSum = 0.0f;

        for (int p = shape.start; p < shape.end; p++)
        {
            distanceSum += (space.points.pos[p] - pos).length();
        }

        if (!isnan(pos.x) && !isnan(pos.y))
        {
            obstacle.pos = pos;
            obstacle.radius = distanceSum / numPoints;
        }
    }

    Vector2 groupVelocity(const PhysicsSpace &space, const Boid &boid) const
    {
        Vector2 sum;

        for (int p = boid.pointStart; p < boid.pointEnd; p++)
        {
            sum += space.points.velocity[p];
        }

        return sum / static_cast<float>(boid.pointEnd - boid.pointStart);
    }

    // Puts the boid in its original pose around pos, standing still
    void place(PhysicsSpace &space, Boid &boid, Vector2 pos)
    {
        for (int p = boid.pointStart; p < boid.pointEnd; p++)
        {
            space.points.pos[p] = pos + templateOffsets[boid.templateStart + (p - boid.pointStart)];
            space.points.velocity[p] = Vector2();
        }

        boid.pos = pos;
    }

    // The nearest predator to the boid, or -1
    int nearestPredator(const Boid &boid) const
    {
        int nearest = -1;
        float nearestDistance = 0.0f;

        for (int i = 0; i < boids.size(); i++)
        {
            const float distance = (boids[i].pos - boid.pos).length();

            if (boids[i].type == Type::Predator && &boids[i] != &boid && (nearest == -1 || distance < nearestDistance))
            {
                nearest = i;
                nearestDistance = distance;
            }
        }

        return nearest;
    }

    // Boids try to keep a small distance away from other boids, and a bigger one from predators
    Vector2 separation(const Boid &boid) const
    {
        Vector2 result;

        for (int i = 0; i < boids.size(); i++)
        {
            const Boid &other = boids[i];

            if (&other == &boid)
            {
                continue;
            }

            const bool predator = other.type == Type::Predator;

            if ((other.pos - boid.pos).length() < (predator ? predatorSeparationDistance : settings.separationDistance))
            {
                result -= (other.pos - boid.pos) * (predator ? predatorSeparationFactor * settings.hunterFear : settings.separation);
            }
        }

        return result;
    }

    // Boids try to match velocity with near boids
    Vector2 alignment(const Boid &boid) const
    {
        Vector2 perceivedVelocity;
        int num = 0;

        for (int i = 0; i < boids.size(); i++)
        {
            if (&boids[i] != &boid && (boids[i].pos - boid.pos).length() < settings.neighbourDistance)
            {
                perceivedVelocity += boids[i].velocity;
                num++;
            }
        }

        if (num == 0)
        {
            return Vector2();
        }

        return (perceivedVelocity / static_cast<float>(num) - boid.velocity) * settings.alignment;
    }

    // Boids try to move towards the centre of mass of neighbouring boids, shifted away from the nearest predator
    Vector2 cohesion(const Boid &boid) const
    {
        Vector2 perceivedCentre;
        int num = 0;

        for (int i = 0; i < boids.size(); i++)
        {
            if (&boids[i] != &boid && boids[i].type != Type::Predator && (boids[i].pos - boid.pos).length() < settings.neighbourDistance)
            {
                perceivedCentre += boids[i].pos;
                num++;
            }
        }

        if (num == 0)
        {
            return Vector2();
        }

        perceivedCentre = perceivedCentre / static_cast<float>(num);
        const int predator = nearestPredator(boid);

        if (predator != -1)
        {
            perceivedCentre += (perceivedCentre - boids[predator].pos) * (predatorAvoidanceFactor * settings.hunterFear);
        }

        return (perceivedCentre - boid.pos) * settings.cohesion;
    }

    // Back towards the middle when near the walls
    Vector2 boundPosition(const Boid &boid) const
    {
        Vector2 result;

        if (boid.pos.x < arenaLeft + borderMargin)
        {
            result.x = bounceBack;
        }
        else if (boid.pos.x > arenaRight - borderMargin)
        {
            result.x = -bounceBack;
        }

        if (boid.pos.y < arenaTop + borderMargin)
        {
            result.y = bounceBack;
        }
        else if (boid.pos.y > arenaBottom - borderMargin)
        {
            result.y = -bounceBack;
        }

        return result;
    }

    // Towards the nearest boid of the other type
    Vector2 chaseClosestPrey(const Boid &boid) const
    {
        const Boid *closest = nullptr;
        float closestDistance = 0.0f;

        for (int i = 0; i < boids.size(); i++)
        {
            const float distance = (boids[i].pos - boid.pos).length();

            if (boids[i].type != boid.type && (closest == nullptr || distance < closestDistance))
            {
                closest = &boids[i];
                closestDistance = distance;
            }
        }

        return closest == nullptr ? Vector2() : (closest->pos - boid.pos) * predatorChaseFactor;
    }

    // Limits how far the acceleration turns the boid in one tick, scaling its speed change down to match,
    // and then how big it is
    void limitAcceleration(Vector2 &acceleration, const Boid &boid) const
    {
        const Vector2 newVelocity = boid.velocity + acceleration;
        const float currentAngle = atan2f(boid.velocity.y, boid.velocity.x);
        float angleDiff = atan2f(newVelocity.y, newVelocity.x) - currentAngle;

        // The shortest way round
        if (angleDiff > PI_F)
        {
            angleDiff -= 2.0f * PI_F;
        }
        else if (angleDiff < -PI_F)
        {
            angleDiff += 2.0f * PI_F;
        }

        if (angleDiff != 0.0f)
        {
            const float turnLimit = settings.turnLimitDegrees * PI_F / 180.0f;
            const float limitedDiff = clamp(angleDiff, -turnLimit, turnLimit);
            const float limitFactor = limitedDiff / angleDiff;
            const float magnitude = newVelocity.length() * limitFactor + boid.velocity.length() * (1.0f - limitFactor);
            acceleration = Vector2::fromAngle(currentAngle + limitedDiff) * magnitude - boid.velocity;
        }

        const float maxAcceleration = boid.type == Type::Predator ? predatorAccelerationMax : accelerationMax;
        const float magnitude = acceleration.length();

        if (magnitude > maxAcceleration)
        {
            acceleration = acceleration * (maxAcceleration / magnitude);
        }
    }

    void limitVelocity(Boid &boid) const
    {
        const float speed = boid.velocity.length();

        if (speed > settings.maxSpeed)
        {
            boid.velocity = boid.velocity * (settings.maxSpeed / speed);
        }
    }

    // Swerves to the side if the obstacle is in the boid's way, harder the nearer it is. The original always
    // swerved left; this swerves away from the obstacle's center.
    void limitObstacle(Boid &boid, const Obstacle &obstacle) const
    {
        const Vector2 ahead = boid.pos + boid.velocity * obstacleAvoidanceDistance;

        if (!lineIntersectsCircle(boid.pos, ahead, obstacle.pos, obstacle.radius + boid.radius))
        {
            return;
        }

        const float boost = max(1.0f, obstacleAvoidanceDistanceBoost / max(1.0f, (boid.pos - obstacle.pos).length()));
        const float side = boid.velocity.cross(obstacle.pos - boid.pos) > 0.0f ? -1.0f : 1.0f;
        boid.velocity += Vector2(-boid.velocity.y, boid.velocity.x) * (side * obstacleAvoidanceFactor * boost);
    }

    void tick(PhysicsSpace &space)
    {
        for (int i = 0; i < boids.size(); i++)
        {
            Boid &boid = boids[i];

            // Start from how its body is really moving, bumps and all
            if (!boid.idle && !boid.dragged)
            {
                boid.velocity = Vector2::lerp(boid.velocity, groupVelocity(space, boid) * tickMs, settings.bumpCarry);
            }
            else if (boid.dragged)
            {
                boid.velocity = groupVelocity(space, boid) * tickMs;
            }
        }

        for (int i = 0; i < boids.size(); i++)
        {
            Boid &boid = boids[i];
            const bool predator = boid.type == Type::Predator;
            boid.predatorCloseBy = false;

            if (!predator)
            {
                const int nearest = nearestPredator(boid);

                if (nearest != -1 && (boids[nearest].pos - boid.pos).length() < predatorCloseDistance)
                {
                    boid.predatorCloseBy = true;
                }

                if (boid.idle && boid.predatorCloseBy)
                {
                    boid.idle = false;
                }

                if (!boid.idle)
                {
                    tickMoving(boid, separation(boid) + alignment(boid) + cohesion(boid) + boundPosition(boid));
                }
            }
            else if (!boid.idle)
            {
                tickMoving(boid, chaseClosestPrey(boid) + boundPosition(boid));
            }

            if (boid.idle)
            {
                boid.idleTicks--;
                boid.idle = boid.idleTicks > 0;
            }
        }

        catchPrey();
    }

    void tickMoving(Boid &boid, Vector2 acceleration)
    {
        const bool predator = boid.type == Type::Predator;
        limitAcceleration(acceleration, boid);
        boid.velocity += acceleration;
        limitVelocity(boid);

        for (int i = 0; i < obstacles.size(); i++)
        {
            limitObstacle(boid, obstacles[i]);
        }

        // Now and then one stops for a while, but never with a predator close
        boid.idle = random() < (predator ? predatorIdleProbability : settings.restChance) && !(boid.predatorCloseBy && !predator) && !boid.dragged;

        if (boid.idle)
        {
            boid.idleTicks = static_cast<int>(random() * static_cast<float>(predator ? predatorIdleMaxTicks : idleMaxTicks));
        }
    }

    // A hunter touching a boid has caught it: the boid flashes and bolts, and the hunter rests
    void catchPrey()
    {
        for (int h = 0; h < boids.size(); h++)
        {
            Boid &hunter = boids[h];

            if (hunter.type != Type::Predator || hunter.idle || hunter.dragged)
            {
                continue;
            }

            for (int i = 0; i < boids.size(); i++)
            {
                Boid &prey = boids[i];
                const Vector2 offset = prey.pos - hunter.pos;

                if (prey.type == Type::Predator || offset.length() > hunter.radius + prey.radius + catchDistance)
                {
                    continue;
                }

                prey.flashMs = flashDurationMs;
                prey.idle = false;
                prey.velocity = offset.normalized() * settings.maxSpeed;
                hunter.idle = true;
                hunter.idleTicks = restTicksAfterCatch;
                ripples.push({hunter.pos + offset.normalized() * hunter.radius, prey.radius * 1.5f, 0xFFFFFF, 0.0f, 500.0f});
                break;
            }
        }
    }

    // Flings everything near pos away, and wakes any resting boids it reaches
    void explode(Game &game, Vector2 pos)
    {
        lastExplosionPos = pos;
        pushAwayFrom(game.physicsSpace(), pos, explosionRadius, explosionImpulse, explosionMaxVelocityChange);
        bursts.push({pos, 0.0f});

        for (int i = 0; i < boids.size(); i++)
        {
            if ((boids[i].pos - pos).length() < explosionRadius)
            {
                boids[i].idle = false;
            }
        }

        const uint32_t sparkleColors[3] = {0xFFFFFF, 0xFFD966, 0xFF8A3D};

        for (int k = 0; k < explosionSparkles; k++)
        {
            const Vector2 direction = Vector2::fromAngle(random() * 2.0f * PI_F);
            sparkles.push({pos + direction * (random() * 12.0f), direction * (0.2f + random() * 0.35f), 2.5f + random() * 3.0f, 0.0f,
                           400.0f + random() * 400.0f, sparkleColors[k % 3]});
        }
    }

    // Two eyes looking the way the boid is heading, shut while it rests. Hunters frown.
    void drawEyes(SceneCanvas &canvas, const Boid &boid) const
    {
        const Vector2 forward = boid.heading;
        const Vector2 side(-forward.y, forward.x);
        const float r = boid.radius;
        const float eyeRadius = r * (boid.type == Type::Predator ? 0.22f : 0.26f);
        const CanvasColor white = {1.0f, 1.0f, 1.0f, 1.0f};
        const CanvasColor dark = CanvasColor::hex(0x1B1F2A);

        for (int k = -1; k <= 1; k += 2)
        {
            const Vector2 eye = boid.pos + forward * (r * 0.4f) + side * (static_cast<float>(k) * r * 0.36f);

            if (boid.idle)
            {
                canvas.line(eye - side * eyeRadius, eye + side * eyeRadius, 2.0f, dark);
                continue;
            }

            // Wide eyed when frightened
            canvas.disc(eye, eyeRadius * (1.0f + 0.25f * boid.fear), white);
            canvas.disc(eye + forward * (eyeRadius * 0.4f), eyeRadius * (0.55f - 0.15f * boid.fear), dark);

            if (boid.type == Type::Predator)
            {
                // Slanting down towards the middle
                const Vector2 browCenter = eye + forward * (eyeRadius * 1.3f);
                canvas.line(browCenter + side * (static_cast<float>(k) * eyeRadius * 1.2f) + forward * (eyeRadius * 0.5f),
                            browCenter - side * (static_cast<float>(k) * eyeRadius * 1.2f) - forward * (eyeRadius * 0.1f), 3.0f, dark);
            }
        }
    }

    // The isometric drawing

    // About a boid's height, so those along the near walls still show over them
    static constexpr float wallHeight = 30.0f;
    static constexpr float floorTileSize = 60.0f;
    static constexpr uint32_t floorColors[2] = {0xEDE6D8, 0xE5DCCB};
    static constexpr uint32_t wallColor = 0x7C8A9C;
    static constexpr uint32_t isoOutlineColor = 0x22303F;

    // Something standing on the ground, drawn in order from the back
    struct Standing
    {
        float depth; // How near the viewer
        int boid;    // Or -1 for a rock
        int rock;
    };

    void updateProjection()
    {
        projection.turn = isoView.turn;
        projection.elevation = isoView.elevation;
        projection.pickHeight = preyRadius;
        projection.tallest = wallHeight;
    }

    // Outlines and highlights keep their size zooming in, and shrink zooming out, like the usual drawing
    static float detailScale(const SceneCanvas &canvas)
    {
        return clamp(1.0f / canvas.pixel(), 0.4f, 1.0f);
    }

    void drawIsometric(Game &game, SceneCanvas &canvas)
    {
        const PhysicsSpace &space = game.physicsSpace();
        const Vector2 towardsViewer = projection.towardsViewer();
        canvas.setGround(nullptr);
        drawFloor(canvas);

        // Shadows, flat on the floor under everything standing on it
        for (int i = 0; i < obstacles.size(); i++)
        {
            drawGroundShadow(canvas, obstacles[i].pos, obstacles[i].radius * 1.05f);
        }

        for (int i = 0; i < boids.size(); i++)
        {
            drawGroundShadow(canvas, boids[i].pos, boids[i].radius * 1.1f);
        }

        // Marks on the floor: explosions and catches
        canvas.setGround(&projection);

        for (int i = 0; i < ripples.size(); i++)
        {
            const Ripple &ripple = ripples[i];
            drawRipple(canvas, ripple.pos, ripple.radius, ripple.ageMs / ripple.durationMs, CanvasColor::hex(ripple.color, 0.9f));
        }

        for (int i = 0; i < bursts.size(); i++)
        {
            drawBurst(canvas, bursts[i].pos, explosionRadius * 0.75f, bursts[i].ageMs / burstDurationMs, CanvasColor::hex(0xFFB040));
        }

        canvas.setGround(nullptr);

        // The walls on the far side stand behind everything, those on the near side in front
        drawWalls(canvas, space, false);

        standing.clear();

        for (int i = 0; i < obstacles.size(); i++)
        {
            standing.push({obstacles[i].pos.dot(towardsViewer), -1, i});
        }

        for (int i = 0; i < boids.size(); i++)
        {
            standing.push({boids[i].pos.dot(towardsViewer), i, -1});
        }

        // Insertion sort: from one frame to the next the order hardly changes
        for (int i = 1; i < standing.size(); i++)
        {
            const Standing item = standing[i];
            int j = i - 1;

            for (; j >= 0 && standing[j].depth > item.depth; j--)
            {
                standing[j + 1] = standing[j];
            }

            standing[j + 1] = item;
        }

        for (int i = 0; i < standing.size(); i++)
        {
            if (standing[i].boid != -1)
            {
                drawStandingBoid(game, canvas, space, standing[i].boid);
            }
            else
            {
                drawRock(game, canvas, standing[i].rock);
            }
        }

        drawWalls(canvas, space, true);

        // Sparkles fly up out of an explosion and fall back as they fade
        for (int i = 0; i < sparkles.size(); i++)
        {
            const Sparkle &sparkle = sparkles[i];
            const float t = sparkle.ageMs / sparkle.lifeMs;
            CanvasColor color = CanvasColor::hex(sparkle.color);
            color.a = 1.0f - t;
            canvas.disc(projection.toView(sparkle.pos, 40.0f * t * (1.0f - t) * 4.0f), sparkle.sizePixels * (1.0f - 0.5f * t) * canvas.pixel(), color);
        }
    }

    // A chequered floor over the arena
    void drawFloor(SceneCanvas &canvas)
    {
        const int columns = static_cast<int>(ceilf((arenaRight - arenaLeft) / floorTileSize));
        const int rows = static_cast<int>(ceilf((arenaBottom - arenaTop) / floorTileSize));

        for (int row = 0; row < rows; row++)
        {
            for (int column = 0; column < columns; column++)
            {
                const float x0 = arenaLeft + static_cast<float>(column) * floorTileSize;
                const float y0 = arenaTop + static_cast<float>(row) * floorTileSize;
                const float x1 = min(x0 + floorTileSize, arenaRight);
                const float y1 = min(y0 + floorTileSize, arenaBottom);
                viewPoints.clear();
                viewPoints.push(projection.toView(Vector2(x0, y0)));
                viewPoints.push(projection.toView(Vector2(x1, y0)));
                viewPoints.push(projection.toView(Vector2(x1, y1)));
                viewPoints.push(projection.toView(Vector2(x0, y1)));
                canvas.polygon(viewPoints, CanvasColor::hex(floorColors[(row + column) % 2]));
            }
        }
    }

    void drawGroundShadow(SceneCanvas &canvas, Vector2 pos, float radius)
    {
        canvas.setGround(&projection);
        canvas.disc(pos, radius, {0.0f, 0.0f, 0.0f, 0.13f});
        canvas.setGround(nullptr);
    }

    Vector2 shapeCenter(const PhysicsSpace &space, int shapeIndex) const
    {
        const Shape &shape = space.shapes[shapeIndex];
        Vector2 sum;

        for (int p = shape.start; p < shape.end; p++)
        {
            sum += space.points.pos[p];
        }

        return sum / static_cast<float>(shape.end - shape.start);
    }

    // The walls on the near side of the arena, or the far side, as blocks standing on their outlines. All their sides
    // go before any of their tops: the walls are the same height, so a top only covers the sides of walls behind
    // it. Drawn one wall at a time, a wall's sides were drawn over the top of the one before where they met.
    void drawWalls(SceneCanvas &canvas, const PhysicsSpace &space, bool nearSide)
    {
        const Vector2 arenaMiddle((arenaLeft + arenaRight) * 0.5f, (arenaTop + arenaBottom) * 0.5f);
        const Vector2 towardsViewer = projection.towardsViewer();
        const CanvasColor outline = CanvasColor::hex(isoOutlineColor);
        const float outlineWidth = 1.5f * detailScale(canvas);

        for (int pass = 0; pass < 2; pass++)
        {
            for (int s = firstWallShape; s < firstWallShape + numWalls; s++)
            {
                if (((shapeCenter(space, s) - arenaMiddle).dot(towardsViewer) >= 0.0f) != nearSide)
                {
                    continue;
                }

                if (pass == 0)
                {
                    drawWallSides(canvas, space, s, outlineWidth, outline);
                    continue;
                }

                const Shape &shape = space.shapes[s];
                viewPoints.clear();

                for (int p = shape.start; p < shape.end; p++)
                {
                    viewPoints.push(projection.toView(space.points.pos[p], wallHeight));
                }

                canvas.polygon(viewPoints, CanvasColor::hex(mixHex(wallColor, 0xFFFFFF, 0.25f)));
                canvas.outline(viewPoints, outlineWidth, outline);
            }
        }
    }

    // The sides of a wall that face the viewer, except those up against another wall, as where two walls meet in
    // a corner: they're inside the walls, and drawn they showed through the other wall's top
    void drawWallSides(SceneCanvas &canvas, const PhysicsSpace &space, int shapeIndex, float outlineWidth, CanvasColor outline)
    {
        const Shape &shape = space.shapes[shapeIndex];
        const int numPoints = shape.end - shape.start;

        // Which way round the outline goes, for the sides' outward directions
        float area = 0.0f;

        for (int k = 0; k < numPoints; k++)
        {
            area += space.points.pos[shape.start + k].cross(space.points.pos[shape.start + (k + 1) % numPoints]);
        }

        for (int k = 0; k < numPoints; k++)
        {
            const Vector2 a = space.points.pos[shape.start + k];
            const Vector2 b = space.points.pos[shape.start + (k + 1) % numPoints];
            const Vector2 edge = b - a;
            const Vector2 outward = (area > 0.0f ? Vector2(edge.y, -edge.x) : Vector2(-edge.y, edge.x)).normalized();

            if (outward.dot(projection.towardsViewer()) <= 0.0f || insideAnotherWall(space, shapeIndex, (a + b) * 0.5f + outward))
            {
                continue;
            }

            // Lit from the left of the view, so sides facing that way are lighter
            const float lit = 0.5f - 0.5f * projection.turned(outward).x;
            viewPoints.clear();
            viewPoints.push(projection.toView(a));
            viewPoints.push(projection.toView(b));
            viewPoints.push(projection.toView(b, wallHeight));
            viewPoints.push(projection.toView(a, wallHeight));
            canvas.polygon(viewPoints, CanvasColor::hex(mixHex(wallColor, 0x1B2430, 0.45f - 0.3f * lit)));
            canvas.outline(viewPoints, outlineWidth, outline);
        }
    }

    bool insideAnotherWall(const PhysicsSpace &space, int shapeIndex, Vector2 pos) const
    {
        for (int s = firstWallShape; s < firstWallShape + numWalls; s++)
        {
            const Shape &shape = space.shapes[s];
            bool inside = false;

            for (int i = shape.start, j = shape.end - 1; i < shape.end && s != shapeIndex; j = i++)
            {
                const Vector2 p = space.points.pos[i];
                const Vector2 q = space.points.pos[j];

                if ((p.y > pos.y) != (q.y > pos.y) && pos.x < (q.x - p.x) * (pos.y - p.y) / (q.y - p.y) + p.x)
                {
                    inside = !inside;
                }
            }

            if (inside)
            {
                return true;
            }
        }

        return false;
    }

    // A rock as a dome: the top half of the circle a sphere is seen as, closed by the near half of its base
    void drawRock(Game &game, SceneCanvas &canvas, int rockIndex)
    {
        const Obstacle &rock = obstacles[rockIndex];
        const Vector2 base = projection.toView(rock.pos);
        const float r = rock.radius;
        const int segments = 24;
        viewPoints.clear();

        for (int k = 0; k <= segments; k++)
        {
            const float angle = PI_F + PI_F * static_cast<float>(k) / static_cast<float>(segments);
            viewPoints.push(base + Vector2(cosf(angle), sinf(angle)) * r);
        }

        for (int k = 1; k < segments; k++)
        {
            const float angle = PI_F * static_cast<float>(k) / static_cast<float>(segments);
            viewPoints.push(base + Vector2(cosf(angle), sinf(angle) * sinf(projection.elevation)) * r);
        }

        uint32_t color = rockColor;
        shapeFill(rock.shapeIndex, color);
        drawLitBody(canvas, base + Vector2(-0.3f, -0.45f) * r, color, game.hoveredShapeIndex() == rock.shapeIndex);
    }

    // A boid as a ball resting on the floor. Its outline is its body's, squashes and all, stood up to face the viewer.
    void drawStandingBoid(Game &game, SceneCanvas &canvas, const PhysicsSpace &space, int boidIndex)
    {
        const Boid &boid = boids[boidIndex];
        const float r = boid.radius;
        const Vector2 middle = projection.toView(boid.pos, r);
        const int numPoints = boid.pointEnd - boid.pointStart;
        const int subdivisions = clamp(static_cast<int>(ceilf(2.0f * PI_F * r / static_cast<float>(numPoints) / (6.0f * canvas.pixel()))), 1, 4);
        viewPoints.clear();

        for (int k = 0; k < numPoints; k++)
        {
            const Vector2 p0 = upright(space, boid, middle, k - 1 + numPoints);
            const Vector2 p1 = upright(space, boid, middle, k);
            const Vector2 p2 = upright(space, boid, middle, k + 1);
            const Vector2 p3 = upright(space, boid, middle, k + 2);

            for (int step = 0; step < subdivisions; step++)
            {
                viewPoints.push(catmullRom(p0, p1, p2, p3, static_cast<float>(step) / static_cast<float>(subdivisions)));
            }
        }

        const int shapeIndex = firstBoidShape + boidIndex;
        uint32_t color = boid.color;
        shapeFill(shapeIndex, color);
        drawLitBody(canvas, middle + Vector2(-0.3f, -0.35f) * r, color, game.hoveredShapeIndex() == shapeIndex);

        if (r / canvas.pixel() >= 6.0f)
        {
            canvas.disc(middle + Vector2(-0.38f, -0.42f) * r, 0.16f * r, {1.0f, 1.0f, 1.0f, 0.55f});
        }

        drawStandingEyes(canvas, boid);
    }

    // Point k of the boid's body, around middle in the view, turned with the ground but not tilted
    Vector2 upright(const PhysicsSpace &space, const Boid &boid, Vector2 middle, int k) const
    {
        const int numPoints = boid.pointEnd - boid.pointStart;
        return middle + projection.turned(space.points.pos[boid.pointStart + k % numPoints] - boid.pos);
    }

    // Fills viewPoints, shaded from light, and outlines it. Lighter when the mouse is over it, like the usual drawing.
    void drawLitBody(SceneCanvas &canvas, Vector2 light, uint32_t color, bool hovered)
    {
        uint32_t outline = isoOutlineColor;

        if (hovered)
        {
            outline = mixHex(outline, color, 0.4f);
            color = mixHex(color, 0xFFFFFF, 0.2f);
        }

        canvas.shadedPolygon(viewPoints, light, CanvasColor::hex(mixHex(color, 0xFFFFFF, 0.35f)), CanvasColor::hex(mixHex(color, 0x000000, 0.10f)));
        canvas.outline(viewPoints, 2.5f * detailScale(canvas), CanvasColor::hex(outline));
    }

    // The eyes as on the flat version, but on the ball's surface, looking the way it heads. Those on the far side
    // of the ball can't be seen, and ones turning away get smaller.
    void drawStandingEyes(SceneCanvas &canvas, const Boid &boid)
    {
        const Vector2 forward = boid.heading;
        const Vector2 side(-forward.y, forward.x);
        const float r = boid.radius;
        const float eyeRadius = r * (boid.type == Type::Predator ? 0.22f : 0.26f);
        const CanvasColor white = {1.0f, 1.0f, 1.0f, 1.0f};
        const CanvasColor dark = CanvasColor::hex(0x1B1F2A);
        const Vector2 towardsViewer = projection.towardsViewer() * cosf(projection.elevation);
        const float viewerUp = sinf(projection.elevation);
        const float eyeUp = r * 0.3f;

        for (int k = -1; k <= 1; k += 2)
        {
            const Vector2 out = forward * (r * 0.78f) + side * (static_cast<float>(k) * r * 0.4f);
            const float facing = (out.dot(towardsViewer) + eyeUp * viewerUp) / sqrtf(out.lengthSquared() + eyeUp * eyeUp);

            if (facing < 0.15f)
            {
                continue;
            }

            const float size = sqrtf(facing);
            const Vector2 eye = projection.toView(boid.pos + out, r + eyeUp);

            if (boid.idle)
            {
                canvas.line(projection.toView(boid.pos + out - side * eyeRadius, r + eyeUp),
                            projection.toView(boid.pos + out + side * eyeRadius, r + eyeUp), 2.0f, dark);
                continue;
            }

            // Wide eyed when frightened
            canvas.disc(eye, eyeRadius * size * (1.0f + 0.25f * boid.fear), white);
            canvas.disc(projection.toView(boid.pos + out + forward * (eyeRadius * 0.4f), r + eyeUp), eyeRadius * size * (0.55f - 0.15f * boid.fear), dark);

            if (boid.type == Type::Predator)
            {
                // Slanting down towards the middle
                const Vector2 brow = out + forward * (eyeRadius * 0.3f);
                canvas.line(projection.toView(boid.pos + brow + side * (static_cast<float>(k) * eyeRadius * 1.2f), r + eyeUp + eyeRadius * 1.5f),
                            projection.toView(boid.pos + brow - side * (static_cast<float>(k) * eyeRadius * 1.2f), r + eyeUp + eyeRadius * 0.9f), 3.0f, dark);
            }
        }
    }

    static const int numWalls = 4;
    const bool isometric;
    ViewProjection projection;
    int firstWallShape = 0;
    Array<Standing> standing;
    Array<Vector2> viewPoints;

    // From the sliders, every frame, so the Defaults and Original buttons take effect too
    void applyBodySettings(PhysicsSpace &space)
    {
        for (int i = 0; i < boids.size(); i++)
        {
            Shape &shape = space.shapes[firstBoidShape + i];
            shape.stiffness = baseStiffness * settings.bodyStiffness;
            shape.damping = baseDamping * settings.bodyDamping;
        }
    }

    int firstBoidShape = 0;
    float baseStiffness = 1.0f; // A boid's body's stiffness and damping as made, before the sliders
    float baseDamping = 1.0f;
    float timeToNextTickMs = 0.0f;
    uint32_t randomState = 4242u;
    Vector2 lastExplosionPos;
    Array<Boid> boids;
    Array<Obstacle> obstacles;
    Array<Vector2> templateOffsets;
    Array<Burst> bursts;
    Array<Sparkle> sparkles;
    Array<Ripple> ripples;
};

void initBoidsScene(Game *game)
{
    game->setScript(new BoidsScript(*game, false));
}

void initIsometricBoidsScene(Game *game)
{
    game->setScript(new BoidsScript(*game, true));
}
