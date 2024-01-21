#include "Shapes.h"
#include "../containers/Array.h"
#include "./Physics.h"

namespace Shapes {
    Shape createQuad(Array<PointMass> &points, Array<Spring>& springs, float x, float y, float width, float height, float drag) {
        points.append({
                {.pos = Vector2(x, y), .velocity = Vector2::zero(), .mass = drag, .pin = 0.0f },
                {.pos = Vector2(x + width, y), .velocity = Vector2::zero(), .mass = drag, .pin = 0.0f },
                {.pos = Vector2(x + width, y + height), .velocity = Vector2::zero(), .mass = drag, .pin = 0.0f},
                {.pos = Vector2(x, y + height), .velocity = Vector2::zero(), .mass = drag, .pin = 0.0f}
            });

        const Span span = { .start = points.size() - 4, .end = points.size() };
        const PointMass &p0 = points[span.start];
        const PointMass &p1 = points[span.start + 1];
        const PointMass &p2 = points[span.start + 2];
        const PointMass &p3 = points[span.start + 3];

        float stiffness = 0.3f;
        float damping = 28.9f;

        springs.push({span.start, span.start + 1, p0.pos.distance(p1.pos), stiffness, damping});
        springs.push({span.start + 1, span.start + 2, p1.pos.distance(p2.pos), stiffness, damping});
        springs.push({span.start + 2, span.start + 3, p2.pos.distance(p3.pos), stiffness, damping});
        springs.push({span.start + 3, span.start, p3.pos.distance(p0.pos), stiffness, damping});
        springs.push({span.start, span.start + 2, p0.pos.distance(p2.pos), stiffness, damping});
        springs.push({span.start + 1, span.start + 3, p1.pos.distance(p3.pos), stiffness, damping});

        return Shape{ .start = span.start, .end = span.end, .isStatic = false };
    }
  Shape createParallelogram(Array<PointMass> &points, float x, float y, float width, float height, float sideOffset, float drag) {
        points.append({
                { .pos = Vector2(x, y), .velocity = Vector2::zero(), .mass = drag, .pin = 0.0f },
                { .pos = Vector2(x + width, y), .velocity = Vector2::zero(), .mass = drag, .pin = 0.0f},
                { .pos = Vector2(x + width + sideOffset, y + height), .velocity = Vector2::zero(), .mass = drag, .pin = 0.0f},
                { .pos = Vector2(x + sideOffset, y + height), .velocity = Vector2::zero(), .mass = drag, .pin = 0.0f}
            });
            
        return Shape{ .start = points.size() - 4, .end = points.size(), .isStatic = false };
    }

    Shape createTriangle(Array<PointMass> &points, float x0, float y0, float x1, float y1, float x2, float y2, float drag) {
        points.append({{ .pos = Vector2(x0, y0), .velocity = Vector2(0.0f, 0.0f), .mass = drag, .pin = 0.0f},
                             { .pos = Vector2(x1, y1), .velocity = Vector2(0.0f, 0.0f), .mass = drag, .pin = 0.0f},
                             { .pos = Vector2(x2, y2), .velocity = Vector2(0.0f, 0.0f), .mass = drag, .pin = 0.0f }});

        const Span span = { .start = points.size() - 3, .end = points.size() };
        const PointMass &p0 = points[span.start];
        const PointMass &p1 = points[span.start + 1];
        const PointMass &p2 = points[span.start + 2];

        return Shape{ .start = span.start, .end = span.end, .isStatic = false };
    }

    Shape createLine(Array<PointMass> &points, Array<Spring> &springs, float x0, float y0, float x1, float y1, float drag) {
        PointMass p0 = {.pos = Vector2(x0, y0), .velocity = Vector2(0.0f, 0.0f), .mass = drag, .pin = 0.0f};
        PointMass p1 = {.pos = Vector2(x1, y1), .velocity = Vector2(0.0f, 0.0f), .mass = drag, .pin = 0.0f};
        points.append({ p0, p1 });

        const Span span = { .start = points.size() - 2, .end = points.size() };
        springs.push({ .pointA = span.start, .pointB = span.start + 1, .length = p0.pos.distance(p1.pos), .stiffness = 0.5f, .damping =  35.0f });

        return Shape{ .start = span.start, .end = span.end, .isStatic = false };
    }
}

