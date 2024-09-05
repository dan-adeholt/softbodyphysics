#include "Game.h"

#include <stdio.h>
#include "./Editor.h"
#include "Game.h"
#include "Physics.h"
#include "Shapes.h"
#include "../game/Scenes.h"
#include "../utils/Console.h"
#include "../utils/ParseIniFile.h"
#include "../containers/Array.h"
#include "../imgui/imgui.h"
#include "../game/PhysicsSpaceStorage.h"
#include "../game/PhysicsTests.h"
#include "../fontawesome/IconsFontAwesome4.h"

#ifdef __APPLE__
#include <dirent.h>
#include <sys/stat.h>
#include "Editor.h"
#else
// TODO: Implement for other platforms
#endif

const char *levelsDirectory = "levels";
PhysicsTestDefinition *curTestCase = nullptr;

#define MAX_FILENAME_LENGTH 256
#define MAX_FILES 100

struct FileEntry
{
    char name[MAX_FILENAME_LENGTH];
};

struct Editor::Impl
{
    Impl() : fileList(MAX_FILES), isPlaying(false)
    {
        lastOpenedFile[0] = '\0';
    }

    bool showProfiler;
    Array<FileEntry> fileList;
    char lastOpenedFile[MAX_FILENAME_LENGTH];
    bool isPlaying;
};

Editor::Editor()
{
    m = new Impl;
    m->showProfiler = false;
#ifdef __APPLE__
    DIR *dir = opendir(levelsDirectory);
    if (dir == NULL)
    {
        Console::log("Error opening levels directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            FileEntry fileEntry;

            snprintf(fileEntry.name, MAX_FILENAME_LENGTH, "%s/%s", levelsDirectory, entry->d_name);
            fileEntry.name[MAX_FILENAME_LENGTH - 1] = '\0';
            m->fileList.push(fileEntry);
        }
    }

    closedir(dir);
#else
// TODO: Implement for other platforms
#endif

    // Load the editor state
    const char *stateFileName = "editor_state.ini";

    FILE *stateFile = fopen(stateFileName, "r");
    if (stateFile != NULL)
    {
        parseIniFile(stateFile, this, &Editor::readIniValue);
        fclose(stateFile);
    }
}

void Editor::readIniValue(const char *section, const char *name, const char *value)
{
    if (strcmp(section, "Editor") == 0)
    {
        if (strcmp(name, "LastOpenedFile") == 0)
        {
            strcpy(m->lastOpenedFile, value);
        }
        else if (strcmp(name, "ShowProfiler") == 0)
        {
            m->showProfiler = atoi(value) != 0;
        }
    }
}

Editor::~Editor()
{
    delete m;
}

void Editor::renderUI(Game &game, ConsoleProfileInfo &profileInfo)
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;
    ImGui::SetNextWindowSizeConstraints(ImVec2(displaySize.x, 0), ImVec2(displaySize.x, 0));
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.85f, 0.85f, 0.85f, 1.0f)); // Menu bar background color
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));     // Menu bar background color

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save") && m->lastOpenedFile[0] != '\0')
            {
                PhysicsSpaceStorage::dumpToFile(game.physicsSpace(), m->lastOpenedFile);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Levels"))
        {
            for (int i = 0; i < m->fileList.size(); i++)
            {
                if (ImGui::MenuItem(m->fileList[i].name))
                {
                    snprintf(m->lastOpenedFile, MAX_FILENAME_LENGTH, "%s", m->fileList[i].name);
                    PhysicsSpaceStorage::loadFromFile(game.physicsSpace(), m->lastOpenedFile);

                    // TODO: Load level
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Settings"))
        {
            ImGui::Checkbox("Show profiler", &m->showProfiler);
            ImGui::Checkbox("Gravity", &game.physicsSpace().gravityEnabled);
            ImGui::Checkbox("Collisions", &game.physicsSpace().collisionsEnabled);
            ImGui::Checkbox("Shape matching", &game.physicsSpace().shapeMatchingEnabled);
            ImGui::Checkbox("Springs", &game.physicsSpace().springsEnabled);
            ImGui::DragInt("Speed", &game.simulationSpeed(), 1.0, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
            ImGui::EndMenu();
        }

        const char *currentSceneName = game.currentSceneName();
        if (ImGui::BeginMenu("Scenes"))
        {
            for (int i = 0; i < SceneDefinition::numScenes; i++)
            {
                const SceneDefinition &scene = SceneDefinition::allScenes[i];

                const bool isSelected = currentSceneName != nullptr && strcmp(scene.name, currentSceneName) == 0;
                if (ImGui::MenuItem(scene.name))
                {
                    game.init(scene);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tests"))
        {
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

            ImGui::EndMenu();
        }
        ImVec2 buttonSize(32.0f, 18.0f);
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button(ICON_FA_STEP_BACKWARD, buttonSize) && currentSceneName != nullptr)
        {
            const SceneDefinition *scene = SceneDefinition::getDefinitionFromName(currentSceneName);
            game.init(*scene);
        }
        if (ImGui::Button(ICON_FA_BACKWARD, buttonSize))
        {
            game.paused() = true;
            game.rewindHistory();
        }
        ImGui::SameLine();
        if (ImGui::Button(game.paused() ? ICON_FA_PLAY : ICON_FA_PAUSE, buttonSize))
        {
            game.paused() = !game.paused();
        }
        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_STEP_FORWARD, buttonSize))
        {
            game.update(1000.0 / 120.0, profileInfo);
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_FORWARD, buttonSize))
        {
            game.paused() = true;
            game.forwardHistory();
        }
        ImGui::PopButtonRepeat();

        ImGui::EndMainMenuBar();
    }

    if (m->showProfiler)
    {
        ImGui::Begin("Profiler");
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
    }

    ImGui::PopStyleColor(2);
}

void Editor::saveState()
{
    // Open the editor state file for writing
    const char *stateFileName = "editor_state.ini";
    FILE *stateFile = fopen(stateFileName, "w");

    if (stateFile == NULL)
    {
        Console::log("Error: Unable to open editor state file for writing.");
        return;
    }

    writeIniSection(stateFile, "Editor");
    writeIniProperty(stateFile, "LastOpenedFile", m->lastOpenedFile);
    writeIniProperty(stateFile, "ShowProfiler", m->showProfiler ? "1" : "0");

    // Flush the file buffer
    fflush(stateFile);

    // Close the file
    fclose(stateFile);
}

bool Editor::executingTest()
{
    return curTestCase != nullptr && curTestCase->time < curTestCase->duration && curTestCase->invariantResult == nullptr;
}

void Editor::stepTest(Game *game, double elapsedMilliseconds, ConsoleProfileInfo &profileInfo)
{
    if (curTestCase != nullptr)
    {
        curTestCase->step(game, elapsedMilliseconds, profileInfo);
    }
}
