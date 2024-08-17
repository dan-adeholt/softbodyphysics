#ifndef __SCENES__H
#define __SCENES__H

typedef void (*SceneInitFunction)(class Game *);

struct SceneDefinition
{
    const char *name;
    SceneInitFunction initFunc;

    static int numScenes;
    static SceneDefinition *allScenes;
    static SceneDefinition *getDefinitionFromName(const char *name);
};

#endif