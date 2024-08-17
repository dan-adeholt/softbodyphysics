#include "PhysicsSpaceStorage.h"
#include "Physics.h"
#include <stdio.h>

void PhysicsSpaceStorage::loadFromFile(PhysicsSpace &space, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file for reading");
        return;
    }

    int numPoints, numShapes, numSprings;
    // Read the number of points and shapes
    fscanf(file, "%d %d %d\n", &numPoints, &numShapes, &numSprings);

    space.shapes.clear();
    space.shapes.reserve(numShapes);
    space.points.clear();
    space.points.reserve(numPoints);

    // Read points
    for (int i = 0; i < numPoints; ++i)
    {
        float x, y;
        float velocityX, velocityY;
        float mass;

        fscanf(file, "%f %f %f %f %f\n", &mass, &x, &y, &velocityX, &velocityY);
        space.points.pos.push({x, y});
        space.points.velocity.push({velocityX, velocityY});
        space.points.mass.push(mass);
        space.points.acceleration.push({0.0f, 0.0f});
    }

    // Read shapes
    for (int i = 0; i < numShapes; ++i)
    {
        Shape shape;
        fscanf(file, "%d %d\n", &shape.start, &shape.end);
        space.shapes.push(shape);
    }

    // Read springs
    for (int i = 0; i < numSprings; ++i)
    {
        Spring spring;
        fscanf(file, "%d %d %f %f %f %d\n", &spring.pointA, &spring.pointB, &spring.length, &spring.stiffness, &spring.damping, &spring.shapeIndex);
        space.springs.push(spring);
    }

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

    // Dump the number of points and shapes first
    fprintf(file, "%d %d %d\n", space.points.size(), space.shapes.size(), space.springs.size());

    // Dump points
    for (int i = 0; i < space.points.size(); ++i)
    {
        Vector2 pos = space.points.pos[i];

        Vector2 velocity = space.points.velocity[i];
        float mass = space.points.mass[i];

        fprintf(file, "%f %f %f %f %f\n", mass, pos.x, pos.y, velocity.x, velocity.y);
    }

    // Dump shapes
    for (int i = 0; i < space.shapes.size(); ++i)
    {
        fprintf(file, "%d %d\n", space.shapes[i].start, space.shapes[i].end);
    }

    // Dump springs
    for (int i = 0; i < space.springs.size(); ++i)
    {
        fprintf(file, "%d %d %f %f %f %d\n", space.springs[i].pointA, space.springs[i].pointB, space.springs[i].length, space.springs[i].stiffness, space.springs[i].damping, space.springs[i].shapeIndex);
    }

    fclose(file);
}
