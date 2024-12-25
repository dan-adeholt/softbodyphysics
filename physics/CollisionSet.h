#ifndef __PHYSICS__COLLISION_SET_h
#define __PHYSICS__COLLISION_SET_h

#include "../containers/Array.h"

// CollisionSet class
class CollisionSet
{
public:
    // Constructor
    CollisionSet();

    // Destructor
    ~CollisionSet();

    // Initialize with a new number of shapes
    void init(int numShapes);

    // Methods to set, check, and clear collisions
    void setCollision(int i, int j);
    bool hasCollision(int i, int j) const;
    void clearCollision(int i, int j);

private:
    int numberOfShapes; // Number of shapes
    int totalPairs;     // Total number of unique pairs
    int numWords;       // Number of words in collisionBits

    Array<unsigned int> collisionBits; // Bitmask array

    // Helper function to map (i, j) to bit index
    int getPairIndex(int i, int j) const;

    // Constants
    static const int BITS_PER_WORD = sizeof(unsigned int) * 8;
};

#endif