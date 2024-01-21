#ifndef __PHYSICS__h
#define __PHYSICS__h

#include "../containers/Span.h"

#define PI   3.141592653589793238463
#define PI_F 3.14159265358979f

class Vector2
{
public:
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    float x;
    float y;

    float dot() const {
        return x * x + y * y;
    }

    float dot(const Vector2& rhs) {
        return x * rhs.x + y * rhs.y;
    }    

    Vector2 normalized();

    Vector2 reflect(const Vector2& normal);
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;

    static Vector2 zero();
    static Vector2 one();
    static Vector2 up();
    static Vector2 down();
    static Vector2 left();
    static Vector2 right();
    static Vector2 fromAngle(float angle);

    static Vector2 lerp(const Vector2& a, const Vector2& b, float t);

    void operator +=(const Vector2& other);
    void operator -=(const Vector2& other);
    void operator *=(float scalar);
    void operator /=(float scalar);
    bool operator ==(const Vector2& other) const;
    bool operator !=(const Vector2& other) const;
    float length() const;
    float lengthSquared() const;
    float distance(const Vector2& other) const;
    float distanceSquared(const Vector2& other) const;
    float angle() const;
    float angle(const Vector2& other) const;
    Vector2 rotate(float angle) const;
};


struct PointMass
{
    float mass;
    Vector2 pos;
    Vector2 velocity;
    Vector2 force;
    float pin;
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
    bool isStatic;

    operator Span() const
    {
        return Span{start, end};
    }    
};

struct ShapeBoundingBox {
    int shapeIndex;
    float x1, y1;
    float x2, y2;
};

template<typename T> class Array;
template<typename T> class Range;

struct CollisionResult {
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
    Range<int> &collisionCounterForPoints2);

int calculateStaticCollisions(
    Range<PointMass> staticShape,
    Range<PointMass> movingShape,
    const ShapeBoundingBox& staticBox,
    const ShapeBoundingBox& movingBox,
    Range<int> &collisionCounterForMovingShape);

void calculateBoundingBoxes(Array<ShapeBoundingBox>& boundingBoxes, const Array<Shape>& shapes, const Array<PointMass>& points);


void applySprings(Range<PointMass>& points, Range<Spring>& springs, const Range<int>& counterForCollisions);

void applyGravity(Range<PointMass>& points);

#endif