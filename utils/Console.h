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
    double collisionHandlingTimeMillis;
    double collisionGridUpdateTimeMillis;
    int numBboxes;
    int numBbboxChecks;
    int numBboxOverlaps;
    int numCircleRejections;
    int numIntersections;
    int numCollisions;
    double rawFrameTimeMillis;
    double displayedFrameTimeMillis;
    double displayedFps;
    double targetFrameTimeMillis;
    double targetTickRate;
    bool frameTimeSnappingEnabled;
};

class Vector2;

struct ConsoleState;
struct ImFont;
struct ShapeBoundingBox;

extern "C" ConsoleState *allocConsoleState();
extern "C" void freeConsoleState(ConsoleState *state);

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
    // The log as a floating panel at the given position and size, in screen pixels
    static void drawWindow(float x, float y, float width, float height, ImFont *boldFont);

    // Collision lines, labels and other debug geometry drawn on top of the simulation
    static void drawDebugGeometry(float scale, const Vector2 &offset);

    static void drawPoint(const Vector2 &point, unsigned int color);
    static void drawBoundingBox(const ShapeBoundingBox &box, unsigned int color);
    static void drawQuad(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, const Vector2 &p3, unsigned int color);
    static void drawSegment(const Vector2 &v0, const Vector2 &v1, unsigned int color);
    static void drawVelocityVector(const Vector2 &position, const Vector2 &vector, unsigned int color);
    static void drawVector(const Vector2 &position, const Vector2 &vector, unsigned int color);

    static void printToStandardOut();
};

#endif
