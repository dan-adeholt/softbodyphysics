#ifndef __PHYSICS__h
#define __PHYSICS__h

#include "../containers/Span.h"
#include "../containers/Array.h"
#include "../math/Vector2.h"
#include "../utils/Console.h"
#include <stdint.h>

extern const float baseStiffness;
extern const float baseDamping;

extern const float gridSize;
extern const float physicsStep;
extern const float minPointSnapDist;

struct BoundingBox;

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

struct Vertex
{
    Vector2 pos;
    Vector2 originalPos;
    float mass = 1.0f;
    Vector2 velocity;
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

    void insert(int index, Vertex vertex)
    {
        pos.insert(index, vertex.pos);
        shapeOriginalPos.insert(index, vertex.originalPos);
        mass.insert(index, vertex.mass);
        velocity.insert(index, vertex.velocity);
    }

    void insert(int index, float x, float y, float origX, float origY, float mass)
    {
        pos.insert(index, {x, y});
        shapeOriginalPos.insert(index, {origX, origY});
        this->mass.insert(index, mass);
        velocity.insert(index, {0.0f, 0.0f});
    }

    void push(Vertex vertex)
    {
        pos.push(vertex.pos);
        shapeOriginalPos.push(vertex.originalPos);
        mass.push(vertex.mass);
        velocity.push(vertex.velocity);
    }

    void push(float x, float y, float mass = 1.0f)
    {
        this->pos.push({x, y});
        this->shapeOriginalPos.push({x, y});
        this->mass.push(mass);
        this->velocity.push({0.0f, 0.0f});
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

struct ShapeJoint
{
    int shapeIndex1;
    int shape1Points[4] = {-1, -1, -1, -1};
    float shape1Weights[4] = {0.25f, 0.25f, 0.25f, 0.25f};
    int shapeIndex2;
    int shape2Points[4] = {-1, -1, -1, -1};
    float shape2Weights[4] = {0.25f, 0.25f, 0.25f, 0.25f};
};

struct RadialAccelerator
{
    int shapeIndex;
    bool enabled = false;
    float strength = 1.0f;
    float limit = 0.0f;
};

enum class WheelMotorMode
{
    Coast = 0,
    Drive,
    Handover,
    Brake,
    Air
};

struct WheelMotor
{
    int shapeIndex = -1;
    int parentId = -1;
    bool enabled = false;
    float command = 0.0f;
    float targetSurfaceSpeed = 0.0f;
    float maxDriveImpulsePerStep = 0.0f;
    float maxBrakeImpulsePerStep = 0.0f;
    float reverseEngageSpeed = 0.0f;
    float freeSpinDamping = 0.0f;

    float lastCommand = 0.0f;
    bool groundedThisStep = false;
    int groundedContactCount = 0;
    Vector2 groundedTangentSum;
    Vector2 groundedGroundVelocitySum;
    float lastSurfaceSpeed = 0.0f;
    float lastSurfaceSpeedError = 0.0f;
    float lastAppliedImpulse = 0.0f;
    float lastParentForwardSpeed = 0.0f;
    float lastGroundSpeed = 0.0f;
    float lastRelativeForwardSpeed = 0.0f;
    float lastCommandSpaceSpeed = 0.0f;
    float lastAuthorityClamp = 0.0f;
    float lastHandoverBand = 0.0f;
    WheelMotorMode lastMode = WheelMotorMode::Coast;
};

struct StaticJoint
{
    int pointIndex;
    Vector2 position;
};

struct Shape
{
    Shape() : start(0), end(0), volume(0.0f), isStatic(false), index(-1), parentId(-1) {}
    Shape(int start, int end, float volume = 0.0f) : start(start), end(end), volume(volume), isStatic(false), index(-1), parentId(-1) {}

    int resourceId = 0;
    int texture = 0;
    int start;
    int end;
    float volume;
    bool isStatic;
    int index;
    int parentId;
    bool selfIntersecting = false;
    float stiffness = 1.0f;
    float damping = 1.0f;

    // Not serialized (as of now), recalculated on triangulation
    int triangleStart = -1;
    int triangleEnd = -1;

    uint16_t indices[4] = {UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX};
    bool interiorEdges[4] = {false, false, false, false};

    int numIndices() const
    {
        int count = 0;
        for (int i = 0; i < 4; i++)
        {
            if (indices[i] != UINT16_MAX)
            {
                count++;
            }
        }
        return count;
    }

    bool allInteriorEdges() const
    {
        return interiorEdges[0] && interiorEdges[1] && interiorEdges[2] && interiorEdges[3];
    }

    bool hasIndices() const
    {
        return indices[0] != UINT16_MAX;
    }

    int size() const
    {
        return hasIndices() ? numIndices() : end - start;
    }

    int commonEdge(const Shape &other) const
    {
        if (hasIndices() && other.hasIndices() && other.start == start)
        {
            int o1 = other.indices[0];
            int o2 = other.indices[1];
            int o3 = other.indices[2];
            int o4 = other.indices[3];

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (indices[i] == other.indices[j] && indices[i] != UINT16_MAX)
                    {
                        {
                            // Check if the prev index starts the edge
                            int prev = i == 0 ? 3 : i - 1;
                            int next = i == 3 ? 0 : i + 1;

                            if (indices[prev] == o1 || indices[prev] == o2 || indices[prev] == o3 || indices[prev] == o4)
                            {
                                return prev;
                            }
                            else if (indices[next] == o1 || indices[next] == o2 || indices[next] == o3 || indices[next] == o4)
                            {
                                return i;
                            }
                        }
                    }
                }
            }
        }

        return -1;
    }
};

struct ShapeProperties
{
    Vector2 center;
    Vector2 origCenter;
    float diffAngle;
};

struct ShapeVelocities
{
    Vector2 centerOfMassVelocity;
    Vector2 centerOfMass;
    float angularVelocity;
};

enum AddSubshapeShape
{
    SUBSHAPE_RECT = 0,
    SUBSHAPE_TRIANGLE_1,
    SUBSHAPE_TRIANGLE_2,
    SUBSHAPE_TRIANGLE_3,
    SUBSHAPE_TRIANGLE_4,
    NUM_SUBSHAPES
};

struct AddSubShapeData
{
    bool active = false;
    bool mouseDown = false;

    AddSubshapeShape shape = AddSubshapeShape::SUBSHAPE_RECT;
    int repeatX = 0;
    int repeatY = 0;

    Vector2 sourcePos;
    int numPoints = 4;
    Vector2 points[4] = {
        Vector2(0, 0), Vector2(gridSize, 0.0f), Vector2(gridSize, gridSize), Vector2(0, gridSize)};
};

struct ShapeMatchDragData
{
    ShapeMatchDragData()
    {
        dragShapeIndex = -1;
        center = Vector2::zero();
    }

    Vector2 center;
    int dragShapeIndex;
};

template <typename T>
class Array;
template <typename T>
struct Range;

void applySpringDerivatives(PointMassesRange &points, Range<PointDerivative> derivatives, bool enableShapeMatching);

struct ShapeIterator
{
    int cur = 0;
    int end = 0;
    int indices[4] = {-1, -1, -1, -1};
    bool interiorEdges[4] = {false, false, false, false};
    bool indexed = false;

    ShapeIterator(const Shape &shape)
    {
        indexed = shape.hasIndices();
        cur = indexed ? 0 : shape.start;
        end = indexed ? shape.numIndices() : shape.end;

        if (indexed)
        {
            for (int i = 0; i < 4; i++)
            {
                indices[i] = shape.start + shape.indices[i];
                interiorEdges[i] = shape.interiorEdges[i];
            }
        }
    }

    bool isValid() const
    {
        return cur < end;
    }

    bool isInteriorEdge() const
    {
        return indexed ? interiorEdges[cur] : false;
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
        end = indexed ? shape.numIndices() : shape.end;

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

struct CollisionGridCell
{
    static const int maxNumIndices = 128;
    int indices[maxNumIndices] = {-1, -1, -1, -1, -1, -1, -1, -1,
                                  -1, -1, -1, -1, -1, -1, -1, -1,
                                  -1, -1, -1, -1, -1, -1, -1, -1,
                                  -1, -1, -1, -1, -1, -1, -1, -1};
    int numIndices = 0;
};

struct CollisionGridSimple
{
    float originX = 0.0f;
    float originY = 0.0f;

    static const int width = 82;
    static const int height = 60;
    int cellSize = 15;

    CollisionGridCell cells[width * height];

    void init(Array<BoundingBox> &boundingBoxes);
};

#endif
