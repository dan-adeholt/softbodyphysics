#include <cstdio>
#include "containers/Array.test.h"
#include "game/Physics.test.h"
#include "utils/Console.h"
#include "utils/UnitTestUtil.h"

int main()
{
    printf("Test\n");

    UnitTestUtil::runTests();
    Console::printToStandardOut();

    return 0;
}
