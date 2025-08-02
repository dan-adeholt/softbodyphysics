#ifndef __SHAPES_H
#define __SHAPES_H

struct Shape;
struct PhysicsSpace;
class Vector2;
template <typename T>
struct Range;

namespace Shapes
{
    void recalculateOriginalPos(PhysicsSpace &space, const Shape &shape);
    void snapToGrid(PhysicsSpace &space, int shapeIndex);
    void addPointToShape(PhysicsSpace &space, int shapeIndex, float x, float y);

    void handlePointInserted(PhysicsSpace &space, int shapeStart, int newPointIndex);

    void addSubshapeToShape(PhysicsSpace &space, const Vector2 &position, const Range<Vector2> &points);

    void updateOriginalPos(PhysicsSpace &space, int shapeIndex);
    void resetShape(PhysicsSpace &space, int shapeIndex);

    int createMesh(PhysicsSpace &space, float x, float y, float width, float height, int numSegments, float mass, float stiffnessFactor = 1.0f);
    Shape createCircle(PhysicsSpace &space, float x, float y, float radius, float mass, float stiffnessFactor = 1.0f);
    int createBridge(PhysicsSpace &space, float x, float y, float mass, int numSegments, float segmentWidth = 70.0f, float segmentHeight = 70.0f);
    Shape createLine(PhysicsSpace &space, float x0, float y0, float x1, float y1, float mass);
    Shape createQuad(PhysicsSpace &space, float x, float y, float width, float height, float mass, float stiffnessFactor = 1.0f);
    Shape createRoundedQuad(PhysicsSpace &space, float x, float y, float width, float height, float mass, float stiffnessFactor = 1.0f);
    Shape createStaticQuad(PhysicsSpace &space, float x, float y, float width, float height, float mass);
    Shape createParallelogram(PhysicsSpace &space, float x, float y, float width = 32.0f, float height = 32.0f, float sideOffset = 0.0f, float mass = 1.0f);
    Shape createTriangle(PhysicsSpace &space, bool isStatic, float x0, float y0, float x1, float y1, float x2, float y2, float mass = 1.0f);

    void createCar(PhysicsSpace &space, float x, float y, const PhysicsSpace &prefabSpace);
}

#endif