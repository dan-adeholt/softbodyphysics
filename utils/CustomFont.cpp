#include "CustomFont.h"
#include <stdio.h>
#include <string.h>
#include "imgui.h"
#include "../stb_image/stb_image.h"
#include "../containers/Array.h"

struct CharData
{
    ImWchar id = 0;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int xoffset = 0;
    int yoffset = 0;
    int xadvance = 0;
    int page = 0;
    int chnl = 0;
    int rectId = 0;
};

void CustomFont::load(Range<CustomFontEntry> fonts)
{
    ImGuiIO &io = ImGui::GetIO();
    Array<Array<CharData>> allCharDatas;

    for (int i = 0; i < fonts.size; i++)
    {
        CustomFontEntry entry = fonts[i];

        FILE *f = fopen(entry.path, "r");

        if (!f)
        {
            printf("Failed to open %s\n", entry.path);
            return;
        }

        Array<CharData> charDataArray;
        // Read the font file line by line, delimiter \n
        char line[256];
        while (fgets(line, sizeof(line), f))
        {
            if (strstr(line, "char ") == line)
            {

                CharData charData = {};
                sscanf(line, "char id=%hu   x=%d     y=%d     width=%d     height=%d     xoffset=%d     yoffset=%d    xadvance=%d     page=%d  chnl=%d \n",
                       &charData.id, &charData.x, &charData.y, &charData.width, &charData.height, &charData.xoffset, &charData.yoffset, &charData.xadvance, &charData.page, &charData.chnl);
                if (charData.width > 0 && charData.height > 0)
                {
                    charData.yoffset += entry.fixedYOffset;
                    charData.rectId = io.Fonts->AddCustomRectFontGlyph(entry.font, (ImWchar)charData.id, charData.width, charData.height, charData.xadvance, ImVec2(charData.xoffset, charData.yoffset));
                    charDataArray.push(charData);
                }
            }
        }

        allCharDatas.push(charDataArray);
        fclose(f);
    }

    io.Fonts->Build();

    unsigned char *tex_pixels = nullptr;
    int tex_width, tex_height;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);

    unsigned char *tex_pixels_write = tex_pixels;

    for (int i = 0; i < fonts.size; i++)
    {
        CustomFontEntry entry = fonts[i];
        Array<CharData> charDataArray = allCharDatas[i];
        // Variables to store image dimensions and number of channels
        int width, height, channels;

        // Load the image as an RGBA buffer
        unsigned char *imageData = stbi_load(entry.imagePath, &width, &height, &channels, STBI_rgb_alpha);
        if (!imageData)
        {
            printf("Failed to load image: %s\n", stbi_failure_reason());
            return;
        }

        unsigned int *imageDataPixels = (unsigned int *)imageData;
        for (int j = 0; j < charDataArray.size(); j++)
        {
            CharData charData = charDataArray[j];
            if (const ImFontAtlasCustomRect *rect = io.Fonts->GetCustomRectByIndex(charData.rectId))
            {
                // Fill the custom rectangle with red pixels (in reality you would draw/copy your bitmap data here!)
                for (int y = 0; y < rect->Height; y++)
                {
                    ImU32 *p = (ImU32 *)tex_pixels + (rect->Y + y) * tex_width + (rect->X);
                    for (int x = 0; x < rect->Width; x++)
                    {
                        *p++ = imageDataPixels[(charData.y + y) * width + (charData.x + x)];
                    }
                }
            }
        }
        stbi_image_free(imageData);
    }
}