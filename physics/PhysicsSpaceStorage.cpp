#include "PhysicsSpaceStorage.h"
#include "PhysicsSpace.h"
#include <stdio.h>
#include <assert.h>
#include "../utils/Console.h"

void readVector2QuadArray(FILE *file, Vector2 *array)
{
    int size;
    fscanf(file, "%d\n", &size);
    assert(size == 4);

    for (int j = 0; j < size; ++j)
    {
        fscanf(file, "%f %f\n", &array->x, &array->y);
        array++;
    }
}

void readIntQuadArray(FILE *file, int *array)
{
    int size;
    fscanf(file, "%d\n", &size);
    assert(size == 4);

    for (int j = 0; j < size; ++j)
    {
        fscanf(file, "%d\n", array++);
    }
}

void readIntArray(FILE *file, Array<int> &array)
{
    int size;
    fscanf(file, "%d\n", &size);
    array.reserve(size);
    array.clear();

    for (int i = 0; i < size; ++i)
    {
        int value = 0;
        fscanf(file, "%d\n", &value);
        array.push(value);
    }
}

void readVector2Array(FILE *file, Array<Vector2> &array)
{
    int size;
    fscanf(file, "%d\n", &size);
    array.reserve(size);
    array.clear();

    for (int i = 0; i < size; ++i)
    {
        Vector2 value;
        fscanf(file, "%f %f\n", &value.x, &value.y);
        array.push(value);
    }
}

void readFloatArray(FILE *file, Array<float> &array)
{
    int size;
    fscanf(file, "%d\n", &size);
    array.reserve(size);
    array.clear();

    for (int i = 0; i < size; ++i)
    {
        float value;
        fscanf(file, "%f\n", &value);
        array.push(value);
    }
}

void readShapeArray(FILE *file, Array<Shape> &array)
{
    int size;
    fscanf(file, "size=%d\n", &size);
    array.reserve(size);
    array.clear();

    for (int i = 0; i < size; ++i)
    {
        Shape shape;
        fscanf(file, "start=%d end=%d substart=%d subend=%d volume=%f\n", &shape.start, &shape.end, &shape.subShapeSpan.start, &shape.subShapeSpan.end, &shape.volume);
        array.push(shape);
    }
}

void readPartialShapes(FILE *file, Array<ShapeQuad> &partialShapes)
{
    int size;
    fscanf(file, "%d\n", &size);
    partialShapes.reserve(size);
    partialShapes.clear();

    for (int j = 0; j < size; ++j)
    {
        ShapeQuad quad;
        fscanf(file, "i0=%d i1=%d i2=%d i3=%d\n", &quad.indices[0], &quad.indices[1], &quad.indices[2], &quad.indices[3]);
        readIntQuadArray(file, quad.indices);
        readVector2QuadArray(file, quad.shapePos);
        readVector2QuadArray(file, quad.originalPos);
        fscanf(file, "%d\n", &quad.size);
        partialShapes.push(quad);
    }
}

void dumpIntArray(FILE *file, int *array, int size)
{
    fprintf(file, "%d\n", size);

    for (int i = 0; i < size; ++i)
    {
        fprintf(file, "%d\n", array[i]);
    }
}

void dumpFloatArray(FILE *file, float *array, int size)
{
    fprintf(file, "%d\n", size);

    for (int i = 0; i < size; ++i)
    {
        fprintf(file, "%f\n", array[i]);
    }
}

void dumpVector2Array(FILE *file, Vector2 *array, int size)
{
    fprintf(file, "%d\n", size);

    for (int i = 0; i < size; ++i)
    {
        fprintf(file, "%f %f\n", array[i].x, array[i].y);
    }
}

void dumpShapeArray(FILE *file, Shape *array, int size)
{
    fprintf(file, "size=%d\n", size);

    for (int i = 0; i < size; ++i)
    {
        const Shape &shape = array[i];
        fprintf(file, "start=%d end=%d substart=%d subend=%d volume=%f\n", shape.start, shape.end, shape.subShapeSpan.start, shape.subShapeSpan.end, shape.volume);
    }
}

void dumpPartialShapes(FILE *file, Array<ShapeQuad> &partialShapes)
{

    fprintf(file, "%d\n", partialShapes.size());
    for (int j = 0; j < partialShapes.size(); ++j)
    {
        ShapeQuad quad = partialShapes[j];
        fprintf(file, "i0=%d i1=%d i2=%d i3=%d\n", quad.indices[0], quad.indices[1], quad.indices[2], quad.indices[3]);
        dumpIntArray(file, quad.indices, 4);
        dumpVector2Array(file, quad.shapePos, 4);
        dumpVector2Array(file, quad.originalPos, 4);
        fprintf(file, "%d\n", quad.size);
    }
}

void readPointMasses(FILE *file, PointMasses &points)
{
    readFloatArray(file, points.mass);
    readVector2Array(file, points.pos);
    readVector2Array(file, points.velocity);
    readVector2Array(file, points.shapeOriginalPos);
    readVector2Array(file, points.shapePos);
}

void dumpPointMasses(FILE *file, PointMasses &points)
{
    dumpFloatArray(file, &points.mass[0], points.size());
    dumpVector2Array(file, &points.pos[0], points.size());
    dumpVector2Array(file, &points.velocity[0], points.size());
    dumpVector2Array(file, &points.shapeOriginalPos[0], points.size());
    dumpVector2Array(file, &points.shapePos[0], points.size());
}

void readSprings(FILE *file, Array<Spring> &springs)
{
    int size;
    fscanf(file, "%d\n", &size);
    springs.reserve(size);
    springs.clear();

    for (int i = 0; i < size; ++i)
    {
        Spring spring;
        fscanf(file, "%d %d %f %f %f %d\n", &spring.pointA, &spring.pointB, &spring.length, &spring.stiffness, &spring.damping, &spring.shapeIndex);
        springs.push(spring);
    }
}

void dumpSprings(FILE *file, Array<Spring> &springs)
{
    fprintf(file, "%d\n", springs.size());

    for (int i = 0; i < springs.size(); ++i)
    {
        fprintf(file, "%d %d %f %f %f %d\n", springs[i].pointA, springs[i].pointB, springs[i].length, springs[i].stiffness, springs[i].damping, springs[i].shapeIndex);
    }
}

void readStaticJoints(FILE *file, Array<StaticJoint> &joints)
{
    int size;
    fscanf(file, "%d\n", &size);
    joints.reserve(size);
    joints.clear();

    for (int i = 0; i < size; ++i)
    {
        StaticJoint joint;
        fscanf(file, "%d %f %f\n", &joint.pointIndex, &joint.position.x, &joint.position.y);
        joints.push(joint);
    }
}

void dumpStaticJoints(FILE *file, Array<StaticJoint> &joints)
{
    fprintf(file, "%d\n", joints.size());

    for (int i = 0; i < joints.size(); ++i)
    {
        const StaticJoint &joint = joints[i];
        fprintf(file, "%d %f %f\n", joint.pointIndex, joint.position.x, joint.position.y);
    }
}

void PhysicsSpaceStorage::loadFromFile(PhysicsSpace &space, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file for reading");
        return;
    }
    readPointMasses(file, space.points);
    readShapeArray(file, space.shapes);
    readSprings(file, space.springs);
    readStaticJoints(file, space.staticJoints);

    fclose(file);
}

void PhysicsSpaceStorage::dumpToFile(PhysicsSpace &space, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Failed to open file for writing");
        return;
    }

    dumpPointMasses(file, space.points);
    dumpShapeArray(file, &space.shapes[0], space.shapes.size());
    dumpSprings(file, space.springs);
    dumpStaticJoints(file, space.staticJoints);

    fflush(file);
    fclose(file);
    Console::log("Dumped physics space to file %s", filename);
}
