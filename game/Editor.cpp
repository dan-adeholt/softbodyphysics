#include "Game.h"

#include <stdio.h>
#include "./Editor.h"
#include "Game.h"
#include "../physics/PhysicsSpace.h"
#include "Shapes.h"
#include "../game/Scenes.h"
#include "../utils/MinMax.h"
#include "../utils/Console.h"
#include "../utils/ParseIniFile.h"
#include "../containers/Array.h"
#include "../containers/StringBuffer.h"
#include "../imgui/imgui.h"
#include "../physics/PhysicsSpaceStorage.h"
#include "../physics/PhysicsTests.h"
#include "../fontawesome/IconsFontAwesome4.h"
#include "../utils/MinMax.h"

const char *contextMenu = "Context menu";
const char *bridgePopup = "Bridge";

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
    StringBuffer<MAX_FILENAME_LENGTH> name;
};

struct BridgePopup
{
    void show()
    {
        shouldTriggerShow = true;
    }

    void render(Game &game)
    {
        if (shouldTriggerShow)
        {
            ImVec2 pos = ImGui::GetMousePos();
            addPos = (Vector2(pos.x, pos.y) - game.offset()) / game.scale();
            ImGui::OpenPopup(bridgePopup);
            shouldTriggerShow = false;
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(bridgePopup, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::PushItemWidth(170);
            ImGui::InputInt("#Segments", &numBridgeSegments);
            ImGui::PopItemWidth();
            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                Console::drawPoint(addPos, 0xFF0000);
                Shapes::createBridge(game.physicsSpace(), addPos.x, addPos.y, 1.0f, numBridgeSegments);

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    bool shouldTriggerShow = false;
    int numBridgeSegments = 10;
    Vector2 addPos;
};

struct Editor::Impl
{
    Impl() : fileList(MAX_FILES)
    {
    }

    ~Impl()
    {
        for (int i = 0; i < games.size(); i++)
        {
            delete games[i];
        }

        games.clear();
    }

    Array<FileEntry> fileList;
    StringBuffer<MAX_FILENAME_LENGTH> lastOpenedFile;
    StringBuffer<MAX_FILENAME_LENGTH> lastScene;
    bool isPlaying = false;
    const char *appPath = nullptr;
    Array<Game *> games;
    int currentGameIndex = 0;
    Array<bool> closeTabStates;
    char newFileName[MAX_FILENAME_LENGTH] = {};
    Array<FileEntry> openedBuffers;
    int gameTabIndex = -1;
    BridgePopup bridgePopup;
};

Editor::Editor(const char *appPath)
{

    m = new Impl;
    m->appPath = appPath;

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
            fileEntry.name.append(entry->d_name);
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
    m->games.push(new Game(appPath, nullptr));

    if (stateFile != NULL)
    {
        parseIniFile(stateFile, this, &Editor::readIniValue);
        fclose(stateFile);
    }

    m->currentGameIndex = min(m->games.size() - 1, m->currentGameIndex);
    Game &game = *m->games[0];
    game.init(lastSceneName());
}

void Editor::readIniValue(const char *section, const char *name, const char *value)
{
    if (strcmp(section, "Editor") == 0)
    {
        if (strcmp(name, "LastOpenedFile") == 0)
        {
            m->lastOpenedFile.append(value);
        }

        if (strcmp(name, "LastScene") == 0)
        {
            m->lastScene.append(value);
        }

        if (strcmp(name, "CurrentGameIndex") == 0)
        {
            m->currentGameIndex = atoi(value);
        }

        if (strcmp(name, "OpenedBuffers") == 0)
        {
            m->openedBuffers.clear();
            const char *start = value;
            const char *end = value;
            while (*end != '\0')
            {
                if (*end == ',')
                {
                    FileEntry fileEntry;
                    fileEntry.name.appendRange(start, end);
                    Console::log("Opened buffer: %s", fileEntry.name.data);
                    m->openedBuffers.push(fileEntry);
                    Game *newGame = new Game(m->appPath, fileEntry.name.data);

                    StringBuffer<512> fullPath;
                    fullPath.append("%s/%s", m->appPath, fileEntry.name.data);

                    PhysicsSpaceStorage::loadFromFile(newGame->physicsSpace(), fullPath.data);
                    m->games.push(newGame);
                    start = end + 1;
                }
                end++;
            }
        }
    }
}

Game *Editor::getCurrentGame()
{
    return m->games[m->currentGameIndex];
}

Editor::~Editor()
{
    delete m;
}

bool ImGuiBeginTallerMenu(const char *menuName)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 18.0f));
    bool ret = ImGui::BeginMenu(menuName);
    ImGui::PopStyleVar();
    return ret;
}

void Editor::renderUI(Game &game, ConsoleProfileInfo &profileInfo)
{
    bool triggerBridgePopup = false;
    bool triggerOpenPopup = false;
    int deleteGameIndex = -1;
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;

    if (ImGui::IsKeyPressed(ImGuiKey_N) && (io.KeyMods & ImGuiMod_Ctrl) != 0)
    {
        triggerOpenPopup = true;
    }

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.85f, 0.85f, 0.85f, 1.0f)); // Menu bar background color
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));     // Menu bar background color
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 9.0f));

    ImGui::SetNextWindowSizeConstraints(ImVec2(250, displaySize.y - 283), ImVec2(250, displaySize.y - 283));
    ImGui::SetNextWindowPos(ImVec2(250, 22), ImGuiCond_Always, ImVec2(1, 0));

    if (ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
    {
        ImVec2 buttonSize(32.0f, 35.0f);
        ImGui::PushButtonRepeat(true);

        float buttonVerticalMargin = 30.0f;
        const char *currentSceneName = game.currentSceneName();

        if (ImGui::Button(ICON_FA_STEP_BACKWARD, buttonSize) && currentSceneName != nullptr)
        {
            const SceneDefinition *scene = SceneDefinition::getDefinitionFromName(currentSceneName);
            game.init(*scene);
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_BACKWARD, buttonSize))
        {
            game.setPaused();

            game.rewindHistory();
        }

        ImGui::SameLine();

        if (ImGui::Button(game.paused() ? ICON_FA_PLAY : ICON_FA_PAUSE, buttonSize))
        {
            game.togglePaused();
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_STEP_FORWARD, buttonSize))
        {
            game.update(1000.0 / 120.0, true, profileInfo);
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_FA_FORWARD, buttonSize))
        {
            game.setPaused();
            game.forwardHistory();
        }

        ImGui::Dummy(ImVec2(10.0f, 0.0f));
        ImGui::PopButtonRepeat();

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

        if (ImGui::BeginTabBar("EditorTabs", tab_bar_flags))
        {

            if (ImGui::BeginTabItem("Scenes"))
            {
                for (int i = 0; i < SceneDefinitionFolder::numFolders; i++)
                {
                    const SceneDefinitionFolder &folder = SceneDefinitionFolder::allFolders[i];
                    if (ImGui::TreeNode(folder.name))
                    {
                        ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
                        for (int j = 0; j < folder.numScenes; j++)
                        {
                            const SceneDefinition &scene = folder.scenes[j];

                            const bool isSelected = currentSceneName != nullptr && strcmp(scene.name, currentSceneName) == 0;
                            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
                            if (isSelected)
                            {
                                node_flags |= ImGuiTreeNodeFlags_Selected;
                            }

                            ImGui::TreeNodeEx((void *)(intptr_t)j, node_flags, "%s %s", ICON_FA_FILE_CODE_O, scene.name);
                            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                            {
                                m->currentGameIndex = 0;
                                m->lastScene.clear();
                                m->lastScene.append(scene.name);
                                m->games[0]->init(scene);
                                saveState();
                            }
                        }
                        ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
                        ImGui::TreePop();
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Profiler"))
            {
                ImGui::Text("Slowdown factor: %.2lf", profileInfo.slowdownFactor);
                ImGui::Text("Total Physics time: %.2lf ms", profileInfo.totalPhysicsTimeMillis);
                ImGui::Text("Elapsed step time: %.2lf ms", profileInfo.elapsedStepTimeMillis);
                ImGui::Text("Physics iterations: %d", profileInfo.numPhysicsSteps);
                ImGui::Text("Physics time: %.2lf ms", profileInfo.physicsTimeMillis);
                ImGui::Text("Render time: %.2lf ms", profileInfo.renderTimeMillis);
                ImGui::Text("Swap time: %.2lf ms", profileInfo.swapTimeMillis);
                ImGui::Text("Springs time: %.2lf ms", profileInfo.springsTimeMillis);
                ImGui::Text("Bounding box time: %.2lf ms", profileInfo.boundingBoxTimeMillis);
                ImGui::Text("Num bboxes: %d", profileInfo.numBboxes);
                ImGui::Text("Num bbox checks: %d", profileInfo.numBbboxChecks);
                ImGui::Text("Num bbox overlaps: %d", profileInfo.numBboxOverlaps);
                ImGui::Text("Num collisions: %d", profileInfo.numCollisions);
                ImGui::Text("Collisions time: %.2lf ms", profileInfo.collisionTimeMillis);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Shape"))
            {
                int selectedShapeIndex = game.selectedShapeIndex();
                PhysicsSpace &space = game.physicsSpace();

                if (space.mouseJoint.pointIndex != -1)
                {
                    ImGui::Text("Mouse joint: %d", space.mouseJoint.pointIndex);
                    ImGui::Text("Position: %.2f, %.2f", space.points.pos[space.mouseJoint.pointIndex].x, space.points.pos[space.mouseJoint.pointIndex].y);
                    ImGui::Text("Velocity: %.2f, %.2f", space.points.velocity[space.mouseJoint.pointIndex].x, space.points.velocity[space.mouseJoint.pointIndex].y);
                    ImGui::Text("Mass: %.2f", space.points.mass[space.mouseJoint.pointIndex]);
                }

                if (selectedShapeIndex != -1)
                {

                    if (space.shapes.size() > selectedShapeIndex)
                    {
                        Shape &shape = space.shapes[selectedShapeIndex];
                        ImGui::Text("Selected shape: %d [%d]", selectedShapeIndex, shape.parentId);

                        ImGui::Checkbox("Static", &shape.isStatic);

                        if (ImGui::Button("Snap to grid"))
                        {
                            Shapes::snapToGrid(space, selectedShapeIndex);
                        }

                        if (ImGui::Button("Update original position"))
                        {
                            Shapes::updateOriginalPos(space, selectedShapeIndex);
                        }
                    }
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
    // Detect right-click anywhere on the canvas
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(contextMenu);
    }

    if (ImGui::BeginPopup(contextMenu))
    {
        PhysicsSpace &space = game.physicsSpace();
        int prevShapesSize = space.shapes.size();

        if (ImGui::BeginMenu("Static shapes"))
        {
            ImVec2 pos = ImGui::GetMousePos();
            PhysicsSpace &space = game.physicsSpace();
            Vector2 addPos = (Vector2(pos.x, pos.y) - game.offset()) / game.scale();
            float defaultMass = 4.0f;
            float size = gridSize;

            if (ImGui::MenuItem("Quad"))
            {
                Shapes::createStaticQuad(space, addPos.x, addPos.y, size, size, defaultMass);
            }

            if (ImGui::MenuItem("Circle"))
            {
                Shapes::createCircle(space, addPos.x, addPos.y, size, defaultMass);
                int shapeIndex = space.shapes.size() - 1;
                Shape &circle = space.shapes[shapeIndex];
                circle.isStatic = true;
            }

            if (ImGui::MenuItem("Triangle"))
            {
                Shapes::createTriangle(space, true, addPos.x, addPos.y, addPos.x + size, addPos.y, addPos.x + size, addPos.y + size, defaultMass);
            }

            ImGui::EndMenu();
        }

        ImVec2 pos = ImGui::GetMousePos();
        Vector2 addPos = (Vector2(pos.x, pos.y) - game.offset()) / game.scale();

        if (ImGui::BeginMenu("Shapes"))
        {

            float defaultMass = 2.0f;
            float size = gridSize;

            if (ImGui::MenuItem("Quad"))
            {
                Shapes::createQuad(space, addPos.x, addPos.y, size, size, defaultMass);
            }

            if (ImGui::MenuItem("Circle"))
            {
                Shapes::createCircle(space, addPos.x, addPos.y, size, defaultMass);
            }

            if (ImGui::MenuItem("Heavy circle"))
            {
                Shapes::createCircle(space, addPos.x, addPos.y, size, defaultMass * 50.0f);
            }

            if (ImGui::MenuItem("Triangle"))
            {
                Shapes::createTriangle(space, false, addPos.x, addPos.y, addPos.x + size, addPos.y, addPos.x + size, addPos.y + size, defaultMass);
            }

            if (ImGui::MenuItem("Bridge"))
            {
                m->bridgePopup.show();
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        int selectedShapeIndex = game.selectedShapeIndex();
        if (ImGui::MenuItem("Add point") && selectedShapeIndex != -1)
        {
            ImVec2 popupPos = ImGui::GetWindowPos();
            Vector2 newPos = (Vector2(popupPos.x, popupPos.y) - game.offset()) / game.scale();

            Shapes::addPointToShape(space, selectedShapeIndex, newPos.x, newPos.y);
        }

        if (space.shapes.size() != prevShapesSize)
        {
            game.updateBoundingBoxes();
            game.setSelectedShapeIndex(space.shapes.size() - 1);
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGuiBeginTallerMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N/Meta+N"))
            {
                triggerOpenPopup = true;
            }

            if (ImGui::BeginMenu("Open"))
            {
                for (int i = 0; i < m->fileList.size(); i++)
                {
                    if (ImGui::MenuItem(m->fileList[i].name.data))
                    {
                        m->lastOpenedFile.clear();
                        m->lastOpenedFile.append(m->fileList[i].name.data);

                        Game *newGame = new Game(m->appPath, m->lastOpenedFile.data);
                        m->games.push(newGame);
                        m->currentGameIndex = m->games.size() - 1;
                        m->openedBuffers.push({m->lastOpenedFile});
                        saveState();
                        StringBuffer<512> fullPath;
                        fullPath.append("%s/%s", m->appPath, m->lastOpenedFile.data);
                        PhysicsSpaceStorage::loadFromFile(newGame->physicsSpace(), fullPath.data);
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Save", "Ctrl+S/Meta+S"))
            {
                game.saveToFile();
            }

            if (ImGui::MenuItem("Dump to unit test"))
            {
                PhysicsSpaceStorage::dumpToUnitTest(game.physicsSpace(), game.scale(), game.offset());
            }
            ImGui::EndMenu();
        }

        if (ImGuiBeginTallerMenu("Settings"))
        {
            ImGui::Checkbox("Gravity", &game.physicsSpace().gravityEnabled);
            ImGui::Checkbox("Collisions", &game.physicsSpace().collisionsEnabled);
            ImGui::Checkbox("Shape matching", &game.physicsSpace().shapeMatchingEnabled);
            ImGui::Checkbox("Springs", &game.physicsSpace().springsEnabled);
            ImGui::DragInt("Speed", &game.simulationSpeed(), 1.0, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
            ImGui::EndMenu();
        }

        if (ImGuiBeginTallerMenu("Tests"))
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

        ImGui::EndMainMenuBar();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::SetNextWindowSizeConstraints(ImVec2(displaySize.x - 246, 29), ImVec2(displaySize.x - 246, 29));
    ImGui::SetNextWindowPos(ImVec2(246, 23), ImGuiCond_Always, ImVec2(0, 0));

    if (ImGui::Begin("Scenes window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration))
    {
        if (triggerOpenPopup)
        {
            ImGui::OpenPopup("Add level");
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Add level", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (triggerOpenPopup)
            {
                m->newFileName[0] = '\0';
                ImGui::SetKeyboardFocusHere();
            }

            ImGui::InputText("Level name", m->newFileName, sizeof(m->newFileName));

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                Console::log("Closing popup");
                if (strlen(m->newFileName) > 0)
                {
                    StringBuffer<256> newFileNameWithExtension;
                    newFileNameWithExtension.append("%s.txt", m->newFileName);
                    Game *newGame = new Game(m->appPath, newFileNameWithExtension.data);
                    m->games.push(newGame);
                    m->openedBuffers.push({newFileNameWithExtension});

                    m->currentGameIndex = m->games.size() - 1;
                    saveState();
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        m->closeTabStates.fill(true, m->games.size());

        if (ImGui::BeginTabBar("Scenes", ImGuiTabBarFlags_FittingPolicyResizeDown))
        {
            bool setSelectedTabState = m->currentGameIndex != m->gameTabIndex;

            for (int i = 0; i < m->closeTabStates.size(); i++)
            {
                StringBuffer<256> tabName;
                tabName.append(" %s %s ", ICON_FA_FILE_CODE_O, m->games[i]->title());

                if (m->closeTabStates[i] && ImGui::BeginTabItem(tabName.data, i == 0 ? nullptr : &m->closeTabStates[i], i == m->currentGameIndex && setSelectedTabState ? ImGuiTabItemFlags_SetSelected : 0))
                {
                    if (ImGui::IsItemActive() && i != m->currentGameIndex)
                    {
                        m->currentGameIndex = i;
                        saveState();
                    }

                    ImGui::EndTabItem();
                }
            }

            m->gameTabIndex = m->currentGameIndex;

            if (ImGui::TabItemButton("...", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            {
                // m->games.push(new Game(m->appPath));
                // m->closeTabStates.push(true);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        for (int i = 0; i < m->closeTabStates.size(); i++)
        {
            if (!m->closeTabStates[i])
            {
                deleteGameIndex = i;
                break;
            }
        }

        ImGui::End();
    }
    ImGui::PopStyleVar(1);

    m->bridgePopup.render(game);

    if (deleteGameIndex != -1)
    {
        Game *g = m->games[deleteGameIndex];
        delete g;
        m->games.remove(deleteGameIndex);
        m->openedBuffers.remove(deleteGameIndex - 1);
        m->closeTabStates.remove(deleteGameIndex);

        if (m->currentGameIndex >= deleteGameIndex)
        {
            m->currentGameIndex--;
            if (m->currentGameIndex < 0)
            {
                m->currentGameIndex = 0;
            }
        }

        saveState();
    }
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
    writeIniProperty(stateFile, "LastOpenedFile", m->lastOpenedFile.data);
    writeIniProperty(stateFile, "LastScene", m->lastScene.data);
    StringBuffer<24> currentGameIndexStr;
    currentGameIndexStr.append("%d", m->currentGameIndex);
    writeIniProperty(stateFile, "CurrentGameIndex", currentGameIndexStr.data);

    StringBuffer<2048> openedFiles;

    for (int i = 0; i < m->openedBuffers.size(); i++)
    {
        openedFiles.append("%s,", m->openedBuffers[i].name.data);
    }

    writeIniProperty(stateFile, "OpenedBuffers", openedFiles.data);

    // Flush the file buffer
    fflush(stateFile);

    // Close the file
    fclose(stateFile);
}

const char *Editor::lastSceneName()
{
    return m->lastScene.data;
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
