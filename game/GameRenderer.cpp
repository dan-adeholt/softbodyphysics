#include "GameRenderer.h"
#include <SDL.h>
#include "../containers/Array.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#include "../stb_image/stb_image.h"
#pragma GCC diagnostic pop

#include "../utils/Console.h"
#include "../utils/MinMax.h"
#include "stddef.h"
#include "../physics/Physics.h"
#include "../physics/PhysicsSpace.h"
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
    Uint32 Rmask, Gmask, Bmask, Amask;
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

AtlasCoordinate whiteColor(0, 0, 15, 15);
AtlasCoordinate circle(17, 1, 13, 13);
AtlasCoordinate springData(50, 0, 3, 512);

void addCircle(Array<Vertex> &vertices, float x, float y, float scale, SDL_Color color)
{
    float size = circle.w * 0.5f / scale;
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

void addLine(Array<Vertex> &vertices, float p0x, float p0y, float p1x, float p1y, float scale, SDL_Color color)
{
    float lineWidth = 1.0f / scale;
    float dx = p1x - p0x;
    float dy = p1y - p0y;
    float length = Vector2::vec2length(dx, dy);
    float nx = (-dy / length) * lineWidth * 0.5f;
    float ny = (dx / length) * lineWidth * 0.5f;

    vertices.push({color,
                   {p0x + nx, p0y + ny},
                   whiteColor.topRight});
    vertices.push({color,
                   {p1x + nx, p1y + ny},
                   whiteColor.bottomRight});
    vertices.push({color,
                   {p1x - nx, p1y - ny},
                   whiteColor.bottomLeft});
    vertices.push({color,
                   {p1x - nx, p1y - ny},
                   whiteColor.bottomLeft});
    vertices.push({color,
                   {p0x - nx, p0y - ny},
                   whiteColor.topLeft});
    vertices.push({color,
                   {p0x + nx, p0y + ny},
                   whiteColor.topRight});
}

void GameRenderer::renderGame(SDL_Renderer *renderer, Game &game, bool renderShapeMatching, ConsoleProfileInfo &profileInfo)
{
    m->vertices.clear();

    Vector2 &offset = game.offset();
    float &scale = game.scale();

    Range<Shape> shapes;

    PhysicsSpace &physicsSpace = game.physicsSpace();
    PointMassesRange points = physicsSpace.points.range();
    float scaleForGeometry = min(3.1f, scale);

    GameRenderSettings renderSettings = game.renderSettings();

    if (renderSettings.renderShapeLines)
    {
        renderShapes(renderer, physicsSpace.shapes.range(), points, physicsSpace, renderShapeMatching, scaleForGeometry);
    }

    if (renderSettings.renderSprings)
    {
        renderSprings(renderer, physicsSpace.springs.range(), points, scaleForGeometry);
    }

    if (renderSettings.renderPoints)
    {

        for (int i = 0; i < points.size(); i++)
        {
            Vector2 pos = points.pos[i];
            addCircle(m->vertices, pos.x, pos.y, scale, {255, 255, 255, 255});

            // Vector2 velocity = points.velocity[i];

            // Vector2 target = pos + velocity * 200.0f;
            // addLine(m->vertices, pos.x, pos.y, target.x, target.y, scale, {255, 0, 255, 255});
            // addCircle(m->vertices, target.x, target.y, scale, {255, 255, 255, 255});
        }
    }

    PhysicsSpace &prevPhysicsSpace = game.lastCollisionSpace();
    PointMassesRange lastCollisionPoints = physicsSpace.points.range();

    Vertex *vtx_buffer = &m->vertices[0];
    const float *xy = (const float *)(const void *)((const char *)(vtx_buffer) + offsetof(Vertex, pos));
    const float *uv = (const float *)(const void *)((const char *)(vtx_buffer) + offsetof(Vertex, uv));
    const SDL_Color *color = (const SDL_Color *)(const void *)((const char *)(vtx_buffer) + offsetof(Vertex, color)); // SDL 2.0.19+

    for (int i = 0; i < m->vertices.size(); i++)
    {
        // Scale coordinates by scale and translate by offset
        m->vertices[i].pos.x = m->vertices[i].pos.x * scale + offset.x;
        m->vertices[i].pos.y = m->vertices[i].pos.y * scale + offset.y;
    }

    SDL_RenderGeometryRaw(renderer, m->texture,
                          xy, (int)sizeof(Vertex),
                          color, (int)sizeof(Vertex),
                          uv, (int)sizeof(Vertex),
                          m->vertices.size(), nullptr, 0, 0);
}

void GameRenderer::renderShapes(SDL_Renderer *renderer, Range<Shape> shapes, PointMassesRange &pointMasses, PhysicsSpace &space, bool renderShapeMatching, float scale)
{
    for (int i = 0; i < shapes.size; i++)
    {
        const Shape &shape = shapes.data[i];

        Vector2 startPos = pointMasses.pos[shape.start];
        Vector2 pos = startPos;

        for (int pointIndex = shape.start + 1; pointIndex < shape.end; pointIndex++)
        {
            Vector2 nextPos = pointMasses.pos[pointIndex];

            if (isnan(pos.x) || isnan(pos.y) || isnan(nextPos.x) || isnan(nextPos.y))
            {
                continue;
            }

            addLine(m->vertices, pos.x, pos.y, nextPos.x, nextPos.y, scale, {0, 0, 0, 255});
            pos = nextPos;
        }

        addLine(m->vertices, pos.x, pos.y, startPos.x, startPos.y, scale, {0, 0, 0, 255});

        if (!renderShapeMatching)
        {
            continue;
        }

        if (shape.subShapeSpan.isValid())
        {
            Range<ShapeQuad> subshape = space.partialShapes.range(shape.subShapeSpan);

            for (int j = 0; j < subshape.size; j++)
            {
                const ShapeQuad &subShape = subshape.data[j];
                Vector2 startPos = subShape.shapePos[0];
                Vector2 pos = startPos;

                for (int pointIndex = 1; pointIndex < subShape.size; pointIndex++)
                {
                    Vector2 nextPos = subShape.shapePos[pointIndex];

                    if (isnan(pos.x) || isnan(pos.y) || isnan(nextPos.x) || isnan(nextPos.y))
                    {
                        continue;
                    }

                    addLine(m->vertices, pos.x, pos.y, nextPos.x, nextPos.y, scale, {0, 255, 0, 255});
                    pos = nextPos;
                }

                addLine(m->vertices, pos.x, pos.y, startPos.x, startPos.y, scale, {0, 255, 0, 255});
            }
        }
        else
        {
            startPos = pointMasses.shapePos[shape.start];
            pos = startPos;

            for (int pointIndex = shape.start + 1; pointIndex < shape.end; pointIndex++)
            {
                Vector2 nextPos = pointMasses.shapePos[pointIndex];

                if (isnan(pos.x) || isnan(pos.y) || isnan(nextPos.x) || isnan(nextPos.y))
                {
                    continue;
                }

                addLine(m->vertices, pos.x, pos.y, nextPos.x, nextPos.y, scale, {0, 255, 0, 255});
                pos = nextPos;
            }

            addLine(m->vertices, pos.x, pos.y, startPos.x, startPos.y, scale, {0, 255, 0, 255});
        }
    }
}

inline void addSpring(Array<Vertex> &vertices, float p0x, float p0y, float p1x, float p1y, float springLength, float scale)
{
    AtlasCoordinate springDataMod(springData.x, springData.y, springData.w, min(512.0f, springLength) * scale);

    float lineWidth = 1.5f / scale;

    float dx = p1x - p0x;
    float dy = p1y - p0y;
    float vecLength = Vector2::vec2length(dx, dy);
    float normalX = (-dy / vecLength) * lineWidth;
    float normalY = (dx / vecLength) * lineWidth;

    // float tension = fabs(1.0f - (springLength / vecLength));
    SDL_Color color = {64, 64, 64, 255};

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

void GameRenderer::renderSprings(SDL_Renderer *renderer, Range<Spring> springs, const PointMassesRange &points, float scale)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    for (int i = 0; i < springs.size; i++)
    {
        const Spring &spring = springs.data[i];
        Vector2 p0 = points.pos[spring.pointA];
        Vector2 p1 = points.pos[spring.pointB];

        // To prevent SDL taking extremely long to render degenerate lines
        if (p0.x > MIN_LINE_POS && p0.x < MAX_LINE_POS &&
            p0.y > MIN_LINE_POS && p0.y < MAX_LINE_POS &&
            p1.x > MIN_LINE_POS && p1.x < MAX_LINE_POS &&
            p1.y > MIN_LINE_POS && p1.y < MAX_LINE_POS)
        {
            addSpring(m->vertices, p0.x, p0.y, p1.x, p1.y, spring.length, scale);
        }
    }
}
