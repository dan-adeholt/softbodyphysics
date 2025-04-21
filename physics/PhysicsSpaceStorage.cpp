#include "PhysicsSpaceStorage.h"
#include "PhysicsSpace.h"
#include "ShapeUtils.h"
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
        int interiorEdges[4] = {0, 0, 0, 0};
        int selfIntersecting = 0;

        fscanf(file, "start=%d end=%d stiffness=%f damping=%f volume=%f isStatic=%d selfIntersecting=%d parentId=%d indices=%hu,%hu,%hu,%hu interiorEdges=%d,%d,%d,%d\n", &shape.start, &shape.end, &shape.stiffness, &shape.damping, &shape.volume, &isStatic, &selfIntersecting, &shape.parentId, &shape.indices[0], &shape.indices[1], &shape.indices[2], &shape.indices[3], &interiorEdges[0], &interiorEdges[1], &interiorEdges[2], &interiorEdges[3]);

        shape.isStatic = isStatic != 0;
        shape.selfIntersecting = selfIntersecting != 0;
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
        fprintf(file, "start=%d end=%d stiffness=%f damping=%f volume=%f isStatic=%d selfIntersecting=%d parentId=%d indices=%hu,%hu,%hu,%hu interiorEdges=%d,%d,%d,%d\n", shape.start, shape.end, shape.stiffness, shape.damping, shape.volume, shape.isStatic ? 1 : 0, shape.selfIntersecting ? 1 : 0, shape.parentId, shape.indices[0], shape.indices[1], shape.indices[2], shape.indices[3], shape.interiorEdges[0] ? 1 : 0, shape.interiorEdges[1] ? 1 : 0, shape.interiorEdges[2] ? 1 : 0, shape.interiorEdges[3] ? 1 : 0);
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
    readStaticJoints(file, space.staticJoints);

    fclose(file);
}

void PhysicsSpaceStorage::appendFromFile(PhysicsSpace &space, const char *filename, const Vector2 &posOffset)
{
    PhysicsSpace tempSpace;
    loadFromFile(tempSpace, filename);

    int pointStart = space.points.size();
    space.points.append(tempSpace.points);

    for (int i = pointStart; i < space.points.size(); i++)
    {
        space.points.pos[i] += posOffset;
    }

    int shapeStart = space.shapes.size();
    space.shapes.append(tempSpace.shapes);

    for (int i = shapeStart; i < space.shapes.size(); i++)
    {
        Shape &shape = space.shapes[i];
        shape.start += pointStart;
        shape.end += pointStart;
    }

    int springsStart = space.springs.size();
    space.springs.append(tempSpace.springs);
    for (int i = springsStart; i < space.springs.size(); i++)
    {
        Spring &spring = space.springs[i];
        spring.shapeIndex += shapeStart;
        spring.pointA += pointStart;
        spring.pointB += pointStart;
    }

    int staticJointsStart = space.staticJoints.size();
    space.staticJoints.append(tempSpace.staticJoints);

    for (int i = staticJointsStart; i < space.staticJoints.size(); i++)
    {
        StaticJoint &joint = space.staticJoints[i];
        joint.pointIndex += pointStart;
    }

    int shapeJointsStart = space.shapeJoints.size();
    space.shapeJoints.append(tempSpace.shapeJoints);

    for (int i = shapeJointsStart; i < space.shapeJoints.size(); i++)
    {
        ShapeJoint &joint = space.shapeJoints[i];
        joint.shapeIndex1 += shapeStart;
        joint.shapeIndex2 += shapeStart;

        for (int j = 0; j < 4; j++)
        {
            joint.shape1Points[j] += pointStart;
            joint.shape2Points[j] += pointStart;
        }
    }
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

void PhysicsSpaceStorage::dumpToPrefab(PhysicsSpace &space, int selectedShapeIndex)
{
    int saveNumber = getHighestFileNumber("scenedefs", "prefab_%d.txt") + 1;
    StringBuffer<256> filename;
    filename.append("scenedefs/prefab_%d.txt", saveNumber);

    PhysicsSpace tempSpace;

    Shape &selectedShape = space.shapes[selectedShapeIndex];

    if (selectedShapeIndex == -1)
    {
        Console::log("No shape selected");
    }
    else if (selectedShape.parentId != -1 || selectedShape.hasIndices())
    {
        Console::log("Multi-shapes not supported for prefabs");
        return;
    }

    const Shape &shape = space.shapes[selectedShapeIndex];
    ShapeProperties averages = ShapeUtils::getShapeProperties(space.points.range(), shape);

    for (int i = shape.start; i < shape.end; i++)
    {
        tempSpace.points.pos.push(space.points.pos[i] - averages.center);
        tempSpace.points.velocity.push(Vector2());
        tempSpace.points.mass.push(space.points.mass[i]);
        tempSpace.points.shapeOriginalPos.push(space.points.pos[i] - averages.center);
    }

    Shape newShape(shape);
    newShape.isStatic = false;
    newShape.start = 0;
    newShape.end = shape.end - shape.start;
    tempSpace.shapes.push(newShape);

    for (int i = 0; i < space.springs.size(); i++)
    {
        const Spring &spring = space.springs[i];
        if (spring.shapeIndex == selectedShapeIndex)
        {
            Spring newSpring = spring;
            newSpring.pointA -= shape.start;
            newSpring.pointB -= shape.start;
            tempSpace.springs.push(newSpring);
        }
    }

    dumpToFile(tempSpace, filename.data);

    const char *tempFileName = "/tmp/physics_space_copy.txt";

    FILE *f = fopen(tempFileName, "w");

    fprintf(f, "    {\"New Prefab %d\", [](Game *game)\n", saveNumber);
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

void PhysicsSpaceStorage::dumpToUnitTest(PhysicsSpace &space, float scale, const Vector2 &offset)
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
    fprintf(f, "         game->scale() = %.2ff;\n", scale);
    fprintf(f, "         game->offset() = Vector2(%.2ff, %.2ff);\n", offset.x, offset.y);
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