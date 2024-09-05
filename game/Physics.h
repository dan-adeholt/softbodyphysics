#ifndef __PHYSICS__h
#define __PHYSICS__h

#include "../containers/Span.h"
#include "../containers/Array.h"
#include "Vector2.h"
#include "../utils/Console.h"

extern const float physicsStep;

struct CollisionMap
{
    Array<unsigned char> data;
    int numElements;

    int calculateIndex(int i, int j) const
    {
        if (i > j)
        {
            int temp = i;
            i = j;
            j = temp;
        }

        return (j * (j - 1) / 2) + i;
    }

    static int arraySize(int numElements)
    {
        return (numElements * (numElements - 1)) / 2;
    }

    CollisionMap(int numElements) : data(CollisionMap::arraySize(numElements)), numElements(numElements) {}

    void resize(int numElements)
    {
        data.fill(0, CollisionMap::arraySize(numElements));
        this->numElements = numElements;
    }

    void clear()
    {
        data.fill(0, CollisionMap::arraySize(this->numElements));
    }

    void resetCollision(int i, int j)
    {
        int index = calculateIndex(i, j);
        data[index] = 0;
    }

    void incrementCollision(int i, int j)
    {
        int index = calculateIndex(i, j);
        if (data[index] < 255)
        {
            ++data[index];
        }
    }

    unsigned char getCollisionCount(int i, int j) const
    {
        return data[calculateIndex(i, j)];
    }
};

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
    Range<Vector2> acceleration;
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
        acceleration.clear();
        shapePos.clear();
        shapeOriginalPos.clear();
    }

    void append(const PointMasses &other)
    {
        pos.append(other.pos);
        mass.append(other.mass);
        velocity.append(other.velocity);
        acceleration.append(other.acceleration);
        shapePos.append(other.shapePos);
        shapeOriginalPos.append(other.shapeOriginalPos);
    }

    void replace(const PointMasses &other)
    {
        pos.replace(other.pos);
        mass.replace(other.mass);
        velocity.replace(other.velocity);
        acceleration.replace(other.acceleration);
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
            shapeOriginalPos.range(start, end),
            shapePos.range(start, end),
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
    Shape() : start(0), end(0), volume(0.0f), subShapeSpan() {}
    Shape(int start, int end, float volume = 0.0f) : start(start), end(end), volume(volume), subShapeSpan() {}

    int start;
    int end;
    float volume;

    operator Span() const
    {
        return Span(start, end);
    }

    Span subShapeSpan;
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

    bool includes(const Vector2 &point) const
    {
        return point.x >= x1 && point.x <= x2 && point.y >= y1 && point.y <= y2;
    }

    bool overlaps(const ShapeBoundingBox &other) const
    {
        return x1 < other.x2 && x2 > other.x1 && y1 < other.y2 && y2 > other.y1;
    }
};

template <typename T>
class Array;
template <typename T>
struct Range;

struct CollisionResult
{
    int shape1PointIndex;
    int shape2PointIndex;
    float x;
    float y;
};

int calculateCollisions(
    PointMassesRange shape1,
    PointMassesRange movingShape,
    PointMassesRange movingShapePrevPos,
    const ShapeBoundingBox &box1,
    const ShapeBoundingBox &box2,
    float step);

int calculateStaticCollisions(
    PointMassesRange staticShape,
    PointMassesRange movingShape,
    PointMassesRange movingShapePrevPos,
    const ShapeBoundingBox &staticBox,
    const ShapeBoundingBox &movingBox,
    float step);

void findClosestLineSegmentToPoint(PointMassesRange collisionShape, const Vector2 &point, const Vector2 &velocity, int &minIndex, Vector2 &minPoint, float &minT);

void calculateBoundingBoxes(Array<ShapeBoundingBox> &boundingBoxes, const Array<Shape> &shapes, const PointMasses &points);

void applySpringDerivatives(PointMassesRange &points, Range<Spring> &springs, Range<PointDerivative> derivatives, Range<ShapeQuad> partialShapes, bool enableShapeMatching);

void shapeMatchAlignInit(PointMassesRange points, Shape &shape, Range<ShapeQuad> partialShapes);

void shapeMatchAlign(PointMassesRange points, Array<Shape> &shapes, Range<ShapeQuad> partialShapes, int draggingShapeIndex);

ShapeBoundingBox calculateShapeBoundingBox(const Shape &shape, int shapeIndex, const PointMasses &points);

bool shapesOverlap(const PointMassesRange &poly1, const PointMassesRange &poly2);

bool pointInShape(const Vector2 &point, const PointMassesRange &shape);

struct PhysicsSpace;
struct ConsoleProfileInfo;

struct RK4Integrator
{
    Array<PointDerivative> rk1;
    Array<PointDerivative> rk2;
    Array<PointDerivative> rk3;
    Array<PointDerivative> rk4;
    PointMasses rkTemp;
    Array<PointDerivative> rkEmptyDerivatives;

    void prepareRK4Step(PhysicsSpace &PhysicsSpace, float dt, Array<PointDerivative> &derivatives, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo);
    void updateRK4Springs(PhysicsSpace &spaces, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo);
    void performThreadedSpringDerivatives(Range<Shape> shapeRange, PointMassesRange points, Range<Spring> springs, Range<PointDerivative> derivatives, Range<ShapeQuad> partialShapes, bool enableShapeMatching, ConsoleProfileInfo &profileInfo);
    void performRK4Integration(PhysicsSpace &space, ConsoleProfileInfo &profileInfo);

    void testRK4Performance(int iterations, PhysicsSpace &space);
    void testRK4PreparePerformance(int iterations, PhysicsSpace &space);
    void testSpringPerformance(int iterations, PhysicsSpace &space);
};

struct PhysicsSpace
{
    PhysicsSpace();

    void assign(PhysicsSpace &other);

    int nextShapeIndex() const;
    int nextStaticShapeIndex() const;

    void clear();

    Array<Shape> shapes;
    Array<ShapeQuad> partialShapes;
    PointMasses points;
    Array<Shape> staticShapes;
    PointMasses staticPoints;
    Array<Spring> springs;
    Array<StaticJoint> staticJoints;

    StaticJoint mouseJoint;
    bool gravityEnabled;
    bool collisionsEnabled;
    bool shapeMatchingEnabled;
    bool springsEnabled;
    int draggingShapeIndex;
    int draggingSubShapeIndex;
};

struct PhysicsIntegrator
{
    RK4Integrator rk4Integrator;
    void performIntegration(PhysicsSpace &space, ConsoleProfileInfo &profileInfo);
};

struct CollisionPair
{
    int shape1Index;
    int shape2Index;
};

struct PhysicsCollisionSolver
{
    PhysicsCollisionSolver();
    void clear();
    void handleCollisions(PhysicsSpace &space, PhysicsSpace &prevSpace, float step, ConsoleProfileInfo &profileInfo);

    CollisionMap collisionMap;
    Array<CollisionPair> resolvedCollisionPairs;
    Array<ShapeBoundingBox> boundingBoxes;
    Array<ShapeBoundingBox> staticBoundingBoxes;
    Array<ShapeBoundingBox> sortedBoundingBoxes;
    Array<ShapeBoundingBox> sortedStaticBoundingBoxes;
    Array<int> ejectShapeIndices;
};

Vector2 intersectLineSegmentPoint(const Vector2 &p0, const Vector2 &p1, Vector2 d);

#endif