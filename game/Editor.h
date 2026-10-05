#ifndef __EDITOR_H__
#define __EDITOR_H__

struct ConsoleProfileInfo;
struct ImFont;
struct ImVec2;
struct SceneDefinition;
class Game;

class Editor
{
public:
    Editor(const char *appPath);
    ~Editor();

    // Toolbar, level tabs, popups, profiler and console
    void renderUI(Game &game, ConsoleProfileInfo &profileInfo, ImFont *titleFont, ImFont *boldFont);

    void saveState();

    const char *lastSceneName();

    void readIniValue(const char *section, const char *name, const char *value);

    Game *getCurrentGame();

private:
    void setCurrentGameIndex(int index);
    void loadScene(const SceneDefinition &scene);
    void fitViewToScene(Game &game);

    void renderToolbar(Game &game, ImFont *titleFont);
    void renderFileMenu(float frameTop, Game &game, bool iconOnly);
    void renderLevelTabs();
    void renderCanvasPopups(Game &game);
    void updateCanvasHover(Game &game);
    void renderDragHint(Game &game);
    void renderSettings(Game &game);
    void renderShapeSettings(Game &game);

    // Floating profiler with the step, rewind and forward controls, toggled in the settings
    void renderProfilerOverlay(Game &game, ConsoleProfileInfo &profileInfo);
    void renderSceneControls(Game &game);
    void renderStepperButtons(Game &game, ConsoleProfileInfo &profileInfo, ImVec2 buttonSize);
    void renderProfilerStats(Game &game, const ConsoleProfileInfo &profileInfo);

    struct Impl;
    Impl *m;
};

#endif