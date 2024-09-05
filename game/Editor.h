#ifndef __EDITOR_H__
#define __EDITOR_H__

struct ConsoleProfileInfo;
class Game;

class Editor
{
public:
    Editor();
    ~Editor();
    void renderUI(Game &game, ConsoleProfileInfo &profileInfo);

    void saveState();

    bool executingTest();

    void stepTest(Game *game, double elapsedMilliseconds, ConsoleProfileInfo &profileInfo);

    void readIniValue(const char *section, const char *name, const char *value);
private:
    struct Impl;
    Impl *m;
};

#endif