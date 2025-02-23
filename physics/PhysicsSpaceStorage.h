#ifndef __PHYSICS_SPACE_STORAGE_H
#define __PHYSICS_SPACE_STORAGE_H

struct PhysicsSpace;
class Vector2;

struct PhysicsSpaceStorage
{
    static void loadFromFile(PhysicsSpace &space, const char *filename);
    static void appendFromFile(PhysicsSpace &space, const char *filename, const Vector2 &posOffset);

    static void dumpToFile(PhysicsSpace &space, const char *filename);

    static void dumpToUnitTest(PhysicsSpace &space, float scale, const Vector2 &offset);

    static void dumpToPrefab(PhysicsSpace &space, int selectedShapeIndex);
};

#endif