#ifndef __TEXTURES_H__
#define __TEXTURES_H__

enum TextureType
{
    CLOTH_BASIC = 0,
    CLOTH_REFINED = 1,
    CLOTH_REFINED_2 = 2,
    ROCK = 3,
    GRASS = 4,
    NUM_TEXTURE_TYPES
};

struct TextureData
{
    TextureType type;
    const char *path;
    float width;
    float height;
};

extern TextureData textureLookup[NUM_TEXTURE_TYPES];

#endif