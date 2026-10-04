#ifndef __GAME_PHYSICSSPACE_H__
#define __GAME_PHYSICSSPACE_H__

#include "./Physics.h"

template <typename T>
class Array;
struct ShapeEntry
{
    bool isStatic;
    PointMasses points;
};

struct PhysicsSpace
{
    PhysicsSpace();

    void assign(PhysicsSpace &other);

    void initFromEntries(const Array<ShapeEntry> &entries);

    int nextShapeIndex() const;

    int nextParentId() const;

    void updateIndices();

    void clear();

    void removeShape(int shapeIndex);

    Shape pasteShape(float x, float y, int copyIndex, const PhysicsSpace &sourceSpace);

    int closestPointIndex(float x, float y, int shapeIndex) const;

    Shape addShapeFromSpace(float x, float y, const PhysicsSpace &other, int resourceId);

    void triangulate();

    void addShape(const Shape &shape);
    Array<Shape> shapes;
    PointMasses points;
    Array<StaticJoint> staticJoints;
    Array<ShapeJoint> shapeJoints;
    Array<RadialAccelerator> radialAccelerators;
    Array<WheelMotor> wheelMotors;

    Array<int> triangleIndices;

    StaticJoint mouseJoint;
    bool gravityEnabled;
    // Downward acceleration in world units per millisecond squared
    float gravity = defaultGravity;
    bool collisionsEnabled;
    bool shapeMatchingEnabled;
    bool springsEnabled;

    bool isPrefab = false;

    static constexpr float defaultGravity = 0.00015f;

private:
    void removeShapeWithoutPoints(int shapeIndex);
};

#endif // __GAME_PHYSICSSPACE_H__
