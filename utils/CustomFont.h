#ifndef __CUSTOM_FONT_H
#define __CUSTOM_FONT_H

#include "../containers/Range.h"

struct ImFont;

struct CustomFontEntry
{
    ImFont *font;
    const char *path;
    const char *imagePath;
    int fixedYOffset;
};

namespace CustomFont
{
    void load(Range<CustomFontEntry> fonts);
}

#endif