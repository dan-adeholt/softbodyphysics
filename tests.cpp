#include <stdio.h>
#include "containers/Array.test.h"
#include "physics/Physics.test.h"
#include "utils/Console.h"
#include "utils/UnitTestUtil.h"

int main(int argc, const char **argv)
{
    if (argc < 3)
    {
        printf("Usage: %s --test <test_name>\n", argv[0]);
        return 1;
    }

    UnitTestUtil::runTest(argv[2]);
    Console::printToStandardOut();

    if (UnitTestUtil::getTestFailed())
    {
        return 1;
    }

    return 0;
}
