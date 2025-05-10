#include "Textures.h"

TextureData textureLookup[TextureType::NUM_TEXTURE_TYPES] = {
    {TextureType::CLOTH_BASIC, "./data/cloth_refined.png", 128.0f, 128.0f},
    {TextureType::CLOTH_REFINED, "./data/cloth_refined.png", 256.0f, 256.0f},
    {TextureType::CLOTH_REFINED_2, "./data/cloth_refined2.png", 256.0f, 256.0f},
    {TextureType::ROCK, "./data/rock.png", 256.0f, 256.0f},
    {TextureType::GRASS, "./data/grass.png", 256.0f, 256.0f},
};