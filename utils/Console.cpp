#include "Console.h"
#include "../game/Scenes.h"
#include "../physics/Physics.h"
#include "../physics/CollisionSolver.h"
#include "../containers/Range.h"
#include "../game/Game.h"
#include "imgui.h"
#include <cstdarg>
#include <cstdio>

const unsigned char BlackComponent = BlackComponent;

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
    bool bold;
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

struct ConsoleState
{
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
    FILE *logFilePointer = nullptr;
    long lastLogFilePos = 0;
    char lineBuffer[MAX_LINE_LENGTH];
    int lineBufferIndex = 0;
};

ConsoleState *state = nullptr;

ConsoleState *allocConsoleState()
{
    return new ConsoleState();
}

void freeConsoleState(ConsoleState *consoleState)
{
    delete consoleState;
}

void Console::setConsoleState(ConsoleState *newState)
{
    state = newState;
}

void Console::setDebugger(bool debugger)
{
    state->debuggerState = debugger;
}

bool Console::isDebugger()
{
    return state->debuggerState;
}

void Console::logCollisionImpulse(const Vector2 &impulse, const Vector2 &point)
{
    if (state->numCollisionImpulses >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in collision impulses buffer");
        return;
    }

    state->collisionImpulses[state->numCollisionImpulses].impulse = impulse;
    state->collisionImpulses[state->numCollisionImpulses].point = point;
    state->numCollisionImpulses++;
}

void Console::checkLogFile()
{
    if (state->logFilePointer == nullptr)
    {
        state->logFilePointer = fopen("build-output.log", "r");
        if (state->logFilePointer != nullptr)
        {
            fseek(state->logFilePointer, 0, SEEK_END);
            state->lastLogFilePos = ftell(state->logFilePointer);
        }
    }

    if (state->logFilePointer != nullptr)
    {
        clearerr(state->logFilePointer);

        // Move file pointer to where we left off
        fseek(state->logFilePointer, state->lastLogFilePos, SEEK_SET);

        char buffer[1000];
        while (fgets(buffer, 1000, state->logFilePointer) != nullptr)
        {
            for (int i = 0; i < 1000 && buffer[i] != '\0'; i++)
            {
                if (state->lineBufferIndex == MAX_LINE_LENGTH - 2 || buffer[i] == '\n')
                {
                    state->lineBuffer[state->lineBufferIndex] = '\0';
                    Console::log("> %s", state->lineBuffer);
                    state->lineBufferIndex = 0;

                    if (buffer[i] == '\n')
                    {
                        continue;
                    }
                }

                state->lineBuffer[state->lineBufferIndex++] = buffer[i];
            }
        }

        state->lastLogFilePos = ftell(state->logFilePointer);
    }
}

void Console::clear()
{
    state->numLines = 0;
}

void Console::clearFrame()
{
    state->numPosLines = 0;
    state->numDebugGeometry = 0;
}

void Console::log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int writeIndex = state->numLines % BUFFER_SIZE;
    vsnprintf(state->lines[writeIndex].text, MAX_LINE_LENGTH, format, args);
    va_end(args);

    if (strstr(state->lines[writeIndex].text, "error: "))
    {
        state->lines[writeIndex].r = 255;
        state->lines[writeIndex].bold = true;
    }
    else
    {
        state->lines[writeIndex].r = BlackComponent;
        state->lines[writeIndex].bold = false;
    }

    state->lines[writeIndex].g = BlackComponent;
    state->lines[writeIndex].b = BlackComponent;
    state->lines[writeIndex].writeIndex = state->numLines;
    state->numLines++;
}

void Console::logFrame(float x, float y, const char *format, ...)
{
    if (state->numPosLines >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in position lines buffer");
        state->numPosLines = 0;
        // return;
    }

    va_list args;
    va_start(args, format);

    vsnprintf(state->positionLines[state->numPosLines].text, MAX_LINE_LENGTH, format, args);
    va_end(args);

    state->positionLines[state->numPosLines].x = x;
    state->positionLines[state->numPosLines].y = y;
    state->positionLines[state->numPosLines].r = BlackComponent;
    state->positionLines[state->numPosLines].g = BlackComponent;
    state->positionLines[state->numPosLines].b = BlackComponent;
    state->positionLines[state->numPosLines].vx = 0;
    state->positionLines[state->numPosLines].vy = 0;
    state->numPosLines++;
}

void Console::logVectorFrame(float x, float y, float vx, float vy, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Console::logFrame(x, y, format, args);
    va_end(args);
    state->positionLines[state->numPosLines - 1].vx = vx;
    state->positionLines[state->numPosLines - 1].vy = vy;
}

int selectedSceneIndex = -1;
bool showingConsole = true;

void Console::clearCollisionFrame()
{
    state->numCollisionIntersections = 0;
}

void Console::logCollisionIntersectionTest(const Vector2 &v0, const Vector2 &v1, const char *format, ...)
{
    if (state->numCollisionIntersections >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in collision intersections buffer");
        return;
    }

    va_list args;
    va_start(args, format);

    vsnprintf(state->collisionIntersections[state->numCollisionIntersections].text, MAX_LINE_LENGTH, format, args);
    va_end(args);

    state->collisionIntersections[state->numCollisionIntersections].v0 = v0;
    state->collisionIntersections[state->numCollisionIntersections].v1 = v1;
    state->numCollisionIntersections++;
}

void Console::draw(ConsoleProfileInfo profileInfo, float scale, const Vector2 &offset, ImFont *boldFont)
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;

    ImGui::SetNextWindowSizeConstraints(ImVec2(displaySize.x, 250), ImVec2(displaySize.x, 250));
    ImGui::SetNextWindowPos(ImVec2(displaySize.x, displaySize.y - 250), ImGuiCond_Always, ImVec2(1, 0));
    ImGui::Begin("Console");
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;
    ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_None,
                      window_flags);

    if (ImGui::Button("Clear"))
    {
        clear();
    }

    int startIndex = state->numLines >= BUFFER_SIZE ? state->numLines % BUFFER_SIZE : 0;
    int numLinesToDraw = state->numLines < BUFFER_SIZE ? state->numLines : BUFFER_SIZE;
    for (int i = 0; i < numLinesToDraw; i++)
    {
        int index = (startIndex + i) % BUFFER_SIZE;
        ImGui::Separator();
        const LineEntry &entry = state->lines[index];

        if (entry.bold)
        {
            ImGui::PushFont(boldFont);
        }

        ImGui::TextColored(ImVec4(static_cast<float>(entry.r) / 255.0f,
                                  static_cast<float>(entry.g) / 255.0f,
                                  static_cast<float>(entry.b) / 255.0f,
                                  1.0f),
                           "[%d] %s", entry.writeIndex, entry.text);

        if (entry.bold)
        {
            ImGui::PopFont();
        }
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    ImGui::End();

    ImDrawList *foreground = ImGui::GetForegroundDrawList();

    for (int i = 0; i < state->numCollisionIntersections; i++)
    {
        CollisionIntersectionEntry &entry = state->collisionIntersections[i];

        // // Scale coordinates by scale and translate by offset
        // m->vertices[i].pos.x = m->vertices[i].pos.x * scale + offset.x;
        // m->vertices[i].pos.y = m->vertices[i].pos.y * scale + offset.y;

        Vector2 v0 = entry.v0 * scale + offset;
        Vector2 v1 = entry.v1 * scale + offset;

        foreground->AddLine(ImVec2(v0.x, v0.y), ImVec2(v1.x, v1.y), IM_COL32(255, 0, 0, 255), 1.0f);
        foreground->AddText(ImVec2(v0.x, v0.y), IM_COL32(0, 0, 0, 255), entry.text);
    }

    for (int i = 0; i < state->numPosLines; i++)
    {
        PositionLineEntry &entry = state->positionLines[i];
        Vector2 entryPos = Vector2(entry.x, entry.y) * scale + offset;

        foreground->AddText(ImVec2(entryPos.x, entryPos.y), IM_COL32(entry.r, entry.g, entry.b, 255), entry.text);
        if (entry.vx != 0 || entry.vy != 0)
        {
            Vector2 velocity = Vector2(entry.vx, entry.vy) * scale;
            foreground->AddLine(ImVec2(entryPos.x, entryPos.y), ImVec2(entryPos.x + velocity.x, entryPos.y + velocity.y), IM_COL32(255, 0, 0, 255), 1.0f);
        }
    }

    for (int i = 0; i < state->numCollisionImpulses; i++)
    {
        CollisionImpulseEntry &entry = state->collisionImpulses[i];
        Vector2 entryPoint = entry.point * scale + offset;
        foreground->AddLine(ImVec2(entryPoint.x, entryPoint.y), ImVec2(entryPoint.x + entry.impulse.x * scale, entryPoint.y + entry.impulse.y * scale), IM_COL32(255, 0, 0, 255), 1.0f);
    }

    for (int i = 0; i < state->numDebugGeometry; i++)
    {
        DebugGeometryEntry &entry = state->debugGeometry[i];
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

void Console::drawPoint(const Vector2 &point, unsigned int color)
{
    if (state->numDebugGeometry >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in debug geometry buffer");
        return;
    }

    state->debugGeometry[state->numDebugGeometry].geometry = DebugGeometryPoint;
    state->debugGeometry[state->numDebugGeometry].v0 = point;
    state->debugGeometry[state->numDebugGeometry].r = (color >> 16) & 0xFF;
    state->debugGeometry[state->numDebugGeometry].g = (color >> 8) & 0xFF;
    state->debugGeometry[state->numDebugGeometry].b = color & 0xFF;
    state->numDebugGeometry++;
}

void Console::drawBoundingBox(const ShapeBoundingBox &box, unsigned int color)
{
    drawQuad(Vector2(box.x1, box.y1), Vector2(box.x2, box.y1), Vector2(box.x2, box.y2), Vector2(box.x1, box.y2), color);
}

void Console::drawQuad(const Vector2 &p0, const Vector2 &p1, const Vector2 &p2, const Vector2 &p3, unsigned int color)
{
    if (state->numDebugGeometry >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in debug geometry buffer");
        return;
    }

    state->debugGeometry[state->numDebugGeometry].geometry = DebugGeometryQuad;
    state->debugGeometry[state->numDebugGeometry].v0 = p0;
    state->debugGeometry[state->numDebugGeometry].v1 = p1;
    state->debugGeometry[state->numDebugGeometry].v2 = p2;
    state->debugGeometry[state->numDebugGeometry].v3 = p3;
    state->debugGeometry[state->numDebugGeometry].r = (color >> 16) & 0xFF;
    state->debugGeometry[state->numDebugGeometry].g = (color >> 8) & 0xFF;
    state->debugGeometry[state->numDebugGeometry].b = color & 0xFF;
    state->numDebugGeometry++;
}

void Console::drawSegment(const Vector2 &v0, const Vector2 &v1, unsigned int color)
{

    if (state->numDebugGeometry >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in debug geometry buffer");
        return;
    }

    state->debugGeometry[state->numDebugGeometry].geometry = DebugGeometrySegment;
    state->debugGeometry[state->numDebugGeometry].v0 = v0;
    state->debugGeometry[state->numDebugGeometry].v1 = v1;
    state->debugGeometry[state->numDebugGeometry].r = (color >> 16) & 0xFF;
    state->debugGeometry[state->numDebugGeometry].g = (color >> 8) & 0xFF;
    state->debugGeometry[state->numDebugGeometry].b = color & 0xFF;
    state->numDebugGeometry++;
}

void Console::drawVelocityVector(const Vector2 &position, const Vector2 &vector, unsigned int color)
{
    drawVector(position, vector * 10.0f, color);
}

void Console::drawVector(const Vector2 &position, const Vector2 &vector, unsigned int color)
{
    if (state->numDebugGeometry >= BUFFER_SIZE)
    {
        Console::log("Error: overflow in debug geometry buffer");
        return;
    }

    state->debugGeometry[state->numDebugGeometry].geometry = DebugGeometryVector;
    state->debugGeometry[state->numDebugGeometry].v0 = position;
    state->debugGeometry[state->numDebugGeometry].v1 = position + vector;
    state->debugGeometry[state->numDebugGeometry].r = (color >> 16) & 0xFF;
    state->debugGeometry[state->numDebugGeometry].g = (color >> 8) & 0xFF;
    state->debugGeometry[state->numDebugGeometry].b = color & 0xFF;
    state->numDebugGeometry++;
}

void Console::printToStandardOut()
{
    for (int i = 0; i < state->numLines; i++)
    {
        int index = i % BUFFER_SIZE;
        printf("[%d] %s\n", state->lines[index].writeIndex, state->lines[index].text);
    }
}
