#include "GameRenderer.h"
#include <SDL.h>
#include "../containers/Array.h"
#include "../stb_image/stb_image.h"
#include "../utils/Console.h"
#include "../utils/MinMax.h"
#include "stddef.h"
#include "Physics.h"
#include "Game.h"

float MIN_LINE_POS = -100000;
float MAX_LINE_POS = 1000000;

struct Vertex
{
    SDL_Color color;
    Vector2 pos;
    Vector2 uv;
};

SDL_Surface *
loadImage(const char *filename)
{
    // Read data
    int width, height, bytesPerPixel;
    void *data = stbi_load(filename, &width, &height, &bytesPerPixel, 0);

    // Calculate pitch
    int pitch;
    pitch = width * bytesPerPixel;
    pitch = (pitch + 3) & ~3;

    // Setup relevance bitmask
    int Rmask, Gmask, Bmask, Amask;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    Rmask = 0x000000FF;
    Gmask = 0x0000FF00;
    Bmask = 0x00FF0000;
    Amask = (bytesPerPixel == 4) ? 0xFF000000 : 0;
#else
    int s = (bytesPerPixel == 4) ? 0 : 8;
    Rmask = 0xFF000000 >> s;
    Gmask = 0x00FF0000 >> s;
    Bmask = 0x0000FF00 >> s;
    Amask = 0x000000FF >> s;
#endif
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, width, height, bytesPerPixel * 8, pitch, Rmask, Gmask,
                                                    Bmask, Amask);
    if (!surface)
    {
        Console::log("Unable to create surface from image: %s", filename);
        stbi_image_free(data);
        return NULL;
    }
    return surface;
}

struct GameRenderer::Impl
{
    Impl(SDL_Renderer *renderer) : texture(nullptr)
    {
        SDL_Surface *surface = loadImage("./data/atlas.png");
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    SDL_Texture *texture;
    Array<Vertex> vertices;
};

GameRenderer::GameRenderer(SDL_Renderer *renderer) : m(new Impl(renderer))
{
}

GameRenderer::~GameRenderer()
{
    delete m;
}

struct AtlasCoordinate
{
    AtlasCoordinate(float x, float y, float width, float height) : x(x), y(y), w(width), h(height)
    {
        topLeft = {x / 512.0f, y / 512.0f};
        topRight = {(x + width) / 512.0f, y / 512.0f};
        bottomLeft = {x / 512.0f, (y + height) / 512.0f};
        bottomRight = {(x + width) / 512.0f, (y + height) / 512.0f};
    }
    float x, y, w, h;
    Vector2 topLeft;
    Vector2 topRight;
    Vector2 bottomLeft;
    Vector2 bottomRight;
};

AtlasCoordinate blackColor(0, 0, 15, 15);
AtlasCoordinate circle(17, 1, 13, 13);
AtlasCoordinate springData(50, 0, 3, 512);

void addCircle(Array<Vertex> &vertices, float x, float y, SDL_Color color)
{
    float size = circle.w * 0.5f;
    float cx = x - size * 0.5f;
    float cy = y - size * 0.5f;

    vertices.push({color,
                   {cx, cy},
                   circle.topLeft});
    vertices.push({color,
                   {cx + size, cy},
                   circle.topRight});
    vertices.push({color,
                   {cx + size, cy + size},
                   circle.bottomRight});
    vertices.push({color,
                   {cx + size, cy + size},
                   circle.bottomRight});
    vertices.push({color,
                   {cx, cy + size},
                   circle.bottomLeft});
    vertices.push({color,
                   {cx, cy},
                   circle.topLeft});
}

void addLine(Array<Vertex> &vertices, float p0x, float p0y, float p1x, float p1y, SDL_Color color)
{
    float lineWidth = 1.0f;
    float dx = p1x - p0x;
    float dy = p1y - p0y;
    float length = Vector2::vec2length(dx, dy);
    float nx = (-dy / length) * lineWidth * 0.5f;
    float ny = (dx / length) * lineWidth * 0.5f;

    vertices.push({color,
                   {p0x + nx, p0y + ny},
                   blackColor.topRight});
    vertices.push({color,
                   {p1x + nx, p1y + ny},
                   blackColor.bottomRight});
    vertices.push({color,
                   {p1x - nx, p1y - ny},
                   blackColor.bottomLeft});
    vertices.push({color,
                   {p1x - nx, p1y - ny},
                   blackColor.bottomLeft});
    vertices.push({color,
                   {p0x - nx, p0y - ny},
                   blackColor.topLeft});
    vertices.push({color,
                   {p0x + nx, p0y + ny},
                   blackColor.topRight});
}

void GameRenderer::renderGame(SDL_Renderer *renderer, Game &game, ConsoleProfileInfo &profileInfo)
{
    m->vertices.clear();

    Range<Shape> shapes;
    PointMassesRange pointMasses;

    game.getStaticShapes(shapes);
    game.getStaticPoints(pointMasses);

    renderShapes(renderer, shapes, pointMasses);

    game.getDynamicShapes(shapes);
    game.getDynamicPoints(pointMasses);
    renderShapes(renderer, shapes, pointMasses);
    Range<Spring> springs;
    game.getSprings(springs);
    renderSprings(renderer, springs, pointMasses);

    for (int i = 0; i < pointMasses.size(); i++)
    {
        addCircle(m->vertices, pointMasses.x[i], pointMasses.y[i], {255, 255, 255, 255});
    }

    Vector2 offset(700, 20);
    float height = 512.0f;
    float width = 5.0f;

    Vertex *vtx_buffer = &m->vertices[0];
    const float *xy = (const float *)(const void *)((const char *)(vtx_buffer) + offsetof(Vertex, pos));
    const float *uv = (const float *)(const void *)((const char *)(vtx_buffer) + offsetof(Vertex, uv));
    const SDL_Color *color = (const SDL_Color *)(const void *)((const char *)(vtx_buffer) + offsetof(Vertex, color)); // SDL 2.0.19+

    SDL_RenderGeometryRaw(renderer, m->texture,
                          xy, (int)sizeof(Vertex),
                          color, (int)sizeof(Vertex),
                          uv, (int)sizeof(Vertex),
                          m->vertices.size(), nullptr, 0, 0);
}

void GameRenderer::renderShapes(SDL_Renderer *renderer, Range<Shape> shapes, PointMassesRange &pointMasses)
{
    for (int i = 0; i < shapes.size; i++)
    {
        const Shape &shape = shapes.data[i];

        float startX = pointMasses.x[shape.start];
        float startY = pointMasses.y[shape.start];
        float x = startX;
        float y = startY;

        for (int pointIndex = shape.start + 1; pointIndex < shape.end; pointIndex++)
        {
            float nextX = pointMasses.x[pointIndex];
            float nextY = pointMasses.y[pointIndex];

            if (isnan(x) || isnan(y) || isnan(nextX) || isnan(nextY))
            {
                continue;
            }

            addLine(m->vertices, x, y, nextX, nextY, {255, 255, 255, 255});
            x = nextX;
            y = nextY;
        }

        addLine(m->vertices, x, y, startX, startY, {255, 255, 255, 255});
    }
}

inline void addSpring(Array<Vertex> &vertices, float p0x, float p0y, float p1x, float p1y, float springLength, SDL_Color color)
{
    AtlasCoordinate springDataMod(springData.x, springData.y, springData.w, min(512.0f, springLength));

    float lineWidth = 1.5f;

    float dx = p1x - p0x;
    float dy = p1y - p0y;
    float vecLength = Vector2::vec2length(dx, dy);
    float normalX = (-dy / vecLength) * lineWidth;
    float normalY = (dx / vecLength) * lineWidth;

    vertices.push({color,
                   {p0x + normalX, p0y + normalY},
                   springDataMod.topRight});
    vertices.push({color,
                   {p1x + normalX, p1y + normalY},
                   springDataMod.bottomRight});
    vertices.push({color,
                   {p1x - normalX, p1y - normalY},
                   springDataMod.bottomLeft});
    vertices.push({color,
                   {p1x - normalX, p1y - normalY},
                   springDataMod.bottomLeft});
    vertices.push({color,
                   {p0x - normalX, p0y - normalY},
                   springDataMod.topLeft});
    vertices.push({color,
                   {p0x + normalX, p0y + normalY},
                   springDataMod.topRight});
}

void GameRenderer::renderSprings(SDL_Renderer *renderer, Range<Spring> springs, const PointMassesRange &points)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    for (int i = 0; i < springs.size; i++)
    {
        const Spring &spring = springs.data[i];
        float x0 = points.x[spring.pointA];
        float y0 = points.y[spring.pointA];
        float x1 = points.x[spring.pointB];
        float y1 = points.y[spring.pointB];
        float currentLength = Vector2::vec2length(x1 - x0, y1 - y0);

        float tension = fabs(1.0f - (spring.length / currentLength));

        // To prevent SDL taking extremely long to render degenerate lines
        if (x0 > MIN_LINE_POS && x0 < MAX_LINE_POS &&
            y0 > MIN_LINE_POS && y0 < MAX_LINE_POS &&
            x1 > MIN_LINE_POS && x1 < MAX_LINE_POS &&
            y1 > MIN_LINE_POS && y1 < MAX_LINE_POS)
        {
            addSpring(m->vertices, x0, y0, x1, y1, spring.length, {255, 255, 255, 255});
        }
    }
}
