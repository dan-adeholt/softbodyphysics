#include "PhysicsSpaceStorage.h"
#include "PhysicsSpace.h"
#include <stdio.h>
#include <assert.h>
#include "../utils/Console.h"
#include "../containers/StringBuffer.h"
#include <SDL.h>
#include <dirent.h>

//  interiorEdges=0,1,0,0
//  interiorEdges=0,1,0,1
//  interiorEdges=0,0,0,1

void readVector2QuadArray(FILE *file, Vector2 *array)
{
    int size = 0;
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
    int size = 0;
    fscanf(file, "%d\n", &size);
    assert(size == 4);

    for (int j = 0; j < size; ++j)
    {
        fscanf(file, "%d\n", array++);
    }
}

void readIntArray(FILE *file, Array<int> &array)
{
    int size = 0;
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
    int size = 0;
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
    int size = 0;
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
    int size = 0;
    fscanf(file, "shapes=%d\n", &size);
    array.reserve(size);
    array.clear();

    for (int i = 0; i < size; ++i)
    {
        Shape shape;
        int isStatic = 0;
        int disableShapeMatching = 0;
        int interiorEdges[4] = {0, 0, 0, 0};
        int selfIntersecting = 0;

        fscanf(file, "start=%d end=%d volume=%f isStatic=%d selfIntersecting=%d parentId=%d disableShapeMatching=%d indices=%hu,%hu,%hu,%hu interiorEdges=%d,%d,%d,%d\n", &shape.start, &shape.end, &shape.volume, &isStatic, &selfIntersecting, &shape.parentId, &disableShapeMatching, &shape.indices[0], &shape.indices[1], &shape.indices[2], &shape.indices[3], &interiorEdges[0], &interiorEdges[1], &interiorEdges[2], &interiorEdges[3]);
        shape.isStatic = isStatic != 0;
        shape.selfIntersecting = selfIntersecting != 0;
        shape.disableShapeMatching = disableShapeMatching != 0;
        shape.interiorEdges[0] = interiorEdges[0] != 0;
        shape.interiorEdges[1] = interiorEdges[1] != 0;
        shape.interiorEdges[2] = interiorEdges[2] != 0;
        shape.interiorEdges[3] = interiorEdges[3] != 0;

        array.push(shape);
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
    fprintf(file, "shapes=%d\n", size);

    for (int i = 0; i < size; ++i)
    {
        const Shape &shape = array[i];
        fprintf(file, "start=%d end=%d volume=%f isStatic=%d selfIntersecting=%d parentId=%d disableShapeMatching=%d indices=%hu,%hu,%hu,%hu interiorEdges=%d,%d,%d,%d\n", shape.start, shape.end, shape.volume, shape.isStatic ? 1 : 0, shape.selfIntersecting ? 1 : 0, shape.parentId, shape.disableShapeMatching ? 1 : 0, shape.indices[0], shape.indices[1], shape.indices[2], shape.indices[3], shape.interiorEdges[0] ? 1 : 0, shape.interiorEdges[1] ? 1 : 0, shape.interiorEdges[2] ? 1 : 0, shape.interiorEdges[3] ? 1 : 0);
    }
}

void readPointMasses(FILE *file, PointMasses &points)
{
    readFloatArray(file, points.mass);
    readVector2Array(file, points.pos);
    readVector2Array(file, points.velocity);
    readVector2Array(file, points.shapeOriginalPos);
}

void dumpPointMasses(FILE *file, PointMasses &points)
{
    dumpFloatArray(file, &points.mass[0], points.size());
    dumpVector2Array(file, &points.pos[0], points.size());
    dumpVector2Array(file, &points.velocity[0], points.size());
    dumpVector2Array(file, &points.shapeOriginalPos[0], points.size());
}

void readSprings(FILE *file, Array<Spring> &springs)
{
    int size = 0;
    fscanf(file, "springs=%d\n", &size);
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
    fprintf(file, "springs=%d\n", springs.size());

    for (int i = 0; i < springs.size(); ++i)
    {
        fprintf(file, "%d %d %f %f %f %d\n", springs[i].pointA, springs[i].pointB, springs[i].length, springs[i].stiffness, springs[i].damping, springs[i].shapeIndex);
    }
}

void readStaticJoints(FILE *file, Array<StaticJoint> &joints)
{
    int size = 0;
    fscanf(file, "staticjoints=%d\n", &size);
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
    fprintf(file, "staticjoints=%d\n", joints.size());

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
    printf("Loading physics space from file %s\n", filename);

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

int getHighestFileNumber(const char *directory, const char *format)
{
    DIR *dir;
    struct dirent *entry;
    int max_number = 0;

    dir = opendir(directory);
    if (!dir)
    {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        int number;
        if (sscanf(entry->d_name, format, &number) == 1)
        {
            if (number > max_number)
            {
                max_number = number;
            }
        }
    }

    closedir(dir);
    return max_number;
}

void PhysicsSpaceStorage::dumpToUnitTest(PhysicsSpace &space)
{
    int saveNumber = getHighestFileNumber("scenedefs", "unit_%d.txt") + 1;
    StringBuffer<256> filename;
    filename.append("scenedefs/unit_%d.txt", saveNumber);
    dumpToFile(space, filename.data);

    const char *tempFileName = "/tmp/physics_space_copy.txt";

    FILE *f = fopen(tempFileName, "w");

    fprintf(f, "    {\"New Unit Test %d\", [](Game *game)\n", saveNumber);
    fprintf(f, "      {\n");
    fprintf(f, "         PhysicsSpace &space = game->physicsSpace();\n");
    fprintf(f, "         PhysicsSpaceStorage::loadFromFile(space, \"%s\");\n", filename.data);
    fprintf(f, "         game->setPaused();\n");
    fprintf(f, "    }},\n");
    fclose(f);
    fflush(f);

    // Open the file
    FILE *file = fopen(tempFileName, "r");
    if (!file)
    {
        fprintf(stderr, "Failed to open file: %s\n", tempFileName);
        return;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    size_t fileSize = (size_t)ftell(file);
    rewind(file);

    // Allocate memory to read the file contents
    char *buffer = (char *)malloc(fileSize + 1);
    if (!buffer)
    {
        fprintf(stderr, "Failed to allocate memory for file contents.\n");
        fclose(file);
        return;
    }

    // Read the file contents
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0'; // Null-terminate the string
    fclose(file);

    // Set the clipboard text using SDL
    if (SDL_SetClipboardText(buffer) != 0)
    {
        fprintf(stderr, "Failed to set clipboard text: %s\n", SDL_GetError());
    }
    else
    {
        printf("Clipboard updated successfully!\n");
    }

    // Free the buffer
    free(buffer);
}