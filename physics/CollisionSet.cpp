#include "CollisionSet.h"

// Implementation of CollisionSet methods

CollisionSet::CollisionSet()
    : numberOfShapes(0), totalPairs(0), numWords(0)
{
    // collisionBits is initialized empty
}

CollisionSet::~CollisionSet()
{
    // The Array class handles its own cleanup
}

void CollisionSet::init(int numShapes)
{
    numberOfShapes = numShapes;

    // Recalculate total unique pairs and number of words
    totalPairs = (numberOfShapes * (numberOfShapes - 1)) / 2;
    numWords = (totalPairs + BITS_PER_WORD - 1) / BITS_PER_WORD;

    // Resize collisionBits and fill with zeros
    collisionBits.reserve(numWords);
    collisionBits.fill(0, numWords);
}

int CollisionSet::getPairIndex(int i, int j) const
{
    if (i == j)
        return -1; // No self-collision

    // Ensure i < j to exploit symmetry
    if (i > j)
    {
        int temp = i;
        i = j;
        j = temp;
    }

    // Calculate the index using triangular numbers
    int index = i * (2 * numberOfShapes - i - 1) / 2 + (j - i - 1);
    return index;
}

void CollisionSet::setCollision(int i, int j)
{
    int index = getPairIndex(i, j);
    if (index < 0)
        return; // Invalid index

    int wordIndex = index / BITS_PER_WORD;
    int bitIndex = index % BITS_PER_WORD;

    collisionBits[wordIndex] |= (1U << bitIndex);
}

bool CollisionSet::hasCollision(int i, int j) const
{
    int index = getPairIndex(i, j);
    if (index < 0)
        return false; // Invalid index

    int wordIndex = index / BITS_PER_WORD;
    int bitIndex = index % BITS_PER_WORD;

    return (collisionBits[wordIndex] & (1U << bitIndex)) != 0;
}

void CollisionSet::clearCollision(int i, int j)
{
    int index = getPairIndex(i, j);
    if (index < 0)
        return; // Invalid index

    int wordIndex = index / BITS_PER_WORD;
    int bitIndex = index % BITS_PER_WORD;

    collisionBits[wordIndex] &= ~(1U << bitIndex);
}
