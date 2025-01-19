#ifndef __SCENES__H
#define __SCENES__H

typedef void (*SceneInitFunction)(class Game *);

struct SceneDefinition;

struct SceneDefinition
{
    const char *name;
    SceneInitFunction initFunc;

    static SceneDefinition *getDefinitionFromName(const char *name);
};

struct SceneDefinitionFolder
{
    const char *name;
    SceneDefinition *scenes;
    int numScenes;

    static SceneDefinitionFolder *allFolders;
    static int numFolders;
};

#endif