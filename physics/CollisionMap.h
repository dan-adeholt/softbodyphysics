#ifndef __PHYSICS__COLLISION_MAP_h
#define __PHYSICS__COLLISION_MAP_h

#include "../containers/Array.h"

struct CollisionMap
{
    Array<int> data;
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

    void assign(const CollisionMap &other)
    {
        data.fill(0, CollisionMap::arraySize(other.numElements));

        for (int i = 0; i < other.numElements; i++)
        {
            data[i] = other.data[i];
        }

        numElements = other.numElements;
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
        ++data[index];
    }

    int getCollisionCount(int i, int j) const
    {
        return data[calculateIndex(i, j)];
    }
};

#endif