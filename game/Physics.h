#ifndef __PHYSICS__h
#define __PHYSICS__h

#include "../containers/Span.h"
#include "../containers/Array.h"
#include "Vector2.h"

extern const float physicsStep;

struct PointMassesRange
{
    int size()
    {
        return this->pos.size;
    }

    Range<Vector2> pos;
    Range<float> mass;
    Range<Vector2> velocity;
    Range<Vector2> acceleration;
};

struct PointMasses
{
    PointMasses()
    {
    }

    void clear()
    {
        pos.clear();
        mass.clear();
        velocity.clear();
        acceleration.clear();
    }

    void append(const PointMasses &other)
    {
        pos.append(other.pos);
        mass.append(other.mass);
        velocity.append(other.velocity);
        acceleration.append(other.acceleration);
    }

    void push(float x, float y, float mass = 1.0f, float velocityX = 0.0f, float velocityY = 0.0f)
    {
        this->pos.push({x, y});

        this->mass.push(mass);
        this->velocity.push({velocityX, velocityY});
        this->acceleration.push({0.0f, 0.0f});
    }

    int size()
    {
        return pos.size();
    }

    PointMassesRange range(Span span) const
    {
        return range(span.start, span.end);
    }

    PointMassesRange range(int start, int end) const
    {
        return {
            pos.range(start, end),
            mass.range(start, end),
            velocity.range(start, end),
            acceleration.range(start, end)};
    }

    PointMassesRange range() const
    {
        return range(0, size());
    }

    void reserve(int size)
    {
        pos.reserve(size);
        mass.reserve(size);
        velocity.reserve(size);
        acceleration.reserve(size);
    }

    int size() const
    {
        return pos.size();
    }

    Array<Vector2> pos;
    Array<float> mass;
    Array<Vector2> velocity;
    Array<Vector2> acceleration;
};

struct PointDerivative
{
    PointDerivative() : velocity(Vector2::zero()), acceleration(Vector2::zero()) {}
    Vector2 velocity;
    Vector2 acceleration;
};

struct StaticJoint
{
    int pointIndex;
    Vector2 position;
};

struct Spring
{
    Spring() : pointA(0), pointB(0), length(0.0f), stiffness(0.0f), damping(0.0f), shapeIndex(0) {}
    Spring(int pointA, int pointB, float length, float stiffness, float damping, int shapeIndex)
        : pointA(pointA), pointB(pointB), length(length), stiffness(stiffness), damping(damping), shapeIndex(shapeIndex) {}
    int pointA;
    int pointB;
    float length;
    float stiffness;
    float damping;
    int shapeIndex;
};

struct Shape
{
    int start;
    int end;
    const char *name;
    float volume;

    operator Span() const
    {
        return Span{start, end};
    }
};

struct ShapeBoundingBox
{
    int shapeIndex;
    float x1, y1;
    float x2, y2;

    float width() const
    {
        return x2 - x1;
    }

    float height() const
    {
        return y2 - y1;
    }

    Vector2 center() const
    {
        return Vector2(x1 + (x2 - x1) * 0.5f, y1 + (y2 - y1) * 0.5f);
    }
};

template <typename T>
class Array;
template <typename T>
class Range;

struct CollisionResult
{
    int shape1PointIndex;
    int shape2PointIndex;
    float x;
    float y;
};

int calculateCollisions(
    PointMassesRange shape1,
    PointMassesRange shape2,
    const ShapeBoundingBox &box1,
    const ShapeBoundingBox &box2,
    Range<int> &collisionCounterForPoints1,
    Range<int> &collisionCounterForPoints2,
    float step);

int calculateStaticCollisions(
    PointMassesRange staticShape,
    PointMassesRange movingShape,
    const ShapeBoundingBox &staticBox,
    const ShapeBoundingBox &movingBox,
    Range<int> &collisionCounterForMovingShape,
    float step);

void findClosestLineSegmentToPoint(PointMassesRange collisionShape, const Vector2 &point, const Vector2 &velocity, int &minIndex, Vector2 &minPoint, float &minT);

void calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes, const Array<Shape> &shapes, const PointMasses &points);

void applySpringDerivatives(PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives);

ShapeBoundingBox calculateShapeBoundingBox(const Shape &shape, int shapeIndex, const PointMasses &points);

struct ConsoleProfileInfo;

struct RK4Integrator
{
    Array<PointDerivative> rk1;
    Array<PointDerivative> rk2;
    Array<PointDerivative> rk3;
    Array<PointDerivative> rk4;
    PointMasses rkTemp;
    Array<PointDerivative> rkEmptyDerivatives;

    void prepareRK4Step(PointMassesRange &initialState, Range<Spring> &springs, float dt, Array<PointDerivative> &derivatives, Array<PointDerivative> &outDerivatives, bool gravityEnabled, ConsoleProfileInfo &profileInfo);
    void updateRK4Springs(Range<Spring> &springs, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo);
    void performThreadedSpringDerivatives(PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives, ConsoleProfileInfo &profileInfo);
    void performRK4Integration(
        PointMassesRange &points,
        Range<Spring> &springs,
        Range<int> &collisionCounterForPoints,
        bool gravityEnabled,
        bool updateCollisions,
        ConsoleProfileInfo &profileInfo);

    void testRK4Performance(int iterations, PointMassesRange points, Range<Spring> springs);
    void testRK4PreparePerformance(int iterations, PointMassesRange points, Range<Spring> springs);
    void testSpringPerformance(int iterations, PointMassesRange points, Range<Spring> springs);
};

#endif