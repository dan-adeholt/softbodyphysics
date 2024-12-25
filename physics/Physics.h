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

struct ShapeQuad
{
    int indices[4];
    Vector2 originalPos[4];
    Vector2 shapePos[4];
    int size;

    bool isPointInQuad(const Vector2 &point) const
    {
        int windingNumber = 0;
        for (int i = 0; i < size; i++)
        {
            Vector2 current = shapePos[i];
            Vector2 next = shapePos[(i + 1) % size];
            if (current.y <= point.y)
            {
                if (next.y > point.y && (next - current).cross(point - current) > 0)
                {
                    windingNumber++;
                }
            }
            else
            {
                if (next.y <= point.y && (next - current).cross(point - current) < 0)
                {
                    windingNumber--;
                }
            }
        }

        return windingNumber != 0;
    }
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
    Shape() : start(0), end(0), volume(0.0f), isStatic(false), subShapeSpan() {}
    Shape(int start, int end, float volume = 0.0f) : start(start), end(end), volume(volume), isStatic(false), subShapeSpan() {}

    int start;
    int end;
    float volume;
    bool isStatic;

    operator Span() const
    {
        return Span(start, end);
    }

    Span subShapeSpan;
};

template <typename T>
class Array;
template <typename T>
struct Range;

void applySpringDerivatives(PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives, Range<ShapeQuad> partialShapes, bool enableShapeMatching);

#endif