
#ifndef __PHYSICS__TEST_H
#define __PHYSICS__TEST_H

class Game;

typedef void (*PhysicsTestFunction)(Game *);
typedef const char *(*PhysicsInvariantFunction)(Game *, int);
typedef void (*PhysicsTimeFunction)(Game *, int);

class PhysicsTestDefinition
{
public:
    PhysicsTestDefinition(const char *name,
                          int duration,
                          PhysicsTestFunction initFunc,
                          PhysicsInvariantFunction invariantFunction,
                          PhysicsTimeFunction actionFunction);

    void start(Game *game);
    void step(Game *game, double elapsedMilliseconds);
    void end(Game *game);

    const char *name;
    const char *invariantResult;
    PhysicsTestFunction initFunc;
    PhysicsInvariantFunction invariantFunction;
    PhysicsTimeFunction actionFunction;
    int duration;
    int time;

    static int numTests;
    static PhysicsTestDefinition *allTests;
};

#endif