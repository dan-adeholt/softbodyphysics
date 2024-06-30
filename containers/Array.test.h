#ifndef __ARRAY_TEST_H
#define __ARRAY_TEST_H

#include <stdio.h>
#include "./Array.h"
#include "../utils/Console.h"
#include "../utils/UnitTestUtil.h"

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
    testAssert(numConstructed == 0);
    testAssert(numDestructed == 0);

    array.push(ArraySentinel());
    testAssert(numConstructed == 2);
    testAssert(numDestructed == 1);
    testAssert(array.capacity() == 2);
    testAssert(array.size() == 1);
    array.push(ArraySentinel());
    testAssert(array.capacity() == 2);
    testAssert(array.size() == 2);
    array.push(ArraySentinel());
    testAssert(array.capacity() > 2);
    testAssert(array.size() == 3);
}

void testSentinels()
{
    testSentinelsInner();
    testAssert(numConstructed == numDestructed);
}

void testInts()
{
    Array<int> ints;
    ints.push(0);

    testAssert(ints.capacity() == 2);
    testAssert(ints.size() == 1);
    testAssert(ints[0] == 0);
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
        testAssert(ints[i] == i);
    }
}

void testArrays()
{
    testSentinels();
    testInts();
}

#endif