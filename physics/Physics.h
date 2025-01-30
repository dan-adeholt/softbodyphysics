#ifndef __PHYSICS__h
#define __PHYSICS__h

#include "../containers/Span.h"
#include "../containers/Array.h"
#include "../math/Vector2.h"

extern const float physicsStep;

struct PointMassesRange
{
    int size() const
    {
        return this->pos.size;
    }

    Range<Vector2> pos;
    Range<Vector2> shapeOriginalPos;
    Range<Vector2> shapePos;
    Range<float> mass;
    Range<Vector2> velocity;
};

struct PointMasses
{
    PointMasses()
    {
    }

    PointMasses(
        const Array<Vector2> &otherPos,
        const Array<Vector2> &otherVelocity,
        const Array<float> &otherMass,
        const Array<Vector2> &otherShapeOriginalPos,
        const Array<Vector2> &otherShapePos)
    {
        pos.append(otherPos);
        mass.append(otherMass);
        velocity.append(otherVelocity);
        shapePos.append(otherShapePos);
        shapeOriginalPos.append(otherShapeOriginalPos);
    }

    PointMasses &withPos(const Array<Vector2> &otherPos)
    {
        pos.append(otherPos);
        return *this;
    }

    PointMasses &withMass(const Array<float> &otherMass)
    {
        mass.append(otherMass);
        return *this;
    }

    PointMasses &withVelocity(const Array<Vector2> &otherVelocity)
    {
        velocity.append(otherVelocity);
        return *this;
    }

    PointMasses &withShapePos(const Array<Vector2> &otherShapePos)
    {
        shapePos.append(otherShapePos);
        return *this;
    }

    PointMasses &withShapeOriginalPos(const Array<Vector2> &otherShapeOriginalPos)
    {
        shapeOriginalPos.append(otherShapeOriginalPos);
        return *this;
    }

    void clear()
    {
        pos.clear();
        mass.clear();
        velocity.clear();
        shapePos.clear();
        shapeOriginalPos.clear();
    }

    void append(const PointMasses &other)
    {
        pos.append(other.pos);
        mass.append(other.mass);
        velocity.append(other.velocity);
        shapePos.append(other.shapePos);
        shapeOriginalPos.append(other.shapeOriginalPos);
    }

    void replace(const PointMasses &other)
    {
        pos.replace(other.pos);
        mass.replace(other.mass);
        velocity.replace(other.velocity);
        shapePos.replace(other.shapePos);
        shapeOriginalPos.replace(other.shapeOriginalPos);
    }

    void insert(int index, float x, float y, float mass)
    {
        pos.insert(index, {x, y});
        shapePos.insert(index, {x, y});
        shapeOriginalPos.insert(index, {x, y});
        this->mass.insert(index, mass);
        velocity.insert(index, {0.0f, 0.0f});
    }

    void push(float x, float y, float mass = 1.0f, float velocityX = 0.0f, float velocityY = 0.0f)
    {
        this->pos.push({x, y});
        this->shapePos.push({x, y});
        this->shapeOriginalPos.push({x, y});
        this->mass.push(mass);
        this->velocity.push({velocityX, velocityY});
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
            shapeOriginalPos.range(start, end),
            shapePos.range(start, end),
            mass.range(start, end),
            velocity.range(start, end)};
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
        shapePos.reserve(size);
        shapeOriginalPos.reserve(size);
    }

    int size() const
    {
        return pos.size();
    }

    Array<Vector2> pos;
    Array<Vector2> shapeOriginalPos;
    Array<Vector2> shapePos;
    Array<float> mass;
    Array<Vector2> velocity;
};

struct PointDerivative
{
    PointDerivative() : velocity(Vector2::zero()), acceleration(Vector2::zero()) {}
    Vector2 velocity;
    Vector2 acceleration;
};

struct PointJoint
{
    int pointIndex;
    int otherPointIndex;
};

struct StaticJoint
{
    int pointIndex;
    Vector2 position;
};

struct Spring
{

    Spring()
    {
    }
    Spring(int pointA, int pointB, float length, float stiffness, float damping, int shapeIndex)
        : pointA(pointA), pointB(pointB), length(length), stiffness(stiffness), damping(damping), shapeIndex(shapeIndex), parentShapeIndex(-1) {}

    int pointA;
    int pointB;
    float length;
    float stiffness;
    float damping;
    int shapeIndex;
    int parentShapeIndex;
};

struct Shape
{
    Shape() : start(0), end(0), volume(0.0f), isStatic(false), disableShapeMatching(false), parentIndex(-1) {}
    Shape(int start, int end, float volume = 0.0f) : start(start), end(end), volume(volume), isStatic(false), disableShapeMatching(false), parentIndex(-1) {}

    int start;
    int end;
    float volume;
    bool isStatic;
    bool disableShapeMatching;
    int parentIndex;

    operator Span() const
    {
        return Span(start, end);
    }
};

template <typename T>
class Array;
template <typename T>
struct Range;

void applySpringDerivatives(PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives, bool enableShapeMatching);

#endif