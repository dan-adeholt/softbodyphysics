#ifndef __PHYSICS__h
#define __PHYSICS__h

#include "../containers/Span.h"
#include "../containers/Array.h"

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

    static float vec2distance(float x1, float y1, float x2, float y2)
    {
        return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    }

    static float vec2length(float x, float y)
    {
        return sqrtf(x * x + y * y);
    }

    static float vec2dot(float x1, float y1, float x2, float y2)
    {
        return x1 * x2 + y1 * y2;
    }

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
struct PointMassesRange
{
    int size()
    {
        return this->x.size;
    }

    Range<float> x;
    Range<float> y;
    Range<float> mass;
    Range<float> velocityX;
    Range<float> velocityY;
    Range<float> accelerationX;
    Range<float> accelerationY;
};

struct PointMasses
{
    PointMasses()
    {
    }

    void clear()
    {
        x.clear();
        y.clear();
        mass.clear();
        velocityX.clear();
        velocityY.clear();
        accelerationX.clear();
        accelerationY.clear();
    }

    void append(const PointMasses &other)
    {
        x.append(other.x);
        y.append(other.y);
        mass.append(other.mass);
        velocityX.append(other.velocityX);
        velocityY.append(other.velocityY);
        accelerationX.append(other.accelerationX);
        accelerationY.append(other.accelerationY);
    }

    void push(float x, float y, float mass = 1.0f, float velocityX = 0.0f, float velocityY = 0.0f)
    {
        this->x.push(x);
        this->y.push(y);
        this->mass.push(mass);
        this->velocityX.push(velocityX);
        this->velocityY.push(velocityY);
        this->accelerationX.push(0.0f);
        this->accelerationY.push(0.0f);
    }

    int size()
    {
        return x.size();
    }

    PointMassesRange range(Span span) const
    {
        return range(span.start, span.end);
    }

    PointMassesRange range(int start, int end) const
    {
        return {
            x.range(start, end),
            y.range(start, end),
            mass.range(start, end),
            velocityX.range(start, end),
            velocityY.range(start, end),
            accelerationX.range(start, end),
            accelerationY.range(start, end)};
    }

    PointMassesRange range() const
    {
        return range(0, size());
    }

    void reserve(int size)
    {
        x.reserve(size);
        y.reserve(size);
        mass.reserve(size);
        velocityX.reserve(size);
        velocityY.reserve(size);
        accelerationX.reserve(size);
        accelerationY.reserve(size);
    }

    int size() const
    {
        return x.size();
    }

    Array<float> x;
    Array<float> y;
    Array<float> mass;
    Array<float> velocityX;
    Array<float> velocityY;
    Array<float> accelerationX;
    Array<float> accelerationY;
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
    PointMassesRange shape1,
    PointMassesRange shape2,
    const ShapeBoundingBox &box1,
    const ShapeBoundingBox &box2,
    Range<int> &collisionCounterForPoints1,
    Range<int> &collisionCounterForPoints2,
    double step);

int calculateStaticCollisions(
    PointMassesRange staticShape,
    PointMassesRange movingShape,
    const ShapeBoundingBox &staticBox,
    const ShapeBoundingBox &movingBox,
    Range<int> &collisionCounterForMovingShape,
    double step);

void findClosestLineSegmentToPoint(PointMassesRange collisionShape, const Vector2 &point, const Vector2 &velocity, int &minIndex, Vector2 &minPoint, float &minT);

void calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes, const Array<Shape> &shapes, const PointMasses &points);

void applySpringDerivatives(PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives);

void applyGravity(PointMassesRange &points, double timeStep);

ShapeBoundingBox calculateShapeBoundingBox(const Shape &shape, int shapeIndex, const PointMasses &points);

#endif