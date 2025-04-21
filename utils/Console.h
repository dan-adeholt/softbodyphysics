#ifndef __DEBUG_CONSOLE_H
#define __DEBUG_CONSOLE_H

class Game;

struct ConsoleProfileInfo
{
    double slowdownFactor;
    double physicsTimeMillis;
    double elapsedStepTimeMillis;
    int numPhysicsSteps;
    double totalPhysicsTimeMillis;
    double renderTimeMillis;
    double swapTimeMillis;
    double springsTimeMillis;
    double collisionTimeMillis;
    double boundingBoxTimeMillis;
    int numBboxes;
    int numBbboxChecks;
    int numBboxOverlaps;
    int numCollisions;
};

class Vector2;

struct ConsoleState;
struct ImFont;
struct ShapeBoundingBox;

extern "C" ConsoleState *allocConsoleState();

class Console
{
public:
    static void setConsoleState(ConsoleState *state);
    static void setDebugger(bool debug);
    static bool isDebugger();

    static void logCollisionImpulse(const Vector2 &impulse, const Vector2 &point);
    static void checkLogFile();
    static void clear();
    static void clearFrame();
    static void log(const char *format, ...);
    static void logFrame(float x, float y, const char *format, ...);
    static void logVectorFrame(float x, float y, float vx, float vy, const char *format, ...);

    static void clearCollisionFrame();
    static void logCollisionIntersectionTest(const Vector2 &v0, const Vector2 &v1, const char *format, ...);
    static void draw(ConsoleProfileInfo profileInfo, float scale, const Vector2 &offset, ImFont *boldFont);

    static void drawPoint(const Vector2 &point, unsigned int color);
    static void drawBoundingBox(const ShapeBoundingBox &box, unsigned int color);
    static void drawQuad(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, const Vector2 &p3, unsigned int color);
    static void drawSegment(const Vector2 &v0, const Vector2 &v1, unsigned int color);
    static void drawVelocityVector(const Vector2 &position, const Vector2 &vector, unsigned int color);
    static void drawVector(const Vector2 &position, const Vector2 &vector, unsigned int color);

    static bool executingTest();
    static void stepTest(Game *game, double elapsedTimeMillis, ConsoleProfileInfo &profileInfo);

    static void printToStandardOut();
};

#endif