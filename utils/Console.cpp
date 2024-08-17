#include "Console.h"
#include "../game/Scenes.h"
#include "../game/PhysicsTests.h"
#include "../game/Physics.h"
#include "../containers/Range.h"
#include "../game/Game.h"
#include "imgui.h"
#include <cstdarg>
#include <cstdio>

#define MAX_LINE_LENGTH 1000

PhysicsTestDefinition *curTestCase = nullptr;

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
LineEntry lines[BUFFER_SIZE] = {0};
PositionLineEntry positionLines[BUFFER_SIZE] = {0};

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
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, displaySize.y - 20), ImVec2(300, displaySize.y - 20));
    ImGui::SetNextWindowPos(ImVec2(displaySize.x, 0), ImGuiCond_Always, ImVec2(1, 0));
    ImGui::Begin("Dev tools", &showingConsole);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("DevTools", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Tests"))
        {
            ImGui::SeparatorText("All tests");

            for (int i = 0; i < PhysicsTestDefinition::numTests; i++)
            {
                PhysicsTestDefinition *testCase = &PhysicsTestDefinition::allTests[i];
                const bool isSelected = curTestCase == testCase;
                if (ImGui::Selectable(testCase->name, isSelected))
                {
                    curTestCase = testCase;
                    curTestCase->start(&game);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            if (curTestCase != nullptr)
            {
                ImGui::SeparatorText("Test executor");
                ImGui::Text("Test progress: %d / %d", curTestCase->time, curTestCase->duration);
                ImGui::Text("Failed: %s", curTestCase->invariantResult != nullptr ? curTestCase->invariantResult : "false");
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Scenes"))
        {
            ImGui::BeginListBox("##empty");
            for (int i = 0; i < SceneDefinition::numScenes; i++)
            {
                const SceneDefinition &scene = SceneDefinition::allScenes[i];
                const bool isSelected = (selectedSceneIndex == i);
                if (ImGui::Selectable(scene.name, isSelected))
                {
                    game.clear();
                    PhysicsSpace &space = game.physicsSpace();
                    space.collisionsEnabled = true;
                    space.gravityEnabled = true;
                    scene.initFunc(&game);
                    selectedSceneIndex = i;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::SeparatorText("Profile info");
    ImGui::Text("Total Physics time: %.2lf ms", profileInfo.totalPhysicsTimeMillis);
    ImGui::Text("Elapsed step time: %.2lf ms", profileInfo.elapsedStepTimeMillis);
    ImGui::Text("Physics iterations: %d", profileInfo.numPhysicsSteps);
    ImGui::Text("Num springs: %d", profileInfo.numSprings);
    ImGui::Text("Physics time: %.2lf ms", profileInfo.physicsTimeMillis);
    ImGui::Text("Render time: %.2lf ms", profileInfo.renderTimeMillis);
    ImGui::Text("Swap time: %.2lf ms", profileInfo.swapTimeMillis);
    ImGui::Text("Springs time: %.2lf ms", profileInfo.springsTimeMillis);
    ImGui::Text("Bounding box time: %.2lf ms", profileInfo.boundingBoxTimeMillis);
    ImGui::Text("Num bboxes: %d", profileInfo.numBboxes);
    ImGui::Text("Num bbox checks: %d", profileInfo.numBbboxChecks);
    ImGui::Text("Collisions time: %.2lf ms", profileInfo.collisionTimeMillis);

    ImGui::End();

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

bool Console::executingTest()
{
    return curTestCase != nullptr && curTestCase->time < curTestCase->duration && curTestCase->invariantResult == nullptr;
}

void Console::stepTest(Game *game, double elapsedMilliseconds, ConsoleProfileInfo &profileInfo)
{
    if (curTestCase != nullptr)
    {
        curTestCase->step(game, elapsedMilliseconds, profileInfo);
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
