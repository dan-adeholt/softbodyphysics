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

    // Capacity is reserved, but no items should have been constructed.
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

UNIT_TEST(ArrayTestSentinels)
{
    testSentinelsInner();
    testAssert(numConstructed == numDestructed);
}

UNIT_TEST(ArrayTestInts)
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


UNIT_TEST(ArrayTestCopyConstruct)
{
    Array<int> source;
    source.reserve(16);
    source.push(1);
    source.push(2);

    // The copy only allocates size() elements, so capacity must report that
    // and not the source's larger capacity.
    Array<int> copy(source);
    testAssert(copy.size() == 2);
    testAssert(copy.capacity() == 2);

    // Must grow instead of writing past the smaller allocation.
    copy.push(3);
    testAssert(copy.size() == 3);
    testAssert(copy[0] == 1);
    testAssert(copy[1] == 2);
    testAssert(copy[2] == 3);
}

UNIT_TEST(ArrayTestReplace)
{
    Array<int> dest;
    for (int i = 0; i < 5; i++)
    {
        dest.push(i);
    }

    Array<int> smaller;
    smaller.push(100);
    smaller.push(101);

    dest.replace(smaller);
    testAssert(dest.size() == 2);
    testAssert(dest[0] == 100);
    testAssert(dest[1] == 101);

    Array<int> larger;
    for (int i = 0; i < 9; i++)
    {
        larger.push(i * 2);
    }

    dest.replace(larger);
    testAssert(dest.size() == 9);
    for (int i = 0; i < dest.size(); i++)
    {
        testAssert(dest[i] == i * 2);
    }

    dest.replace(larger.range(2, 5));
    testAssert(dest.size() == 3);
    testAssert(dest[0] == 4);
    testAssert(dest[2] == 8);
}

void testReplaceDestructsInner()
{
    Array<ArraySentinel> dest;
    Array<ArraySentinel> source;

    for (int i = 0; i < 3; i++)
    {
        dest.push(ArraySentinel());
    }

    for (int i = 0; i < 2; i++)
    {
        source.push(ArraySentinel());
    }

    // replace() must destruct the three elements it overwrites, not the
    // uninitialised slots that follow them.
    dest.replace(source);
    testAssert(dest.size() == 2);
}

UNIT_TEST(ArrayTestReplaceDestructs)
{
    numConstructed = 0;
    numDestructed = 0;
    testReplaceDestructsInner();
    testAssert(numConstructed == numDestructed);
}

UNIT_TEST(ArrayTestFilter)
{
    Array<int> ints;
    for (int i = 0; i < 10; i++)
    {
        ints.push(i);
    }

    int numRemoved = ints.filter([](const int &value)
                                 { return value % 2 == 0; });

    testAssert(numRemoved == 5);
    testAssert(ints.size() == 5);
    for (int i = 0; i < ints.size(); i++)
    {
        testAssert(ints[i] == i * 2 + 1);
    }
}

UNIT_TEST(ArrayTestClearAndFree)
{
    Array<int> ints;
    ints.push(1);
    ints.push(2);

    ints.clearAndFree();
    testAssert(ints.size() == 0);
    testAssert(ints.capacity() == 0);

    // clearAndFree() must be idempotent. If it leaves m_data dangling the
    // second call frees the same buffer twice.
    ints.clearAndFree();
    testAssert(ints.size() == 0);
    testAssert(ints.capacity() == 0);

    // Reusing the array after an explicit clearAndFree() must reallocate, and
    // the destructor must not free the released buffer a second time.
    ints.push(3);
    testAssert(ints.size() == 1);
    testAssert(ints[0] == 3);
}

UNIT_TEST(ArrayTestRemoveUnordered)
{
    Array<int> ints;

    for (int i = 0; i < 5; i++)
    {
        ints.push(i);
    }

    // The last element fills the gap
    ints.removeUnordered(1);
    testAssert(ints.size() == 4);
    testAssert(ints[0] == 0 && ints[1] == 4 && ints[2] == 2 && ints[3] == 3);

    // Removing the last element just drops it
    ints.removeUnordered(3);
    testAssert(ints.size() == 3);
    testAssert(ints[0] == 0 && ints[1] == 4 && ints[2] == 2);
}

#endif