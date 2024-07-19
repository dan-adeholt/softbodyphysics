#ifndef __PHYSICS__h
#define __PHYSICS__h

#include "../containers/Span.h"

extern "C"
{
    extern float sqrtf(float __x);
    extern float atan2f(float __y, float __x);
    extern float sinf(float __x);
    extern float cosf(float __x);
}

#define PI 3.141592653589793238463
#define PI_F 3.14159265358979f

class Vector2
{
public:
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    float x;
    float y;

    float dot() const
    {
        return x * x + y * y;
    }

    float dot(const Vector2 &rhs)
    {
        return x * rhs.x + y * rhs.y;
    }

    float cross(const Vector2 &rhs)
    {
        return x * rhs.y - y * rhs.x;
    }

    Vector2 normalized()
    {
        float length = sqrtf(x * x + y * y);
        return {x / length, y / length};
    }

    Vector2 reflect(const Vector2 &normal) const
    {
        return *this - normal * 2.0f * normal.dot();
    }

    Vector2 operator+(const Vector2 &other) const
    {
        return Vector2{x + other.x, y + other.y};
    }

    Vector2 operator-(const Vector2 &other) const
    {
        return Vector2{x - other.x, y - other.y};
    }
    Vector2 operator*(float scalar) const
    {
        return Vector2{x * scalar, y * scalar};
    }
    Vector2 operator/(float scalar) const
    {
        return Vector2{x / scalar, y / scalar};
    }

    static Vector2 zero();
    static Vector2 one();
    static Vector2 up();
    static Vector2 down();
    static Vector2 left();
    static Vector2 right();
    static Vector2 fromAngle(float angle)
    {
        return {cosf(angle), sinf(angle)};
    }

    static Vector2 lerp(const Vector2 &a, const Vector2 &b, float t)
    {
        return a + (b - a) * t;
    }

    constexpr void operator+=(const Vector2 &other) noexcept
    {
        x += other.x;
        y += other.y;
    }

    constexpr void operator-=(const Vector2 &other) noexcept
    {
        x -= other.x;
        y -= other.y;
    }

    constexpr void operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
    }
    constexpr void operator/=(float scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
    }

    constexpr bool operator==(const Vector2 &other) const
    {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const Vector2 &other) const
    {
        return x != other.x || y != other.y;
    }

    constexpr float length() const
    {
        return sqrtf(x * x + y * y);
    }
    constexpr float lengthSquared() const
    {
        return x * x + y * y;
    }

    float distance(const Vector2 &other) const
    {
        return (*this - other).length();
    }

    float distanceSquared(const Vector2 &other) const
    {
        return (*this - other).lengthSquared();
    }

    constexpr float angle() const
    {
        return atan2f(y, x);
    }

    constexpr float angle(const Vector2 &other) const
    {
        return atan2f(y - other.y, x - other.x);
    }

    Vector2 rotate(float angle) const
    {
        return {x * cosf(angle) - y * sinf(angle), x * sinf(angle) + y * cosf(angle)};
    }

    Vector2 normalVector() const
    {
        return Vector2(-y, x);
    }
};

struct PointMass
{
    PointMass(float x = 0.0f, float y = 0.0f, float mass = 1.0f) : mass(mass), pos(x, y), velocity(Vector2::zero()), acceleration(Vector2::zero()) {}
    float mass;
    Vector2 pos;
    Vector2 velocity;
    Vector2 acceleration;
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
    int pointA;
    int pointB;
    float length;
    float stiffness;
    float damping;
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
    Range<PointMass> shape1,
    Range<PointMass> shape2,
    const ShapeBoundingBox &box1,
    const ShapeBoundingBox &box2,
    Range<int> &collisionCounterForPoints1,
    Range<int> &collisionCounterForPoints2,
    double step);

int calculateStaticCollisions(
    Range<PointMass> staticShape,
    Range<PointMass> movingShape,
    const ShapeBoundingBox &staticBox,
    const ShapeBoundingBox &movingBox,
    Range<int> &collisionCounterForMovingShape,
    double step);

void findClosestLineSegmentToPoint(Range<PointMass> collisionShape, const Vector2 &point, const Vector2 &velocity, int &minIndex, Vector2 &minPoint, float &minT);

void calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes, const Array<Shape> &shapes, const Array<PointMass> &points);

void applySpringDerivatives(Range<PointMass> &points, Range<Spring> &springs, Range<PointDerivative> derivatives);

void applyGravity(Range<PointMass> &points, double timeStep);

ShapeBoundingBox calculateShapeBoundingBox(const Shape &shape, int shapeIndex, const Array<PointMass> &points);

#endif