#include "Console.h"
#include "../game/Scenes.h"
#include "../game/Physics.h"
#include "../containers/Range.h"
#include "../game/Game.h"
#include "imgui.h"
#include <cstdarg>
#include <cstdio>

#define MAX_LINE_LENGTH 1000

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

#define BUFFER_SIZE 1000
LineEntry lines[BUFFER_SIZE] = {};
PositionLineEntry positionLines[BUFFER_SIZE] = {};

int numPosLines = 0;
int numLines = 0;

void Console::clear()
{
    numLines = 0;
}

void Console::clearFrame()
{
    numPosLines = 0;
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

void Console::draw(Game &game, ConsoleProfileInfo profileInfo)
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;


    ImGui::SetNextWindowSizeConstraints(ImVec2(displaySize.x, 200), ImVec2(displaySize.x, 200));
    ImGui::SetNextWindowPos(ImVec2(displaySize.x, displaySize.y - 200), ImGuiCond_Always, ImVec2(1, 0));
    ImGui::Begin("Console");
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;
    ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_None,
                      window_flags);

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

    // for (int i = 0; i < pointMasses.size; i++)
    // {
    //     PointMass &point = pointMasses[i];
    //     snprintf(buffer, 100, "%d", i);
    //     foreground->AddText(ImVec2(point.pos.x - 5, point.pos.y + 5), IM_COL32(0, 0, 0, 255), buffer);
    // }

    for (int i = 0; i < numPosLines; i++)
    {
        PositionLineEntry &entry = positionLines[i];
        foreground->AddText(ImVec2(entry.x, entry.y), IM_COL32(entry.r, entry.g, entry.b, 255), entry.text);
        if (entry.vx != 0 || entry.vy != 0)
        {
            foreground->AddLine(ImVec2(entry.x, entry.y), ImVec2(entry.x + entry.vx, entry.y + entry.vy), IM_COL32(255, 0, 0, 255), 1.0f);
        }
    }
}

void Console::printToStandardOut()
{
    for (int i = 0; i < numLines; i++)
    {
        int index = i % BUFFER_SIZE;
        printf("[%d] %s\n", lines[index].writeIndex, lines[index].text);
    }
}
