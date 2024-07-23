#include <stdio.h>
#include "game/Game.h"
#include "timer.h"

int main()
{
    Game game;
    game.init("Circle grid");
    Timer timer;
    game.testRK4Performance(10000);
    double elapsed = timer.elapsedMillis();
    printf("Total time: %.lf ms\n", elapsed);
    timer.reset();
    game.testSpringPerformance(10000);
    elapsed = timer.elapsedMillis();
    printf("Spring time: %.lf ms\n", elapsed);
}