#include "Console.h"
#include "../game/Scenes.h"
#include "../physics/Physics.h"
#include "../containers/Range.h"
#include "../game/Game.h"
#include "imgui.h"
#include <cstdarg>
#include <cstdio>

#define MAX_LINE_LENGTH 1000

typedef enum DebugGeometry
{
    DebugGeometryNone,
    DebugGeometryPoint,
    DebugGeometrySegment,
    DebugGeometryQuad,
    DebugGeometryVector
} DebugGeometry;

struct DebugGeometryEntry
{
    DebugGeometry geometry;
    Vector2 v0;
    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
    int r, g, b;
};

struct LineEntry
{
    char text[MAX_LINE_LENGTH];
    int r, g, b;
    int writeIndex;
};

struct PositionLineEntry
{
    char text[MAX_LINE_LENGTH];
    float x, y;
    float vx, vy;
    int r, g, b;
};

struct CollisionIntersectionEntry
{
    Vector2 v0;
    Vector2 v1;
    char text[MAX_LINE_LENGTH];
};

struct CollisionImpulseEntry
{
    Vector2 impulse;
    Vector2 point;
};

#define BUFFER_SIZE 100000
DebugGeometryEntry debugGeometry[BUFFER_SIZE] = {};
CollisionIntersectionEntry collisionIntersections[BUFFER_SIZE] = {};
LineEntry lines[BUFFER_SIZE] = {};
PositionLineEntry positionLines[BUFFER_SIZE] = {};
CollisionImpulseEntry collisionImpulses[BUFFER_SIZE] = {};

int numCollisionIntersections = 0;
int numPosLines = 0;
int numLines = 0;
int numCollisionImpulses = 0;
int numDebugGeometry = 0;

bool debuggerState = false;

void Console::setDebugger(bool debugger)
{
    debuggerState = debugger;
}

bool Console::isDebugger()
{
    return debuggerState;
}

void Console::logCollisionImpulse(const Vector2 &impulse, const Vector2 &point)
{
    if (numCollisionImpulses >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in collision impulses buffer");
        return;
    }

    collisionImpulses[numCollisionImpulses].impulse = impulse;
    collisionImpulses[numCollisionImpulses].point = point;
    numCollisionImpulses++;
}

void Console::clear()
{
    numLines = 0;
}

void Console::clearFrame()
{
    numPosLines = 0;
    numDebugGeometry = 0;
}

void Console::log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int writeIndex = numLines % BUFFER_SIZE;
    vsnprintf(lines[writeIndex].text, MAX_LINE_LENGTH, format, args);
    va_end(args);

    lines[writeIndex].r = 0;
    lines[writeIndex].g = 0;
    lines[writeIndex].b = 0;
    lines[writeIndex].writeIndex = numLines;
    numLines++;
}

void Console::logFrame(float x, float y, const char *format, ...)
{
    if (numPosLines >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in position lines buffer");
        numPosLines = 0;
        // return;
    }

    va_list args;
    va_start(args, format);

    vsnprintf(positionLines[numPosLines].text, MAX_LINE_LENGTH, format, args);
    va_end(args);

    positionLines[numPosLines].x = x;
    positionLines[numPosLines].y = y;
    positionLines[numPosLines].r = 0;
    positionLines[numPosLines].g = 0;
    positionLines[numPosLines].b = 0;
    positionLines[numPosLines].vx = 0;
    positionLines[numPosLines].vy = 0;
    numPosLines++;
}

void Console::logVectorFrame(float x, float y, float vx, float vy, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Console::logFrame(x, y, format, args);
    va_end(args);
    positionLines[numPosLines - 1].vx = vx;
    positionLines[numPosLines - 1].vy = vy;
}

int selectedSceneIndex = -1;
bool showingConsole = true;

void Console::clearCollisionFrame()
{
    numCollisionIntersections = 0;
}

void Console::logCollisionIntersectionTest(const Vector2 &v0, const Vector2 &v1, const char *format, ...)
{
    if (numCollisionIntersections >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in collision intersections buffer");
        return;
    }

    va_list args;
    va_start(args, format);

    vsnprintf(collisionIntersections[numCollisionIntersections].text, MAX_LINE_LENGTH, format, args);
    va_end(args);

    collisionIntersections[numCollisionIntersections].v0 = v0;
    collisionIntersections[numCollisionIntersections].v1 = v1;
    numCollisionIntersections++;
}

void Console::draw(ConsoleProfileInfo profileInfo, float scale, const Vector2 &offset)
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;

    ImGui::SetNextWindowSizeConstraints(ImVec2(displaySize.x, 200), ImVec2(displaySize.x, 200));
    ImGui::SetNextWindowPos(ImVec2(displaySize.x, displaySize.y - 200), ImGuiCond_Always, ImVec2(1, 0));
    ImGui::Begin("Console");
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;
    ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_None,
                      window_flags);

    if (ImGui::Button("Clear"))
    {
        clear();
    }

    int startIndex = numLines >= BUFFER_SIZE ? numLines % BUFFER_SIZE : 0;
    int numLinesToDraw = numLines < BUFFER_SIZE ? numLines : BUFFER_SIZE;
    for (int i = 0; i < numLinesToDraw; i++)
    {
        int index = (startIndex + i) % BUFFER_SIZE;
        ImGui::TextColored(ImVec4(lines[index].r / 255.0f, lines[index].g / 255.0f, lines[index].b / 255.0f, 1.0f), "[%d] %s", lines[index].writeIndex, lines[index].text);
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    ImGui::End();

    ImDrawList *foreground = ImGui::GetForegroundDrawList();

    for (int i = 0; i < numCollisionIntersections; i++)
    {
        CollisionIntersectionEntry &entry = collisionIntersections[i];

        // // Scale coordinates by scale and translate by offset
        // m->vertices[i].pos.x = m->vertices[i].pos.x * scale + offset.x;
        // m->vertices[i].pos.y = m->vertices[i].pos.y * scale + offset.y;

        Vector2 v0 = entry.v0 * scale + offset;
        Vector2 v1 = entry.v1 * scale + offset;

        foreground->AddLine(ImVec2(v0.x, v0.y), ImVec2(v1.x, v1.y), IM_COL32(255, 0, 0, 255), 1.0f);
        foreground->AddText(ImVec2(v0.x, v0.y), IM_COL32(0, 0, 0, 255), entry.text);
    }

    for (int i = 0; i < numPosLines; i++)
    {
        PositionLineEntry &entry = positionLines[i];
        Vector2 entryPos = Vector2(entry.x, entry.y) * scale + offset;

        foreground->AddText(ImVec2(entryPos.x, entryPos.y), IM_COL32(entry.r, entry.g, entry.b, 255), entry.text);
        if (entry.vx != 0 || entry.vy != 0)
        {
            Vector2 velocity = Vector2(entry.vx, entry.vy) * scale;
            foreground->AddLine(ImVec2(entryPos.x, entryPos.y), ImVec2(entryPos.x + velocity.x, entryPos.y + velocity.y), IM_COL32(255, 0, 0, 255), 1.0f);
        }
    }

    for (int i = 0; i < numCollisionImpulses; i++)
    {
        CollisionImpulseEntry &entry = collisionImpulses[i];
        Vector2 entryPoint = entry.point * scale + offset;
        foreground->AddLine(ImVec2(entryPoint.x, entryPoint.y), ImVec2(entryPoint.x + entry.impulse.x * scale, entryPoint.y + entry.impulse.y * scale), IM_COL32(255, 0, 0, 255), 1.0f);
    }

    for (int i = 0; i < numDebugGeometry; i++)
    {
        DebugGeometryEntry &entry = debugGeometry[i];
        Vector2 v0 = entry.v0 * scale + offset;
        Vector2 v1 = entry.v1 * scale + offset;
        Vector2 v2 = entry.v2 * scale + offset;
        Vector2 v3 = entry.v3 * scale + offset;

        switch (entry.geometry)
        {
        case DebugGeometryPoint:
            foreground->AddCircleFilled(ImVec2(v0.x, v0.y), 4.0f, IM_COL32(entry.r, entry.g, entry.b, 255), 12);
            break;
        case DebugGeometrySegment:
            foreground->AddLine(ImVec2(v0.x, v0.y), ImVec2(v1.x, v1.y), IM_COL32(entry.r, entry.g, entry.b, 255), 1.0f);
            break;
        case DebugGeometryQuad:
            foreground->AddLine(ImVec2(v0.x, v0.y), ImVec2(v1.x, v1.y), IM_COL32(entry.r, entry.g, entry.b, 255), 1.0f);
            foreground->AddLine(ImVec2(v1.x, v1.y), ImVec2(v2.x, v2.y), IM_COL32(entry.r, entry.g, entry.b, 255), 1.0f);
            foreground->AddLine(ImVec2(v2.x, v2.y), ImVec2(v3.x, v3.y), IM_COL32(entry.r, entry.g, entry.b, 255), 1.0f);
            foreground->AddLine(ImVec2(v3.x, v3.y), ImVec2(v0.x, v0.y), IM_COL32(entry.r, entry.g, entry.b, 255), 1.0f);
            break;
        case DebugGeometryVector:
        {

            Vector2 normalizedVec = (v1 - v0).normalized();
            Vector2 arrowHead0 = v1 - normalizedVec.rotate(0.8f) * 10.0f;
            Vector2 arrowHead1 = v1 - normalizedVec.rotate(-0.8f) * 10.0f;
            foreground->AddLine(ImVec2(v0.x, v0.y), ImVec2(v1.x, v1.y), IM_COL32(entry.r, entry.g, entry.b, 255), 1.0f);
            foreground->AddLine(ImVec2(v1.x, v1.y), ImVec2(arrowHead0.x, arrowHead0.y), IM_COL32(entry.r, entry.g, entry.b, 255), 1.0f);
            foreground->AddLine(ImVec2(v1.x, v1.y), ImVec2(arrowHead1.x, arrowHead1.y), IM_COL32(entry.r, entry.g, entry.b, 255), 1.0f);
        }
        break;
        default:
            break;
        }
    }
}

void Console::addDebugPoint(const Vector2 &point, unsigned int color)
{
    if (numDebugGeometry >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in debug geometry buffer");
        return;
    }

    debugGeometry[numDebugGeometry].geometry = DebugGeometryPoint;
    debugGeometry[numDebugGeometry].v0 = point;
    debugGeometry[numDebugGeometry].r = (color >> 16) & 0xFF;
    debugGeometry[numDebugGeometry].g = (color >> 8) & 0xFF;
    debugGeometry[numDebugGeometry].b = color & 0xFF;
    numDebugGeometry++;
}

void Console::addDebugQuad(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, const Vector2 &p3, unsigned int color)
{

    if (numDebugGeometry >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in debug geometry buffer");
        return;
    }

    debugGeometry[numDebugGeometry].geometry = DebugGeometryQuad;
    debugGeometry[numDebugGeometry].v0 = p0;
    debugGeometry[numDebugGeometry].v1 = p1;
    debugGeometry[numDebugGeometry].v2 = p2;
    debugGeometry[numDebugGeometry].v3 = p3;
    debugGeometry[numDebugGeometry].r = (color >> 16) & 0xFF;
    debugGeometry[numDebugGeometry].g = (color >> 8) & 0xFF;
    debugGeometry[numDebugGeometry].b = color & 0xFF;
    numDebugGeometry++;
}

void Console::addDebugSegment(const Vector2 &v0, const Vector2 &v1, unsigned int color)
{

    if (numDebugGeometry >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in debug geometry buffer");
        return;
    }

    debugGeometry[numDebugGeometry].geometry = DebugGeometrySegment;
    debugGeometry[numDebugGeometry].v0 = v0;
    debugGeometry[numDebugGeometry].v1 = v1;
    debugGeometry[numDebugGeometry].r = (color >> 16) & 0xFF;
    debugGeometry[numDebugGeometry].g = (color >> 8) & 0xFF;
    debugGeometry[numDebugGeometry].b = color & 0xFF;
    numDebugGeometry++;
}

void Console::addDebugVelocityVector(const Vector2 &position, const Vector2 &vector, unsigned int color)
{
    addDebugVector(position, vector * 10000.0f, color);
}

void Console::addDebugVector(const Vector2 &position, const Vector2 &vector, unsigned int color)
{
    if (numDebugGeometry >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in debug geometry buffer");
        return;
    }

    debugGeometry[numDebugGeometry].geometry = DebugGeometryVector;
    debugGeometry[numDebugGeometry].v0 = position;
    debugGeometry[numDebugGeometry].v1 = position + vector;
    debugGeometry[numDebugGeometry].r = (color >> 16) & 0xFF;
    debugGeometry[numDebugGeometry].g = (color >> 8) & 0xFF;
    debugGeometry[numDebugGeometry].b = color & 0xFF;
    numDebugGeometry++;
}

void Console::printToStandardOut()
{
    for (int i = 0; i < numLines; i++)
    {
        int index = i % BUFFER_SIZE;
        printf("[%d] %s\n", lines[index].writeIndex, lines[index].text);
    }
}
