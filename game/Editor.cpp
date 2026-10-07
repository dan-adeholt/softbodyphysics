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
#include "../physics/ShapeUtils.h"
#include "../physics/CollisionSolver.h"
#include "../fontawesome/IconsFontAwesome4.h"
#include "../utils/MinMax.h"
#include "ShapeResources.h"
#include "SceneScript.h"
#include <stdlib.h>

const char *contextMenu = "Context menu";
const char *bridgePopup = "Bridge";

#if !defined(_WIN32)
#include <dirent.h>
#include <sys/stat.h>
#include "Editor.h"
#endif

const char *levelsDirectory = "levels";

#define MAX_FILENAME_LENGTH 256
#define MAX_FILES 100

struct FileEntry
{
    StringBuffer<MAX_FILENAME_LENGTH> name;
};

static float averageRadius(Range<Vector2> points, const Shape &shape, const Vector2 &center)
{
    float radius = 0.0f;
    int count = 0;

    for (ShapeIterator iter(shape); iter.isValid(); iter.next())
    {
        radius += (points[iter.index()] - center).length();
        count++;
    }

    return count > 0 ? radius / (float)count : 0.0f;
}

static float wheelRadiusRatio(PhysicsSpace &space, const Shape &shape)
{
    ShapeProperties properties = ShapeUtils::getShapeProperties(space.points.range(), shape);
    float currentRadius = averageRadius(space.points.pos.range(), shape, properties.center);
    float originalRadius = averageRadius(space.points.shapeOriginalPos.range(), shape, properties.origCenter);

    if (originalRadius <= 0.0f)
    {
        return 1.0f;
    }

    return currentRadius / originalRadius;
}

static const char *wheelMotorModeName(WheelMotorMode mode)
{
    switch (mode)
    {
    case WheelMotorMode::Drive:
        return "drive";
    case WheelMotorMode::Handover:
        return "handover";
    case WheelMotorMode::Brake:
        return "brake";
    case WheelMotorMode::Air:
        return "air";
    case WheelMotorMode::Coast:
    default:
        return "coast";
    }
}

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
    char newFileName[MAX_FILENAME_LENGTH] = {};
    Array<FileEntry> openedBuffers;
    BridgePopup bridgePopup;
    bool fitViewPending = true;
    ImVec2 lastDisplaySize;
    bool openAddLevelPopup = false;
    bool showConsole = false;
    bool showProfiler = false;
    bool sceneControlsCollapsed = true;
    bool debugDraw = false;
    bool antiAliasing = true;
    bool hasDragged = false;
};

Editor::Editor(const char *appPath)
{

    m = new Impl;
    m->appPath = appPath;

#if !defined(_WIN32)
    DIR *dir = opendir(levelsDirectory);
    if (dir == NULL)
    {
        Console::log("Error opening levels directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        bool isRegularFile = entry->d_type == DT_REG;
        if (entry->d_type == DT_UNKNOWN)
        {
            StringBuffer<MAX_FILENAME_LENGTH> fullPath;
            fullPath.append("%s/%s", levelsDirectory, entry->d_name);
            struct stat fileStat = {};
            isRegularFile = stat(fullPath.data, &fileStat) == 0 && S_ISREG(fileStat.st_mode);
        }

        if (isRegularFile)
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

    // First run, or the saved scene no longer exists
    if (SceneDefinition::getDefinitionFromName(m->lastScene.data) == nullptr)
    {
        m->lastScene.clear();
        m->lastScene.append(SceneDefinition::defaultSceneName);
    }

    setCurrentGameIndex(min(m->games.size() - 1, m->currentGameIndex));
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

        if (strcmp(name, "ShowProfiler") == 0)
        {
            m->showProfiler = strcmp(value, "1") == 0;
        }

        if (strcmp(name, "ShowConsole") == 0)
        {
            m->showConsole = strcmp(value, "1") == 0;
        }

        if (strcmp(name, "AntiAliasing") == 0)
        {
            m->antiAliasing = strcmp(value, "1") == 0;
        }

        if (strcmp(name, "DebugDraw") == 0)
        {
            m->debugDraw = strcmp(value, "1") == 0;
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

                    if (newGame->physicsSpace().isPrefab)
                    {
                        newGame->setPaused();
                    }

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

void Editor::setCurrentGameIndex(int index)
{
    m->currentGameIndex = index;
    Console::clearFrame();
}

Editor::~Editor()
{
    delete m;
}

// The toolbar floats at the top of the window; the editor panels are laid out below it
static const float toolbarMargin = 14.0f;
static const float toolbarHeight = 60.0f;
static const float toolbarBottom = toolbarMargin + toolbarHeight;
static const float levelTabsHeight = 46.0f;
static const float consoleHeight = 250.0f;

static const ImVec4 overlayWhite(1.0f, 1.0f, 1.0f, 1.0f);
static const ImVec4 overlayGreen(0.30f, 0.69f, 0.48f, 1.0f);

// Look shared by the toolbar and the floating overlays: white rounded panels with a warm border
static void pushOverlayStyle()
{
    const ImVec4 white = overlayWhite;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 9.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 10.0f);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, white);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, white);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.90f, 0.88f, 0.84f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.13f, 0.19f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, white);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.96f, 0.94f, 0.91f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.92f, 0.90f, 0.86f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, white);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.96f, 0.94f, 0.91f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.87f, 0.93f, 0.98f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.93f, 0.96f, 0.99f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.87f, 0.93f, 0.98f, 1.0f));
}

static void popOverlayStyle()
{
    ImGui::PopStyleColor(12);
    ImGui::PopStyleVar(8);
}

void Editor::renderUI(Game &game, ConsoleProfileInfo &profileInfo, ImFont *titleFont, ImFont *boldFont)
{
    renderToolbar(game, titleFont);
    renderLevelTabs();

    // The tabs can switch level or close the one that was current
    Game &currentGame = *getCurrentGame();
    renderCanvasPopups(currentGame);
    renderProfilerOverlay(currentGame, profileInfo);
    renderSceneControls(currentGame);
    updateCanvasHover(currentGame);
    renderDragHint(currentGame);

    if (m->showConsole)
    {
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        pushOverlayStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 12.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
        Console::drawWindow(toolbarMargin, displaySize.y - toolbarMargin - consoleHeight, displaySize.x - toolbarMargin * 2.0f, consoleHeight, boldFont);
        ImGui::PopStyleVar(2);
        popOverlayStyle();
    }
}

// Highlights the shape under the mouse, with a hand cursor, to show that it can be dragged.
// Not while the mouse is over the UI or outside the window.
void Editor::updateCanvasHover(Game &game)
{
    const bool overCanvas = ImGui::IsMousePosValid() && !ImGui::GetIO().WantCaptureMouse;
    const int hovered = game.dragging() || overCanvas ? game.shapeUnderMouse() : -1;
    game.setHoveredShapeIndex(hovered);

    if (hovered != -1)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
}

// Web visitors don't know that shapes can be dragged, so tell them until they have dragged one
void Editor::renderDragHint(Game &game)
{
#ifdef __EMSCRIPTEN__
    m->hasDragged = m->hasDragged || game.dragging();

    if (m->hasDragged)
    {
        return;
    }

    const char *hint = "Drag shapes to move them.";
    const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    const ImVec2 textSize = ImGui::CalcTextSize(hint);
    const ImVec2 padding(16.0f, 9.0f);
    const ImVec2 topLeft((displaySize.x - textSize.x) * 0.5f - padding.x, displaySize.y - toolbarMargin - textSize.y - padding.y * 2.0f);
    const ImVec2 bottomRight(topLeft.x + textSize.x + padding.x * 2.0f, topLeft.y + textSize.y + padding.y * 2.0f);

    // Drawn behind the UI windows, so the console covers it when open
    ImDrawList *drawList = ImGui::GetBackgroundDrawList();
    drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(255, 255, 255, 230), (bottomRight.y - topLeft.y) * 0.5f);
    drawList->AddRect(topLeft, bottomRight, IM_COL32(34, 48, 63, 60), (bottomRight.y - topLeft.y) * 0.5f);
    drawList->AddText(ImVec2(topLeft.x + padding.x, topLeft.y + padding.y), IM_COL32(34, 48, 63, 255), hint);
#endif
}

// Opened levels as a row of tabs below the toolbar. Only shown when a level file is open next to the scene.
void Editor::renderLevelTabs()
{
    if (m->games.size() < 2)
    {
        return;
    }

    int closeIndex = -1;

    pushOverlayStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 7.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 0.0f));
    ImGui::SetNextWindowPos(ImVec2(toolbarMargin, toolbarBottom + 8.0f));

    if (ImGui::Begin("Level tabs", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
    {
        for (int i = 0; i < m->games.size(); i++)
        {
            ImGui::PushID(i);
            const bool isCurrent = i == m->currentGameIndex;

            if (isCurrent)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.87f, 0.93f, 0.98f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.62f, 0.78f, 0.93f, 1.0f));
            }

            StringBuffer<256> tabName;
            tabName.append("%s  %s", i == 0 ? ICON_FA_FLASK : ICON_FA_FILE_TEXT_O, m->games[i]->title());

            if (ImGui::Button(tabName.data) && !isCurrent)
            {
                setCurrentGameIndex(i);
                saveState();
            }

            // The scene is always open, only level files can be closed
            if (i != 0)
            {
                ImGui::SameLine(0.0f, 2.0f);

                if (ImGui::Button(ICON_FA_TIMES))
                {
                    closeIndex = i;
                }
            }

            if (isCurrent)
            {
                ImGui::PopStyleColor(2);
            }

            ImGui::PopID();
            ImGui::SameLine(0.0f, 10.0f);
        }

        ImGui::NewLine();
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
    popOverlayStyle();

    if (closeIndex != -1)
    {
        delete m->games[closeIndex];
        m->games.remove(closeIndex);
        m->openedBuffers.remove(closeIndex - 1);

        if (m->currentGameIndex >= closeIndex)
        {
            setCurrentGameIndex(max(m->currentGameIndex - 1, 0));
        }

        saveState();
    }
}

// The right click menu for adding shapes, and the dialogs it and the File menu open
void Editor::renderCanvasPopups(Game &game)
{
    ImGuiIO &io = ImGui::GetIO();

    if (ImGui::IsKeyPressed(ImGuiKey_N) && (io.KeyMods & ImGuiMod_Ctrl) != 0)
    {
        m->openAddLevelPopup = true;
    }

    const bool openAddLevel = m->openAddLevelPopup;
    m->openAddLevelPopup = false;

    pushOverlayStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 12.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 8.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.97f, 0.96f, 0.93f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.97f, 0.96f, 0.93f, 1.0f));

    // Right click on the canvas, but not on the toolbar or other panels
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
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
                Shapes::createCircle(space, addPos.x, addPos.y, size, defaultMass * 6.0f);
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

    if (openAddLevel)
    {
        ImGui::OpenPopup("Add level");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Add level", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (openAddLevel)
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

                setCurrentGameIndex(m->games.size() - 1);
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

    m->bridgePopup.render(game);

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
    popOverlayStyle();
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
    writeIniProperty(stateFile, "ShowConsole", m->showConsole ? "1" : "0");
    writeIniProperty(stateFile, "ShowProfiler", m->showProfiler ? "1" : "0");
    writeIniProperty(stateFile, "DebugDraw", m->debugDraw ? "1" : "0");
    writeIniProperty(stateFile, "AntiAliasing", m->antiAliasing ? "1" : "0");

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

void Editor::loadScene(const SceneDefinition &scene)
{
    setCurrentGameIndex(0);
    Console::log("Loading scene: %s", scene.name);
    m->lastScene.clear();
    m->lastScene.append(scene.name);
    m->games[0]->init(scene);
    saveState();
    Console::clearFrame();
    m->fitViewPending = true;
}

// Zoom and center the view so the whole scene fits in the area not covered by the toolbar or panels
void Editor::fitViewToScene(Game &game)
{
    PhysicsSpace &space = game.physicsSpace();
    Vector2 minPos(1e9f, 1e9f);
    Vector2 maxPos(-1e9f, -1e9f);
    bool hasPoints = false;
    float staticTop = 1e9f; // The top of the shapes that don't move, like a scene's floor and walls

    for (int s = 0; s < space.shapes.size(); s++)
    {
        const Shape &shape = space.shapes[s];

        for (int i = shape.start; i < shape.end; i++)
        {
            Vector2 pos = space.points.pos[i];

            if (isnan(pos.x) || isnan(pos.y))
            {
                continue;
            }

            minPos = Vector2(min(minPos.x, pos.x), min(minPos.y, pos.y));
            maxPos = Vector2(max(maxPos.x, pos.x), max(maxPos.y, pos.y));
            hasPoints = true;

            if (shape.isStatic)
            {
                staticTop = min(staticTop, pos.y);
            }
        }
    }

    if (!hasPoints)
    {
        return;
    }

    float left = 0.0f;
    float top = toolbarBottom;
    float bottom = 0.0f;

    if (m->games.size() > 1)
    {
        top += levelTabsHeight;
    }

    if (m->showConsole)
    {
        bottom = consoleHeight + toolbarMargin;
    }

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    // Less margin on a small screen, like a phone's
    const float padding = min(40.0f, min(displaySize.x, displaySize.y) * 0.05f);
    float availableWidth = displaySize.x - left - padding * 2.0f;
    float availableHeight = displaySize.y - top - bottom - padding * 2.0f;

    // Too tall to fit even zoomed out as far as the view goes, and most likely because things start high above
    // the scene's floor and walls to fall in, as in Falling circles: fit what's from the top of those down,
    // so the view isn't zoomed far out, and let the rest fall into view.
    if (staticTop > minPos.y && staticTop < maxPos.y && availableHeight / (maxPos.y - minPos.y) < 0.25f)
    {
        minPos.y = staticTop;
    }

    Vector2 size = maxPos - minPos;

    const float fitScale = min(availableWidth / max(size.x, 1.0f), availableHeight / max(size.y, 1.0f));
    const float scale = clamp(fitScale, 0.25f, 1.5f);

    // Centered if it fits. A scene too tall to fit, even zoomed out as far as the view goes, is lined up with
    // the bottom instead, where its ground is and where things end up, as when boxes start stacked high above.
    float y = top + padding + (availableHeight - size.y * scale) * 0.5f - minPos.y * scale;

    if (size.y * scale > availableHeight)
    {
        y = top + padding + availableHeight - maxPos.y * scale;
    }

    game.scale() = scale;
    game.offset() = Vector2(left + padding + (availableWidth - size.x * scale) * 0.5f - minPos.x * scale, y);
}


// How the toolbar fits the screen. As it gets narrower, the scene list shrinks first, then the buttons drop
// their text, then the title goes, then the flask icon and some spacing, so it fits a phone held upright. The
// scene list takes whatever room is left, up to its full width.
struct ToolbarLayout
{
    bool iconOnly = false;
    bool showTitle = true;
    bool showFlask = true;
    float spacing = 0.0f;
    float sceneListWidth = 240.0f;
};

static const char *const fileLabel = ICON_FA_FOLDER_OPEN_O "  File";
static const char *const consoleLabel = ICON_FA_TERMINAL "  Console";
static const char *const settingsLabel = ICON_FA_COG "  Settings";
static const char *const playLabel = ICON_FA_PLAY "  Play";
static const char *const pauseLabel = ICON_FA_PAUSE "  Pause";
static const char *const resetLabel = ICON_FA_REFRESH "  Reset";

// The icon alone, the part of the label before its two spaces
static void iconOf(const char *label, char *icon, size_t size)
{
    const char *end = strstr(label, "  ");
    const size_t length = end != nullptr ? (size_t)(end - label) : strlen(label);
    snprintf(icon, size, "%.*s", (int)min(length, size - 1), label);
}

static float toolbarButtonWidth(const char *label, bool iconOnly)
{
    if (iconOnly)
    {
        // Square, and no smaller than a fingertip
        return max(ImGui::GetFrameHeight(), 36.0f);
    }

    return ImGui::CalcTextSize(label).x + ImGui::GetStyle().FramePadding.x * 2.0f;
}

// A toolbar button showing just its icon when the layout asks for it, with the full label as a tooltip
static bool toolbarButton(const char *label, float width, bool iconOnly)
{
    if (!iconOnly)
    {
        return ImGui::Button(label, ImVec2(width, 0.0f));
    }

    char icon[16];
    iconOf(label, icon, sizeof(icon));
    ImGui::PushID(label);
    // No side padding, or the icon is wider than the room left inside the square button and sits off center
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, ImGui::GetStyle().FramePadding.y));
    const bool pressed = ImGui::Button(icon, ImVec2(width, 0.0f));
    ImGui::PopStyleVar();
    ImGui::PopID();
    ImGui::SetItemTooltip("%s", label + strlen(icon) + 2);
    return pressed;
}

static ToolbarLayout layoutToolbar(float availableWidth, ImFont *titleFont)
{
    const float flaskWidth = ImGui::CalcTextSize(ICON_FA_FLASK).x;
    ImGui::PushFont(titleFont);
    const float titleWidth = ImGui::CalcTextSize("Physics Sandbox").x;
    ImGui::PopFont();
    const float titleGap = 28.0f;
    const float minSceneListWidth = 110.0f;

    // Widest first, each step giving up a little more
    for (int step = 0; step < 4; step++)
    {
        ToolbarLayout layout;
        layout.iconOnly = step >= 1;
        layout.showTitle = step < 2;
        layout.showFlask = step < 3;
        layout.spacing = step < 3 ? ImGui::GetStyle().ItemSpacing.x : 6.0f;

        float buttons = toolbarButtonWidth(fileLabel, layout.iconOnly) + toolbarButtonWidth(consoleLabel, layout.iconOnly) +
                        toolbarButtonWidth(settingsLabel, layout.iconOnly) + toolbarButtonWidth(resetLabel, layout.iconOnly) +
                        max(toolbarButtonWidth(playLabel, layout.iconOnly), toolbarButtonWidth(pauseLabel, layout.iconOnly));
        float used = buttons + layout.spacing * 5.0f;

        if (layout.showFlask)
        {
            used += flaskWidth + (layout.showTitle ? layout.spacing + titleWidth + titleGap : titleGap);
        }

        layout.sceneListWidth = min(availableWidth - used, 240.0f);

        if (layout.sceneListWidth >= minSceneListWidth || step == 3)
        {
            layout.sceneListWidth = max(layout.sceneListWidth, 40.0f);
            return layout;
        }
    }

    return ToolbarLayout();
}

void Editor::renderToolbar(Game &game, ImFont *titleFont)
{
    const float margin = toolbarMargin;
    const float barHeight = toolbarHeight;
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    // View settings, so they follow the editor rather than the scene or level that is open
    game.debugDraw() = m->debugDraw;
    game.antiAliasing() = m->antiAliasing;

    // Refit when the screen changes size, as when a phone is turned, so the scene doesn't end up off to one side
    const ImVec2 currentDisplaySize = ImGui::GetIO().DisplaySize;

    if (currentDisplaySize.x != m->lastDisplaySize.x || currentDisplaySize.y != m->lastDisplaySize.y)
    {
        m->lastDisplaySize = currentDisplaySize;
        m->fitViewPending = true;
    }

    if (m->fitViewPending)
    {
        fitViewToScene(game);
        m->fitViewPending = false;
    }

    const ImVec4 white = overlayWhite;
    const ImVec4 green = overlayGreen;

    pushOverlayStyle();

    ImGui::SetNextWindowPos(ImVec2(margin, margin));
    ImGui::SetNextWindowSize(ImVec2(displaySize.x - margin * 2.0f, barHeight));

    if (ImGui::Begin("Demo toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse))
    {
        ImGuiStyle &style = ImGui::GetStyle();
        const float frameTop = (barHeight - ImGui::GetFrameHeight()) * 0.5f;
        const char *currentSceneName = game.currentSceneName();
        const ToolbarLayout layout = layoutToolbar(ImGui::GetContentRegionAvail().x, titleFont);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(layout.spacing, style.ItemSpacing.y));

        if (layout.showFlask)
        {
            ImGui::SetCursorPosY((barHeight - ImGui::GetTextLineHeight()) * 0.5f);
            ImGui::TextColored(ImVec4(0.95f, 0.65f, 0.18f, 1.0f), ICON_FA_FLASK);

            if (layout.showTitle)
            {
                ImGui::SameLine();
                ImGui::PushFont(titleFont);
                ImGui::SetCursorPosY((barHeight - ImGui::GetTextLineHeight()) * 0.5f);
                ImGui::TextUnformatted("Physics Sandbox");
                ImGui::PopFont();
            }

            ImGui::SameLine(0.0f, 28.0f);
        }

        ImGui::SetCursorPosY(frameTop);
        ImGui::SetNextItemWidth(layout.sceneListWidth);

        // The toolbar has no vertical padding, but the dropdown list should
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 10.0f));
        const bool comboOpen = ImGui::BeginCombo("##scene", currentSceneName != nullptr ? currentSceneName : "Choose a scene", ImGuiComboFlags_HeightLargest);
        ImGui::PopStyleVar();

        if (comboOpen)
        {
            // The demo folders are listed in full, the same as in the web demo
            for (int i = 0; i < SceneDefinitionFolder::numFolders; i++)
            {
                const SceneDefinitionFolder &folder = SceneDefinitionFolder::allFolders[i];

                if (folder.hiddenInWebDemo)
                {
                    continue;
                }

                ImGui::SeparatorText(folder.name);

                for (int j = 0; j < folder.numScenes; j++)
                {
                    const SceneDefinition &scene = folder.scenes[j];
                    const bool isSelected = currentSceneName != nullptr && strcmp(scene.name, currentSceneName) == 0;

                    if (ImGui::Selectable(scene.name, isSelected))
                    {
                        loadScene(scene);
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }

#ifndef __EMSCRIPTEN__
            // The debugging folders are long, so they open as submenus
            ImGui::SeparatorText("Debugging");

            for (int i = 0; i < SceneDefinitionFolder::numFolders; i++)
            {
                const SceneDefinitionFolder &folder = SceneDefinitionFolder::allFolders[i];

                if (!folder.hiddenInWebDemo)
                {
                    continue;
                }

                // Submenus are windows of their own and would get the toolbar's zero vertical padding
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 10.0f));
                const bool folderOpen = ImGui::BeginMenu(folder.name);
                ImGui::PopStyleVar();

                if (!folderOpen)
                {
                    continue;
                }

                for (int j = 0; j < folder.numScenes; j++)
                {
                    const SceneDefinition &scene = folder.scenes[j];
                    const bool isSelected = currentSceneName != nullptr && strcmp(scene.name, currentSceneName) == 0;

                    if (ImGui::MenuItem(scene.name, nullptr, isSelected))
                    {
                        loadScene(scene);
                    }
                }

                ImGui::EndMenu();
            }
#endif

            ImGui::EndCombo();
        }

        renderFileMenu(frameTop, game, layout.iconOnly);

        // Console, settings and playback controls, right aligned
        const float consoleWidth = toolbarButtonWidth(consoleLabel, layout.iconOnly);
        const float settingsWidth = toolbarButtonWidth(settingsLabel, layout.iconOnly);
        const float playWidth = max(toolbarButtonWidth(playLabel, layout.iconOnly), toolbarButtonWidth(pauseLabel, layout.iconOnly));
        const float resetWidth = toolbarButtonWidth(resetLabel, layout.iconOnly);

        ImGui::SameLine(ImGui::GetWindowWidth() - style.WindowPadding.x - consoleWidth - settingsWidth - playWidth - resetWidth - style.ItemSpacing.x * 3.0f);
        ImGui::SetCursorPosY(frameTop);

        // Highlighted while the console is shown
        const bool consoleShown = m->showConsole;

        if (consoleShown)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.87f, 0.93f, 0.98f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.62f, 0.78f, 0.93f, 1.0f));
        }

        if (toolbarButton(consoleLabel, consoleWidth, layout.iconOnly))
        {
            m->showConsole = !m->showConsole;
            m->fitViewPending = true;
            saveState();
        }

        if (consoleShown)
        {
            ImGui::PopStyleColor(2);
        }

        ImGui::SameLine();
        ImGui::SetCursorPosY(frameTop);

        if (toolbarButton(settingsLabel, settingsWidth, layout.iconOnly))
        {
            ImGui::OpenPopup("Settings");
        }

        // Open the settings below the button, right aligned with it
        const ImVec2 settingsButtonMax = ImGui::GetItemRectMax();
        ImGui::SetNextWindowPos(ImVec2(settingsButtonMax.x, settingsButtonMax.y + 8.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 14.0f));
        const bool settingsOpen = ImGui::BeginPopup("Settings");
        ImGui::PopStyleVar();

        if (settingsOpen)
        {
            // Compact controls, the toolbar's frame padding is sized for its buttons
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 8.0f));
            renderSettings(game);
            ImGui::PopStyleVar(3);
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::SetCursorPosY(frameTop);
        ImGui::PushStyleColor(ImGuiCol_Button, green);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.63f, 0.43f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.22f, 0.56f, 0.38f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, green);
        ImGui::PushStyleColor(ImGuiCol_Text, white);

        if (toolbarButton(game.paused() ? playLabel : pauseLabel, playWidth, layout.iconOnly))
        {
            game.togglePaused();
        }

        ImGui::PopStyleColor(5);
        ImGui::SameLine();
        ImGui::SetCursorPosY(frameTop);

        if (toolbarButton(resetLabel, resetWidth, layout.iconOnly) && currentSceneName != nullptr)
        {
            const SceneDefinition *scene = SceneDefinition::getDefinitionFromName(currentSceneName);

            if (scene != nullptr)
            {
                loadScene(*scene);
            }
        }

        ImGui::PopStyleVar();
    }

    ImGui::End();
    popOverlayStyle();
}

void Editor::renderSettings(Game &game)
{
    PhysicsSpace &space = game.physicsSpace();

    ImGui::SeparatorText("Simulation");
    ImGui::Checkbox("Gravity", &space.gravityEnabled);
    ImGui::Checkbox("Collisions", &space.collisionsEnabled);
    ImGui::Checkbox("Shape matching", &space.shapeMatchingEnabled);
    ImGui::Checkbox("Springs", &space.springsEnabled);
    ImGui::Checkbox("Depenetrate along normal", &depenetrateAlongNormal);

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Off restores the legacy normalize(v - 2n) push-out, which\n"
                          "inverts above |v| = 2. Compare with \"Falling box with shelf\".");
    }

    ImGui::SetNextItemWidth(160.0f);
    ImGui::DragInt("Speed", &game.simulationSpeed(), 1.0, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp);

    ImGui::SeparatorText("View");

    if (ImGui::Checkbox("Debug draw", &m->debugDraw))
    {
        game.debugDraw() = m->debugDraw;
        saveState();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Draw the simulation's debug view: points, edges and\n"
                          "shape matching targets, instead of the styled look.");
    }

    if (ImGui::Checkbox("Anti-aliasing", &m->antiAliasing))
    {
        game.antiAliasing() = m->antiAliasing;
        saveState();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Smooth edges on lines and outlines.");
    }

    if (ImGui::Checkbox("Show profiler", &m->showProfiler))
    {
        saveState();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Frame timings and the step, rewind and forward controls.");
    }

    renderShapeSettings(game);
}

// Properties of the selected shape and the point being dragged
void Editor::renderShapeSettings(Game &game)
{
    PhysicsSpace &space = game.physicsSpace();
    const int selectedShapeIndex = game.selectedShapeIndex();
    const bool hasSelection = selectedShapeIndex >= 0 && selectedShapeIndex < space.shapes.size();
    const bool dragging = space.mouseJoint.pointIndex != -1;

    if (!hasSelection && !dragging)
    {
        return;
    }

    ImGui::SeparatorText("Selected shape");

    if (dragging)
    {
        const int pointIndex = space.mouseJoint.pointIndex;
        ImGui::Text("Dragged point: %d", pointIndex);
        ImGui::Text("Position: %.2f, %.2f", space.points.pos[pointIndex].x, space.points.pos[pointIndex].y);
        ImGui::Text("Velocity: %.2f, %.2f", space.points.velocity[pointIndex].x, space.points.velocity[pointIndex].y);
        ImGui::Text("Mass: %.2f", space.points.mass[pointIndex]);
    }

    if (!hasSelection)
    {
        return;
    }

    Shape &shape = space.shapes[selectedShapeIndex];
    ImGui::TextDisabled("Shape %d, parent %d", selectedShapeIndex, shape.parentId);
    ImGui::Checkbox("Static", &shape.isStatic);

    ImGui::SetNextItemWidth(160.0f);

    if (ImGui::BeginCombo("Resource", shapeResourceName(shape.resourceId)))
    {
        for (int i = 0; i < NUM_SHAPE_RESOURCES; i++)
        {
            const bool isSelected = shape.resourceId == i;

            if (ImGui::Selectable(shapeResourceName(i), isSelected))
            {
                shape.resourceId = i;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    if (ImGui::Button("Snap to grid"))
    {
        Shapes::snapToGrid(space, selectedShapeIndex);
    }

    ImGui::SameLine();

    if (ImGui::Button("Update original position"))
    {
        Shapes::updateOriginalPos(space, selectedShapeIndex);
        space.triangulate();
    }
}

// Restart, rewind, play/pause, single step and forward. Held buttons repeat.
void Editor::renderStepperButtons(Game &game, ConsoleProfileInfo &profileInfo, ImVec2 buttonSize)
{
    const char *currentSceneName = game.currentSceneName();
    ImGui::PushButtonRepeat(true);

    // Mirrored around play/pause: history rewind and forward next to it, restart and single step at the ends
    if (ImGui::Button(ICON_FA_STEP_BACKWARD, buttonSize) && currentSceneName != nullptr)
    {
        // A level file has no scene definition to restart
        const SceneDefinition *scene = SceneDefinition::getDefinitionFromName(currentSceneName);

        if (scene != nullptr)
        {
            game.init(*scene);
        }
    }

    ImGui::SetItemTooltip("Restart the scene");
    ImGui::SameLine();

    if (ImGui::Button(ICON_FA_BACKWARD, buttonSize))
    {
        game.setPaused();
        game.rewindHistory();
    }

    ImGui::SetItemTooltip("Rewind through history");
    ImGui::SameLine();

    if (ImGui::Button(game.paused() ? ICON_FA_PLAY : ICON_FA_PAUSE, buttonSize))
    {
        game.togglePaused();
    }

    ImGui::SetItemTooltip(game.paused() ? "Play" : "Pause");
    ImGui::SameLine();

    if (ImGui::Button(ICON_FA_FORWARD, buttonSize))
    {
        game.setPaused();
        game.forwardHistory();
    }

    ImGui::SetItemTooltip("Forward through history");
    ImGui::SameLine();

    if (ImGui::Button(ICON_FA_STEP_FORWARD, buttonSize))
    {
        game.update(1000.0 / 120.0, true, profileInfo);
    }

    ImGui::SetItemTooltip("Simulate a single step");

    ImGui::PopButtonRepeat();
}

void Editor::renderProfilerStats(Game &game, const ConsoleProfileInfo &profileInfo)
{
    ImGui::Text("FPS: %.1lf", profileInfo.displayedFps);
    ImGui::Text("Raw frame time: %.2lf ms", profileInfo.rawFrameTimeMillis);
    ImGui::Text("Displayed frame time: %.2lf ms", profileInfo.displayedFrameTimeMillis);
    if (profileInfo.frameTimeSnappingEnabled)
    {
        ImGui::Text("Tick mode: fixed target");
        ImGui::Text("Tick target: %.1lf Hz", profileInfo.targetTickRate);
        ImGui::Text("Tick frame time: %.2lf ms", profileInfo.targetFrameTimeMillis);
    }
    else
    {
        ImGui::Text("Tick mode: display refresh");
        ImGui::Text("Tick target: raw requestAnimationFrame delta");
    }

    ImGui::Text("Frame snap: %s", profileInfo.frameTimeSnappingEnabled ? "enabled" : "disabled");
    ImGui::Text("Slowdown factor: %.1lf", profileInfo.slowdownFactor);
    ImGui::Text("Total Physics time: %.1lf ms", profileInfo.totalPhysicsTimeMillis);
    ImGui::Text("Elapsed step time: %.1lf ms", profileInfo.elapsedStepTimeMillis);
    ImGui::Text("Physics iterations: %d", profileInfo.numPhysicsSteps);
    ImGui::Text("Physics time: %.1lf ms", profileInfo.physicsTimeMillis);
    ImGui::Text("Render time: %.1lf ms", profileInfo.renderTimeMillis);
    ImGui::Text("Swap time: %.1lf ms", profileInfo.swapTimeMillis);
    ImGui::Text("Springs time: %.1lf ms", profileInfo.springsTimeMillis);
    ImGui::Text("Bounding box time: %.1lf ms", profileInfo.boundingBoxTimeMillis);
    ImGui::Text("Num bboxes: %d", profileInfo.numBboxes);
    ImGui::Text("Num bbox checks: %d", profileInfo.numBbboxChecks);
    ImGui::Text("Num bbox overlaps: %d", profileInfo.numBboxOverlaps);
    ImGui::Text("Num circle rejections: %d", profileInfo.numCircleRejections);
    ImGui::Text("Num intersections: %d", profileInfo.numIntersections);
    ImGui::Text("Num collisions: %d", profileInfo.numCollisions);
    ImGui::Text("Collisions time: %.1lf ms", profileInfo.collisionTimeMillis);
    ImGui::Text("Collision resolve: %.1lf ms", profileInfo.collisionHandlingTimeMillis);
    ImGui::Text("Collision grid: %.1lf ms", profileInfo.collisionGridUpdateTimeMillis);

    PhysicsSpace &space = game.physicsSpace();
    int groundedWheelMotors = 0;
    for (int i = 0; i < space.wheelMotors.size(); i++)
    {
        if (space.wheelMotors[i].groundedThisStep)
        {
            groundedWheelMotors++;
        }
    }

    ImGui::Separator();
    ImGui::Text("Wheel motors: %d", space.wheelMotors.size());
    ImGui::Text("Grounded motors: %d", groundedWheelMotors);

    for (int i = 0; i < space.wheelMotors.size(); i++)
    {
        const WheelMotor &wheelMotor = space.wheelMotors[i];
        if (wheelMotor.shapeIndex < 0 || wheelMotor.shapeIndex >= space.shapes.size())
        {
            continue;
        }

        const Shape &shape = space.shapes[wheelMotor.shapeIndex];
        float radiusRatio = wheelRadiusRatio(space, shape);

        ImGui::Text("Wheel %d: cmd %.1f mode %s grounded %s (%d)",
                    wheelMotor.shapeIndex,
                    wheelMotor.command,
                    wheelMotorModeName(wheelMotor.lastMode),
                    wheelMotor.groundedThisStep ? "yes" : "no",
                    wheelMotor.groundedContactCount);
        ImGui::Text("surface %.3f err %.3f radius %.2f impulse %.3f",
                    wheelMotor.lastSurfaceSpeed,
                    wheelMotor.lastSurfaceSpeedError,
                    radiusRatio,
                    wheelMotor.lastAppliedImpulse);
        ImGui::Text("parent %.3f ground %.3f rel %.3f",
                    wheelMotor.lastParentForwardSpeed,
                    wheelMotor.lastGroundSpeed,
                    wheelMotor.lastRelativeForwardSpeed);
        ImGui::Text("cmd-space %.3f clamp %.3f band %.3f",
                    wheelMotor.lastCommandSpaceSpeed,
                    wheelMotor.lastAuthorityClamp,
                    wheelMotor.lastHandoverBand);
    }
}

// The current scene's own controls, if it has any, in a panel that starts out folded to its title at the top
// left below the toolbar and can be dragged anywhere
void Editor::renderSceneControls(Game &game)
{
    SceneScript *script = game.script();

    if (script == nullptr || !script->hasControls())
    {
        return;
    }

    const float width = 360.0f;
    const float top = toolbarBottom + 10.0f + (m->games.size() > 1 ? levelTabsHeight : 0.0f);

    pushOverlayStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 14.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 5.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 6.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, overlayGreen);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, overlayGreen);

    ImGui::SetNextWindowPos(ImVec2(toolbarMargin, top), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(width, 0.0f), ImVec2(width, ImGui::GetIO().DisplaySize.y - top - toolbarMargin));

    if (ImGui::Begin("Scene controls", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        const float toggleWidth = ImGui::GetFrameHeight();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(ICON_FA_SLIDERS "  Scene settings");
        ImGui::SameLine(ImGui::GetContentRegionMax().x - toggleWidth);

        const bool toggled = ImGui::Button("##toggle scene controls", ImVec2(toggleWidth, toggleWidth));

        // A chevron drawn with lines, as the icon font's is too big for the button
        {
            const ImVec2 center((ImGui::GetItemRectMin().x + ImGui::GetItemRectMax().x) * 0.5f, (ImGui::GetItemRectMin().y + ImGui::GetItemRectMax().y) * 0.5f);
            const float halfWidth = toggleWidth * 0.17f;
            const float halfHeight = halfWidth * 0.5f;
            const float pointing = m->sceneControlsCollapsed ? 1.0f : -1.0f; // Down when folded, up when open
            const ImVec2 points[3] = {ImVec2(center.x - halfWidth, center.y - halfHeight * pointing),
                                      ImVec2(center.x, center.y + halfHeight * pointing),
                                      ImVec2(center.x + halfWidth, center.y - halfHeight * pointing)};
            ImGui::GetWindowDrawList()->AddPolyline(points, 3, ImGui::GetColorU32(ImGuiCol_Text), ImDrawFlags_None, 2.0f);
        }

        if (toggled)
        {
            m->sceneControlsCollapsed = !m->sceneControlsCollapsed;
        }

        if (!m->sceneControlsCollapsed)
        {
            ImGui::PushItemWidth(width * 0.5f);
            script->drawControls(game);
            ImGui::PopItemWidth();
        }
    }

    ImGui::End();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
    popOverlayStyle();
}

void Editor::renderProfilerOverlay(Game &game, ConsoleProfileInfo &profileInfo)
{
    if (!m->showProfiler)
    {
        return;
    }

    const float width = 340.0f;
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    pushOverlayStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 14.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 6.0f));

    // Starts at the top right below the toolbar and can be dragged anywhere
    ImGui::SetNextWindowPos(ImVec2(displaySize.x - toolbarMargin, toolbarBottom + 10.0f), ImGuiCond_FirstUseEver, ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(width, 0.0f), ImVec2(width, displaySize.y - toolbarBottom - toolbarMargin * 2.0f));

    if (ImGui::Begin("Profiler overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        const ImGuiStyle &style = ImGui::GetStyle();
        const float closeWidth = ImGui::GetFrameHeight();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(ICON_FA_TACHOMETER "  Profiler");
        ImGui::SameLine(ImGui::GetContentRegionMax().x - closeWidth);

        if (ImGui::Button(ICON_FA_TIMES, ImVec2(closeWidth, 0.0f)))
        {
            m->showProfiler = false;
            saveState();
        }

        // Five equal buttons across the panel
        const float buttonWidth = (ImGui::GetContentRegionAvail().x - style.ItemSpacing.x * 4.0f) / 5.0f;
        renderStepperButtons(game, profileInfo, ImVec2(buttonWidth, 34.0f));

        ImGui::Spacing();

        if (ImGui::BeginTable("Profiler summary", 2, ImGuiTableFlags_SizingStretchProp))
        {
            const char *labels[] = {"FPS", "Frame time", "Physics", "Collisions", "Render"};
            const double values[] = {profileInfo.displayedFps,
                                     profileInfo.displayedFrameTimeMillis,
                                     profileInfo.physicsTimeMillis,
                                     profileInfo.collisionTimeMillis,
                                     profileInfo.renderTimeMillis};

            for (int i = 0; i < 5; i++)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextDisabled("%s", labels[i]);
                ImGui::TableNextColumn();

                if (i == 0)
                {
                    ImGui::Text("%.1lf", values[i]);
                }
                else
                {
                    ImGui::Text("%.2lf ms", values[i]);
                }
            }

            ImGui::EndTable();
        }

        // A quiet header, the blue selection color is too strong for a section toggle
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.97f, 0.96f, 0.93f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.95f, 0.93f, 0.89f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.93f, 0.90f, 0.86f, 1.0f));
        const bool detailsOpen = ImGui::CollapsingHeader("Details");
        ImGui::PopStyleColor(3);

        if (detailsOpen)
        {
            renderProfilerStats(game, profileInfo);
        }
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
    popOverlayStyle();
}

// File dropdown in the toolbar
void Editor::renderFileMenu(float frameTop, Game &game, bool iconOnly)
{
    const char *id = "File";
    ImGui::SameLine();
    ImGui::SetCursorPosY(frameTop);

    if (toolbarButton(fileLabel, toolbarButtonWidth(fileLabel, iconOnly), iconOnly))
    {
        ImGui::OpenPopup(id);
    }

    const ImVec2 buttonMin = ImGui::GetItemRectMin();
    const ImVec2 buttonMax = ImGui::GetItemRectMax();
    ImGui::SetNextWindowPos(ImVec2(buttonMin.x, buttonMax.y + 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 10.0f));
    const bool open = ImGui::BeginPopup(id);
    ImGui::PopStyleVar();

    if (!open)
    {
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));

    if (ImGui::MenuItem("New", "Ctrl+N/Meta+N"))
    {
        m->openAddLevelPopup = true;
    }

    // The submenu is a window of its own and would get the toolbar's zero vertical padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 10.0f));
    const bool openMenu = ImGui::BeginMenu("Open");
    ImGui::PopStyleVar();

    if (openMenu)
    {
        for (int i = 0; i < m->fileList.size(); i++)
        {
            if (ImGui::MenuItem(m->fileList[i].name.data))
            {
                m->lastOpenedFile.clear();
                m->lastOpenedFile.append(m->fileList[i].name.data);

                Game *newGame = new Game(m->appPath, m->lastOpenedFile.data);
                m->games.push(newGame);
                setCurrentGameIndex(m->games.size() - 1);
                m->openedBuffers.push({m->lastOpenedFile});
                saveState();
                StringBuffer<512> fullPath;
                fullPath.append("%s/%s", m->appPath, m->lastOpenedFile.data);
                PhysicsSpaceStorage::loadFromFile(newGame->physicsSpace(), fullPath.data);
                m->fitViewPending = true;
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

    ImGui::PopStyleVar(2);
    ImGui::EndPopup();
}
