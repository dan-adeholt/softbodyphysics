#ifndef __PHYSICS_SPACE_STORAGE_H
#define __PHYSICS_SPACE_STORAGE_H

struct PhysicsSpace;

struct PhysicsSpaceStorage
{
    static void loadFromFile(PhysicsSpace &space, const char *filename);

    static void dumpToFile(PhysicsSpace &space, const char *filename);
};

#endif