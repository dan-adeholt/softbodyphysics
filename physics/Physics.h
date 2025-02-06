#ifndef __PHYSICS__h
#define __PHYSICS__h

#include "../containers/Span.h"
#include "../containers/Array.h"
#include "../math/Vector2.h"
#include <stdint.h>

extern const float physicsStep;

struct PointMassesRange
{
    int size() const
    {
        return this->pos.size;
    }

    Range<Vector2> pos;
    Range<Vector2> shapeOriginalPos;
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
        const Array<Vector2> &otherShapeOriginalPos)
    {
        pos.append(otherPos);
        mass.append(otherMass);
        velocity.append(otherVelocity);
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
        shapeOriginalPos.clear();
    }

    void append(const PointMasses &other)
    {
        pos.append(other.pos);
        mass.append(other.mass);
        velocity.append(other.velocity);
        shapeOriginalPos.append(other.shapeOriginalPos);
    }

    void replace(const PointMasses &other)
    {
        pos.replace(other.pos);
        mass.replace(other.mass);
        velocity.replace(other.velocity);
        shapeOriginalPos.replace(other.shapeOriginalPos);
    }

    void insert(int index, float x, float y, float mass)
    {
        pos.insert(index, {x, y});
        shapeOriginalPos.insert(index, {x, y});
        this->mass.insert(index, mass);
        velocity.insert(index, {0.0f, 0.0f});
    }

    void push(float x, float y, float mass = 1.0f, float velocityX = 0.0f, float velocityY = 0.0f)
    {
        this->pos.push({x, y});
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
        shapeOriginalPos.reserve(size);
    }

    int size() const
    {
        return pos.size();
    }

    Array<Vector2> pos;
    Array<Vector2> shapeOriginalPos;
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
    Shape() : start(0), end(0), volume(0.0f), isStatic(false), disableShapeMatching(false), index(-1), parentId(-1) {}
    Shape(int start, int end, float volume = 0.0f) : start(start), end(end), volume(volume), isStatic(false), disableShapeMatching(false), index(-1), parentId(-1) {}

    int start;
    int end;
    float volume;
    bool isStatic;
    bool disableShapeMatching;
    int index;
    int parentId;

    uint16_t indices[4] = {UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX};
    bool interiorEdges[4] = {false, false, false, false};

    bool hasIndices() const
    {
        return indices[0] != UINT16_MAX;
    }

    int size() const
    {
        return hasIndices() ? 4 : end - start;
    }
};

struct ShapeProperties
{
    Vector2 center;
    Vector2 origCenter;
    float diffAngle;
};

struct ShapeMatchDragData
{
    ShapeMatchDragData()
    {
        dragShapeIndex = -1;
    }

    Vector2 center;
    int dragShapeIndex;
};

template <typename T>
class Array;
template <typename T>
struct Range;

void applySpringDerivatives(PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives, bool enableShapeMatching);

struct ShapeIterator
{
    int cur = 0;
    int end = 0;
    int indices[4];
    bool indexed = false;

    ShapeIterator(const Shape &shape)
    {
        indexed = shape.hasIndices();
        cur = indexed ? 0 : shape.start;
        end = indexed ? 4 : shape.end;

        for (int i = 0; i < 4; i++)
        {
            indices[i] = shape.start + shape.indices[i];
        }
    }

    bool isValid() const
    {
        return cur < end;
    }

    int index()
    {
        return indexed ? indices[cur] : cur;
    }

    void next()
    {
        cur++;
    }
};

struct ShapeIndexedRange
{
    ShapeIndexedRange(const Shape &shape)
    {
        indexed = shape.hasIndices();
        start = indexed ? 0 : shape.start;
        end = indexed ? 4 : shape.end;

        if (indexed)
        {
            for (int i = 0; i < 4; i++)
            {
                indices[i] = shape.start + shape.indices[i];
            }

            for (int i = 0; i < 4; i++)
            {
                interiorEdges[i] = shape.interiorEdges[i];
            }
        }
    }

    bool hasInteriorEdge(int i) const
    {
        return indexed ? interiorEdges[i] : false;
    }

    int operator[](int i) const
    {
        return indexed ? indices[i] : start + i;
    }

    int size() const
    {
        return end - start;
    }

private:
    int start = 0;
    int end = 0;
    int indices[4] = {0, 0, 0, 0};
    bool interiorEdges[4] = {false, false, false, false};
    bool indexed = false;
};

#endif