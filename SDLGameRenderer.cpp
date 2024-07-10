#include "./SDLGameRenderer.h"
#include <SDL.h>
#include "containers/Range.h"
#include "game/Physics.h"
#include "game/Game.h"
#include "data/BitmapFont.inl.h"
#include "containers/Array.h"
#include "utils/Console.h"

float MIN_LINE_POS = -100000;
float MAX_LINE_POS = 1000000;

struct SDLGameRenderer::Impl
{
    Impl() : texture(nullptr)
    {
    }

    SDL_Texture *texture;
};

SDLGameRenderer::SDLGameRenderer()
{
    m = new Impl();
}

SDLGameRenderer::~SDLGameRenderer()
{
    delete m;
}

void SDLGameRenderer::renderText(SDL_Renderer *renderer, const char *text, int x, int y)
{
    SDL_Color color = {255, 255, 255, 255};
    if (m->texture == nullptr)
    {
        Array<uint32_t> rgba32(CHARS_WIDTH * CHARS_HEIGHT);
        for (int i = 0; i < CHARS_WIDTH * CHARS_HEIGHT; i++)
        {
            uint32_t color = BITMAP_FONT[i];
            uint8_t alpha = color & 0xFF;
            uint8_t r = 255 - (color >> 24) & 0xFF;
            uint8_t g = 255 - (color >> 16) & 0xFF;
            uint8_t b = 255 - (color >> 8) & 0xFF;
            color = (r << 24) | (g << 16) | (b << 8) | alpha;

            rgba32.push(color);
        }

        m->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STATIC, CHARS_WIDTH, CHARS_HEIGHT);
        SDL_UpdateTexture(m->texture, NULL, &rgba32[0], CHARS_WIDTH * sizeof(Uint32));

        SDL_SetTextureBlendMode(m->texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(m->texture, 255); // Full opacity
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_FRect destRect;
    destRect.x = x;
    destRect.y = y;
    destRect.w = 8.0f;
    destRect.h = 15.0f;

    size_t len = strlen(text);
    for (int index = 0; index < len; index++)
    {
        char c = text[index];
        if (c == '\n')
        {
            destRect.x = x;
            destRect.y += 16.0f;
            continue;
        }
        const BitmapTexCoord &texCoord = bitmapTexCoords[c];
        SDL_Rect srcRect;
        srcRect.x = texCoord.x;
        srcRect.y = texCoord.y;
        srcRect.w = 8.0;
        srcRect.h = 15.0;

        int charIndex = c - 32.0f;
        destRect.x += 8.0f;
        SDL_RenderCopyF(renderer, m->texture, &srcRect, &destRect);
    }
}

void renderSprings(SDL_Renderer *renderer, Range<Spring> springs, const Range<PointMass> &points)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    for (int i = 0; i < springs.size; i++)
    {
        const Spring &spring = springs.data[i];
        const PointMass &pointA = points[spring.pointA];
        const PointMass &pointB = points[spring.pointB];
        float currentLength = (pointA.pos - pointB.pos).length();
        float tension = fabs(1.0f - (spring.length / currentLength));
        // SDL_SetRenderDrawColor(renderer, 0, (int)(tension * 100), (int)(tension * 100), 255);
        SDL_SetRenderDrawColor(renderer, 200.0f * tension, 0, 0, 255);

        // To prevent SDL taking extremely long to render degenerate lines
        if (pointA.pos.x > MIN_LINE_POS && pointA.pos.x < MAX_LINE_POS &&
            pointA.pos.y > MIN_LINE_POS && pointA.pos.y < MAX_LINE_POS &&
            pointB.pos.x > MIN_LINE_POS && pointB.pos.x < MAX_LINE_POS &&
            pointB.pos.y > MIN_LINE_POS && pointB.pos.y < MAX_LINE_POS)
        {
            Vector2 direction = (pointB.pos - pointA.pos);
            Vector2 ortho = Vector2(-direction.y, direction.x).normalized();

            Vector2 prevPoint = pointA.pos;

            float numSteps = spring.length / 2.0f;
            float springSize = 1.0f;
            for (int i = 0; i <= numSteps; i++)
            {
                float relPos = i / numSteps;

                Vector2 pos = pointA.pos + (direction * relPos);
                float sign = (i % 2 == 0) ? 1.0f : -1.0f;
                Vector2 springPos = pos + ortho * springSize * sign;

                SDL_RenderDrawLineF(renderer, prevPoint.x, prevPoint.y, springPos.x, springPos.y);
                prevPoint = springPos;
            }
            // SDL_RenderDrawLine(renderer, pointA.pos.x, pointA.pos.y, pointB.pos.x, pointB.pos.y);
        }
    }
}

void renderPointMasses(SDL_Renderer *renderer, Range<PointMass> pointMasses)
{
    SDL_FRect rectangle;
    rectangle.h = 4.0f;
    rectangle.w = 4.0f;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    for (int i = 0; i < pointMasses.size; i++)
    {
        rectangle.x = pointMasses.data[i].pos.x - 2;
        rectangle.y = pointMasses.data[i].pos.y - 2;
        SDL_RenderFillRectF(renderer, &rectangle);
    }
}

void renderShapes(SDL_Renderer *renderer, Range<Shape> shapes, Range<PointMass> pointMasses)
{
    SDL_Rect rectangle;
    rectangle.h = 4;
    rectangle.w = 4;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    for (int i = 0; i < shapes.size; i++)
    {
        const Shape &shape = shapes.data[i];
        PointMass firstPoint = pointMasses[shape.start];
        PointMass p0 = firstPoint;

        for (int pointIndex = shape.start + 1; pointIndex < shape.end; pointIndex++)
        {
            const PointMass &p1 = pointMasses[pointIndex];
            SDL_RenderDrawLineF(renderer, p0.pos.x, p0.pos.y, p1.pos.x, p1.pos.y);
            p0 = p1;
        }

        SDL_RenderDrawLineF(renderer, p0.pos.x, p0.pos.y, firstPoint.pos.x, firstPoint.pos.y);
    }
}

void SDLGameRenderer::render(SDL_Renderer *renderer, Game *game)
{
    Range<Shape> shapes;
    Range<PointMass> pointMasses;

    game->getStaticShapes(shapes);
    game->getStaticPoints(pointMasses);
    renderShapes(renderer, shapes, pointMasses);

    game->getDynamicShapes(shapes);
    game->getDynamicPoints(pointMasses);
    renderShapes(renderer, shapes, pointMasses);

    Range<Spring> springs;
    game->getSprings(springs);
    renderSprings(renderer, springs, pointMasses);
}