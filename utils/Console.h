#ifndef __DEBUG_CONSOLE_H
#define __DEBUG_CONSOLE_H

class Game;

struct ConsoleProfileInfo
{
    double physicsTimeMillis;
    double elapsedStepTimeMillis;
    int numPhysicsSteps;
    double totalPhysicsTimeMillis;
    double renderTimeMillis;
    double swapTimeMillis;
    double springsTimeMillis;
    double collisionTimeMillis;
    int numSprings;
};

class Console
{
public:
    static void clear();
    static void clearFrame();
    static void log(const char *format, ...);
    static void logFrame(float x, float y, const char *format, ...);
    static void logVectorFrame(float x, float y, float vx, float vy, const char *format, ...);
    static void draw(Game &game, ConsoleProfileInfo profileInfo);

    static bool executingTest();
    static void stepTest(Game *game, double elapsedTimeMillis, ConsoleProfileInfo &profileInfo);
};

#endif