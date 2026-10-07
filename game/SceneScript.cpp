#include "SceneScript.h"
#include "../physics/PhysicsSpace.h"
#include "../utils/MinMax.h"
#include <math.h>

uint32_t mixHex(uint32_t a, uint32_t b, float t)
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

void pushAwayFrom(PhysicsSpace &space, Vector2 center, float radius, float impulse, float maxVelocityChange)
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

void drawBurst(SceneCanvas &canvas, Vector2 pos, float radius, float t, CanvasColor flash)
{
    const float fade = 1.0f - clamp(t, 0.0f, 1.0f);
    CanvasColor outer = flash;
    outer.a = 0.55f * fade * fade;

    canvas.disc(pos, radius * (0.35f + 0.4f * t), outer);
    canvas.disc(pos, radius * (0.2f + 0.25f * t), {1.0f, 0.95f, 0.75f, 0.8f * fade * fade});
    canvas.ring(pos, radius * (0.3f + 0.7f * t), 6.0f * fade + 1.0f, {1.0f, 1.0f, 1.0f, 0.9f * fade});
}

// The ground is turned first, then its far side tipped away from the viewer, which shortens it up and down the
// view by the sine of the elevation. Heights point up the view, shortened by its cosine.
Vector2 ViewProjection::toView(Vector2 ground, float height) const
{
    const Vector2 r = ground.rotate(turn);
    return Vector2(r.x, r.y * sinf(elevation) - height * cosf(elevation));
}

Vector2 ViewProjection::toGround(Vector2 view, float height) const
{
    return Vector2(view.x, (view.y + height * cosf(elevation)) / sinf(elevation)).rotate(-turn);
}

Vector2 ViewProjection::toGroundMove(Vector2 viewMove) const
{
    return Vector2(viewMove.x, viewMove.y / sinf(elevation)).rotate(-turn);
}

Vector2 ViewProjection::turned(Vector2 groundOffset) const
{
    return groundOffset.rotate(turn);
}

Vector2 ViewProjection::towardsViewer() const
{
    return Vector2(0.0f, 1.0f).rotate(-turn);
}
