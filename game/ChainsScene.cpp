#include "ChainsScene.h"
#include <math.h>
#include "Game.h"
#include "SceneScript.h"
#include "Shapes.h"
#include "../physics/PhysicsSpace.h"
#include "../physics/PhysicsSpaceStorage.h"
#include "../containers/Array.h"
#include "../utils/MinMax.h"
#include "../imgui/imgui.h"

// Where to launch the ball and how fast, and how stiff the chains are, from the scene settings. Kept when the
// scene is reset, so the same launch can be tried again.
struct ChainsSettings
{
    float angle = -1.05f; // Radians, from the +x axis, with y down: up and to the right
    float speed = 3.0f;   // World units per millisecond
    // On the chain segments' stiffness from the scene file. As made, a fast ball stretched segments to three
    // times their length or more and went through the gap; torn chains tangled for seconds or ended up outside the
    // box. Four times as stiff, out of 64 launches at top speed, chains escaped 2 times instead of 12, tangled
    // past 2 seconds 2 times instead of 10, and none tore, against 8.
    float chainStiffness = 4.0f;
};

static ChainsSettings settings;

// The chains scene's own controls: a button that sets the ball off in a chosen direction, as fast as wanted,
// for trying out how the chains and the collisions hold up to hard, fast hits
class ChainsScript : public SceneScript
{
public:
    explicit ChainsScript(Game &game)
    {
        // The ball: the one moving shape that is a plain circle rather than part of a chain
        const PhysicsSpace &space = game.physicsSpace();

        for (int s = 0; s < space.shapes.size(); s++)
        {
            const Shape &shape = space.shapes[s];

            if (!shape.isStatic && !shape.hasIndices() && shape.end - shape.start >= 8 && ballShape == -1)
            {
                ballShape = s;
            }

            // The chains: segments of bodies
            if (!shape.isStatic && shape.hasIndices())
            {
                chainShapes.push(s);
                baseStiffness.push(shape.stiffness);
            }
        }

        applyChainStiffness(game.physicsSpace());
    }

    void update(Game &game, float elapsedMs) override
    {
        aimShownMs = max(0.0f, aimShownMs - elapsedMs);
    }

    bool hasControls() const override
    {
        return ballShape != -1;
    }

    void drawControls(Game &game) override
    {
        ImGui::SeparatorText("Ball");

        bool changed = ImGui::SliderAngle("Direction", &settings.angle, -180.0f, 180.0f);
        changed |= ImGui::SliderFloat("Speed", &settings.speed, 0.1f, 15.0f, "%.2f", ImGuiSliderFlags_Logarithmic);

        if (changed)
        {
            aimShownMs = aimShowDurationMs;
        }

        ImGui::Spacing();

        if (ImGui::Button("Launch ball"))
        {
            launch(game);
        }

        ImGui::SeparatorText("Chains");

        if (ImGui::SliderFloat("Stiffness", &settings.chainStiffness, 1.0f, 8.0f, "%.1f"))
        {
            applyChainStiffness(game.physicsSpace());
        }
    }

    void drawOverlay(Game &game, SceneCanvas &canvas) override
    {
        if (aimShownMs <= 0.0f || ballShape == -1 || ballShape >= game.physicsSpace().shapes.size())
        {
            return;
        }

        // An arrow from the ball the way it will go, longer the faster, fading out
        const Vector2 from = ballCenter(game.physicsSpace());
        const Vector2 direction(cosf(settings.angle), sinf(settings.angle));
        const Vector2 to = from + direction * (60.0f + settings.speed * 30.0f);
        const Vector2 side(-direction.y, direction.x);
        const float headLength = 18.0f * canvas.pixel();
        CanvasColor color = CanvasColor::hex(0xE85D4F, min(1.0f, aimShownMs / 400.0f));

        canvas.line(from, to, 3.0f, color);
        canvas.line(to, to - direction * headLength + side * headLength * 0.6f, 3.0f, color);
        canvas.line(to, to - direction * headLength - side * headLength * 0.6f, 3.0f, color);
    }

private:
    static constexpr float aimShowDurationMs = 1500.0f;

    Vector2 ballCenter(const PhysicsSpace &space) const
    {
        const Shape &shape = space.shapes[ballShape];
        Vector2 sum;

        for (int i = shape.start; i < shape.end; i++)
        {
            sum += space.points.pos[i];
        }

        return sum / static_cast<float>(shape.end - shape.start);
    }

    void applyChainStiffness(PhysicsSpace &space)
    {
        for (int i = 0; i < chainShapes.size(); i++)
        {
            space.shapes[chainShapes[i]].stiffness = baseStiffness[i] * settings.chainStiffness;
        }
    }

    // Every point of the ball gets the same velocity, replacing whatever it had, so it flies off whole
    void launch(Game &game)
    {
        PhysicsSpace &space = game.physicsSpace();

        if (ballShape >= space.shapes.size())
        {
            return;
        }

        const Shape &shape = space.shapes[ballShape];
        const Vector2 velocity = Vector2(cosf(settings.angle), sinf(settings.angle)) * settings.speed;

        for (int i = shape.start; i < shape.end; i++)
        {
            space.points.velocity[i] = velocity;
        }

        aimShownMs = aimShowDurationMs;
    }

    int ballShape = -1;
    float aimShownMs = 0.0f;
    Array<int> chainShapes;
    Array<float> baseStiffness; // As loaded, before the setting
};

// The walls in the scene file stop well below the tops of the chains, and there is nothing above, so the
// launched ball could leave. The walls are raised past the chains and a bar laid across their tops.
//
// The walls also reach right through the floor and the bar, so the corners are solid. Where the walls only just
// touched them, a point pressed into the floor at a corner was pushed up into the wall, and back down out of the
// wall's bottom edge, a hair away, into the floor again, and slid out under the wall.
static void closeInChains(PhysicsSpace &space)
{
    const float margin = 80.0f;
    const float barThickness = 60.0f;
    float top = 1e9f;

    for (int s = 0; s < space.shapes.size(); s++)
    {
        const Shape &shape = space.shapes[s];

        for (int i = shape.start; i < shape.end && !shape.isStatic; i++)
        {
            top = min(top, space.points.pos[i].y);
        }
    }

    top -= margin;
    float left = 1e9f;
    float right = -1e9f;
    float floorBottom = -1e9f;

    // The floor: the static quad wider than it is tall
    for (int s = 0; s < space.shapes.size(); s++)
    {
        const Shape &shape = space.shapes[s];

        if (!shape.isStatic || shape.end - shape.start != 4)
        {
            continue;
        }

        Vector2 low(1e9f, 1e9f);
        Vector2 high(-1e9f, -1e9f);

        for (int i = shape.start; i < shape.end; i++)
        {
            low = Vector2(min(low.x, space.points.pos[i].x), min(low.y, space.points.pos[i].y));
            high = Vector2(max(high.x, space.points.pos[i].x), max(high.y, space.points.pos[i].y));
        }

        if (high.x - low.x > high.y - low.y)
        {
            floorBottom = max(floorBottom, high.y);
        }
    }

    // The walls: static quads taller than they are wide. Their top corners go up to the top of the bar, and their
    // bottom corners down to the bottom of the floor.
    for (int s = 0; s < space.shapes.size(); s++)
    {
        const Shape &shape = space.shapes[s];

        if (!shape.isStatic || shape.end - shape.start != 4)
        {
            continue;
        }

        Vector2 low(1e9f, 1e9f);
        Vector2 high(-1e9f, -1e9f);

        for (int i = shape.start; i < shape.end; i++)
        {
            low = Vector2(min(low.x, space.points.pos[i].x), min(low.y, space.points.pos[i].y));
            high = Vector2(max(high.x, space.points.pos[i].x), max(high.y, space.points.pos[i].y));
        }

        if (high.y - low.y <= high.x - low.x)
        {
            continue;
        }

        const float middle = (low.y + high.y) * 0.5f;

        for (int i = shape.start; i < shape.end; i++)
        {
            if (space.points.pos[i].y < middle)
            {
                space.points.pos[i].y = top - barThickness;
            }
            else if (floorBottom > middle)
            {
                space.points.pos[i].y = floorBottom;
            }
        }

        Shapes::recalculateOriginalPos(space, shape);
        left = min(left, low.x);
        right = max(right, high.x);
    }

    if (left < right)
    {
        Shapes::createStaticQuad(space, left, top - barThickness, right - left, barThickness, 1.0f);
    }
}

void initChainsScene(Game *game)
{
    PhysicsSpace &space = game->physicsSpace();
    PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_31.txt");
    closeInChains(space);
    game->scale() = 1.00f;
    game->offset() = Vector2(-109.00f, 69.00f);
    game->updateBoundingBoxes();
    game->setScript(new ChainsScript(*game));
}
