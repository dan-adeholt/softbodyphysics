#ifndef __SHAPES_H
#define __SHAPES_H

struct Shape;
struct PhysicsSpace;

namespace Shapes
{
    Shape createCircle(PhysicsSpace &space, float x, float y, float radius, float mass);
    Shape createBridge(PhysicsSpace &space, float x, float y, float mass, int numSegments);
    Shape createLine(PhysicsSpace &space, float x0, float y0, float x1, float y1, float drag);
    Shape createQuad(PhysicsSpace &space, float x, float y, float width, float height, float drag, float stiffnessFactor = 20.0f);
    Shape createRoundedQuad(PhysicsSpace &space, float x, float y, float width, float height, float drag, float stiffnessFactor = 20.0f);
    Shape createStaticQuad(PhysicsSpace &space, float x, float y, float width, float height, float mass);
    Shape createParallelogram(PhysicsSpace &space, float x, float y, float width = 32.0f, float height = 32.0f, float sideOffset = 0.0f, float drag = 1.0f);
    Shape createTriangle(PhysicsSpace &space, bool isStatic, float x0, float y0, float x1, float y1, float x2, float y2, float drag = 1.0f);
}

#endif