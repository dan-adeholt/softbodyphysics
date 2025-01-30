#ifndef __VECTOR2__h
#define __VECTOR2__h

#include <initializer_list>

extern "C"
{
    extern float sqrtf(float __x);
    extern float atan2f(float __y, float __x);
    extern float sinf(float __x);
    extern float cosf(float __x);
    extern float fabsf(float __x);
}

#define PI 3.141592653589793238463
#define PI_F 3.14159265358979f

class Vector2
{
public:
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    // Implicit constructor for brace initialization
    Vector2(std::initializer_list<float> list)
    {
        auto it = list.begin();
        x = (it != list.end()) ? *it++ : 0.0f;
        y = (it != list.end()) ? *it++ : 0.0f;
    }

    float x;
    float y;

    bool isZero() const
    {
        return x == 0.0f && y == 0.0f;
    }

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

    // Unary negation operator
    Vector2 operator-() const
    {
        return Vector2(-x, -y);
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
        float angle = atan2f(y, x);

        if (angle < 0.0f)
        {
            angle += 2.0f * PI_F;
        }

        return angle;
    }

    constexpr float angle(const Vector2 &other) const
    {
        float angle1 = atan2f(y, x);
        float angle2 = atan2f(other.y, other.x);

        float diff = angle2 - angle1;

        // Normalize to [-π, π]
        if (diff > PI_F)
        {
            diff -= 2 * PI_F;
        }
        else if (diff < -PI_F)
        {
            diff += 2 * PI_F;
        }

        return diff;
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

bool lineSegmentIntersection(
    const Vector2 &p1, const Vector2 &p2,
    const Vector2 &q1, const Vector2 &q2,
    float &t_p, float &t_q);

Vector2 closestPointToLineSegment(const Vector2 &p0, const Vector2 &p1, const Vector2 &point);

Vector2 closestPointToAxis(const Vector2 &p0, const Vector2 &p1, const Vector2 &point);

Vector2 intersectLineSegmentPoint(const Vector2 &p0, const Vector2 &p1, Vector2 d);

#endif