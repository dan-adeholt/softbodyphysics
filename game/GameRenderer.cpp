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

void addCircle(Array<Vertex> &vertices, const Vector2 &pos, SDL_Color color)
{
    float size = circle.w * 0.5f;
    Vector2 offset = pos - Vector2(size * 0.5f, size * 0.5f);

    vertices.push({color,
                   {offset.x, offset.y},
                   circle.topLeft});
    vertices.push({color,
                   {offset.x + size, offset.y},
                   circle.topRight});
    vertices.push({color,
                   {offset.x + size, offset.y + size},
                   circle.bottomRight});
    vertices.push({color,
                   {offset.x + size, offset.y + size},
                   circle.bottomRight});
    vertices.push({color,
                   {offset.x, offset.y + size},
                   circle.bottomLeft});
    vertices.push({color,
                   {offset.x, offset.y},
                   circle.topLeft});
}

void addLine(Array<Vertex> &vertices, const Vector2 &p0, const Vector2 &p1, SDL_Color color)
{
    float lineWidth = 1.0f;
    Vector2 dir = p1 - p0;
    Vector2 normal = dir.normalVector().normalized();
    normal *= lineWidth * 0.5f;
    vertices.push({color,
                   {p0.x + normal.x, p0.y + normal.y},
                   blackColor.topRight});
    vertices.push({color,
                   {p1.x + normal.x, p1.y + normal.y},
                   blackColor.bottomRight});
    vertices.push({color,
                   {p1.x - normal.x, p1.y - normal.y},
                   blackColor.bottomLeft});
    vertices.push({color,
                   {p1.x - normal.x, p1.y - normal.y},
                   blackColor.bottomLeft});
    vertices.push({color,
                   {p0.x - normal.x, p0.y - normal.y},
                   blackColor.topLeft});
    vertices.push({color,
                   {p0.x + normal.x, p0.y + normal.y},
                   blackColor.topRight});
}

void GameRenderer::renderGame(SDL_Renderer *renderer, Game &game, ConsoleProfileInfo &profileInfo)
{
    m->vertices.clear();

    Range<Shape> shapes;
    Range<PointMass> pointMasses;

    game.getStaticShapes(shapes);
    game.getStaticPoints(pointMasses);

    renderShapes(renderer, shapes, pointMasses);

    game.getDynamicShapes(shapes);
    game.getDynamicPoints(pointMasses);
    renderShapes(renderer, shapes, pointMasses);

    Range<Spring> springs;
    game.getSprings(springs);
    renderSprings(renderer, springs, pointMasses);
    for (int i = 0; i < pointMasses.size; i++)
    {
        PointMass &point = pointMasses[i];
        addCircle(m->vertices, point.pos, {255, 255, 255, 255});
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

void GameRenderer::renderShapes(SDL_Renderer *renderer, Range<Shape> shapes, Range<PointMass> pointMasses)
{
    for (int i = 0; i < shapes.size; i++)
    {
        const Shape &shape = shapes.data[i];
        PointMass firstPoint = pointMasses[shape.start];
        PointMass p0 = firstPoint;

        for (int pointIndex = shape.start + 1; pointIndex < shape.end; pointIndex++)
        {
            const PointMass &p1 = pointMasses[pointIndex];

            if (isnan(p0.pos.x) || isnan(p0.pos.y) || isnan(p1.pos.x) || isnan(p1.pos.y))
            {
                continue;
            }
            addLine(m->vertices, p0.pos, p1.pos, {255, 255, 255, 255});
            p0 = p1;
        }

        addLine(m->vertices, p0.pos, firstPoint.pos, {255, 255, 255, 255});
    }
}

inline void addSpring(Array<Vertex> &vertices, const Vector2 &p0, const Vector2 &p1, float springLength, SDL_Color color)
{
    AtlasCoordinate springDataMod(springData.x, springData.y, springData.w, min(512.0f, springLength));

    float lineWidth = 3.0f;
    Vector2 dir = p1 - p0;
    Vector2 normal = dir.normalVector().normalized();
    normal *= lineWidth * 0.5f;
    vertices.push({color,
                   {p0.x + normal.x, p0.y + normal.y},
                   springDataMod.topRight});
    vertices.push({color,
                   {p1.x + normal.x, p1.y + normal.y},
                   springDataMod.bottomRight});
    vertices.push({color,
                   {p1.x - normal.x, p1.y - normal.y},
                   springDataMod.bottomLeft});
    vertices.push({color,
                   {p1.x - normal.x, p1.y - normal.y},
                   springDataMod.bottomLeft});
    vertices.push({color,
                   {p0.x - normal.x, p0.y - normal.y},
                   springDataMod.topLeft});
    vertices.push({color,
                   {p0.x + normal.x, p0.y + normal.y},
                   springDataMod.topRight});
}

void GameRenderer::renderSprings(SDL_Renderer *renderer, Range<Spring> springs, const Range<PointMass> &points)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    for (int i = 0; i < springs.size; i++)
    {
        const Spring &spring = springs.data[i];
        const PointMass &pointA = points[spring.pointA];
        const PointMass &pointB = points[spring.pointB];
        float currentLength = (pointA.pos - pointB.pos).length();
        float tension = fabs(1.0f - (spring.length / currentLength));

        // To prevent SDL taking extremely long to render degenerate lines
        if (pointA.pos.x > MIN_LINE_POS && pointA.pos.x < MAX_LINE_POS &&
            pointA.pos.y > MIN_LINE_POS && pointA.pos.y < MAX_LINE_POS &&
            pointB.pos.x > MIN_LINE_POS && pointB.pos.x < MAX_LINE_POS &&
            pointB.pos.y > MIN_LINE_POS && pointB.pos.y < MAX_LINE_POS)
        {
            addSpring(m->vertices, pointA.pos, pointB.pos, spring.length, {255, 255, 255, 255});
        }
    }
}
