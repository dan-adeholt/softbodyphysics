#ifndef __ARRAY_TEST_H
#define __ARRAY_TEST_H

#include <stdio.h>
#include <assert.h>
#include "./Array.h"

int numConstructed = 0;
int numDestructed = 0;

class ArraySentinel
{
public:
    ArraySentinel()
    {
        numConstructed++;
    }

    ~ArraySentinel()
    {
        numDestructed++;
    }

    ArraySentinel(const ArraySentinel &other)
    {
        numConstructed++;
    }
};

void testSentinelsInner()
{
    Array<ArraySentinel> array(2);

    // Capacity is 10, but no items should have been constructed.
    assert(numConstructed == 0);
    assert(numDestructed == 0);

    array.push(ArraySentinel());
    assert(numConstructed == 2);
    assert(numDestructed == 1);
    assert(array.capacity() == 2);
    assert(array.size() == 1);
    array.push(ArraySentinel());
    assert(array.capacity() == 2);
    assert(array.size() == 2);
    array.push(ArraySentinel());
    assert(array.capacity() > 2);
    assert(array.size() == 3);
}

void testSentinels()
{
    testSentinelsInner();
    assert(numConstructed == numDestructed);
}

void testInts()
{
    Array<int> ints;
    ints.push(0);

    assert(ints.capacity() == 2);
    assert(ints.size() == 1);
    assert(ints[0] == 0);
    ints.push(1);
    ints.push(2);
    ints.push(3);
    ints.push(4);
    ints.push(5);
    ints.push(6);
    ints.push(7);
    ints.push(8);

    for (int i = 0; i < ints.size(); i++)
    {
        assert(ints[i] == i);
    }

    printf("%d %d\n", ints.size(), ints.capacity());
}

void testArrays()
{
    printf("Testing\n");
    testSentinels();
    testInts();
}

#endif