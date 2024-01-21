#include "containers/Array.test.h"
#include "game/Physics.test.h"
#include <stdio.h>

int main()
{
    testArrays();
    testCollisions();
    testSprings();
    printf("All tests done\n");

    return 0;
}
