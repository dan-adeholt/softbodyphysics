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
#include "../physics/ShapeUtils.h"
#include "Game.h"

float MIN_LINE_POS = -100000;
float MAX_LINE_POS = 1000000;

struct GameVertex
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
    Array<GameVertex> vertices;
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

void addCircle(Array<GameVertex> &vertices, float x, float y, float scale, SDL_Color color)
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

void addLine(Array<GameVertex> &vertices, float p0x, float p0y, float p1x, float p1y, float scale, SDL_Color color)
{
    float lineWidth = 2.0f / scale;
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

void GameRenderer::drawSubshape(SDL_Renderer *renderer, const AddSubShapeData &addSubshapeData, float scale, const Vector2 &offset)
{
    for (int i = 0; i < addSubshapeData.numPoints; i++)
    {
        Vector2 pos = addSubshapeData.points[i] + offset;
        int next = (i + 1) % addSubshapeData.numPoints;
        Vector2 nextPos = addSubshapeData.points[next] + offset;
        addLine(m->vertices, pos.x, pos.y, nextPos.x, nextPos.y, scale, {255, 0, 0, 255});
    }
}

void GameRenderer::renderGame(SDL_Renderer *renderer, Game &game, ConsoleProfileInfo &profileInfo)
{
    m->vertices.clear();

    Vector2 mousePos(game.mousePos());
    Vector2 translatedMousePos = (mousePos - game.offset()) / game.scale();
    Vector2 &offset = game.offset();
    float &scale = game.scale();
    renderGrid(renderer, game, scale);
    PhysicsSpace &physicsSpace = game.physicsSpace();

    Range<Shape> shapes;

    PointMassesRange points = physicsSpace.points.range();
    float scaleForGeometry = min(3.1f, scale);

    GameRenderSettings renderSettings = game.renderSettings();

    if (renderSettings.renderShapeLines)
    {
        renderShapes(renderer, game.selectedShapeIndex(), physicsSpace.shapes.range(), points, physicsSpace, renderSettings.renderVelocityVectors, renderSettings.renderShapeMatching, renderSettings.renderPointIndices, scaleForGeometry, game.shapeMatchDragData());
    }

    if (renderSettings.renderSprings)
    {
        // TODO: Render shape matching springs
    }

    if (renderSettings.renderPoints)
    {
        int selectedShapeIndex = game.selectedShapeIndex();

        int selectedShapeStart = -1;
        int selectedShapeEnd = -1;

        if (selectedShapeIndex != -1)
        {
            Shape &selectedShape = physicsSpace.shapes[selectedShapeIndex];
            selectedShapeStart = selectedShape.start;
            selectedShapeEnd = selectedShape.end;
        }

        for (int i = 0; i < points.size(); i++)
        {
            Vector2 pos = points.pos[i];

            if (i >= selectedShapeStart && i < selectedShapeEnd)
            {
                addCircle(m->vertices, pos.x, pos.y, scale, {0, 64, 255, 255});
            }
            else
            {
                addCircle(m->vertices, pos.x, pos.y, scale, {255, 0, 0, 255});
            }
        }
    }

    const AddSubShapeData &addSubShapeData = game.addSubShapeData();

    if (addSubShapeData.active)
    {
        Vector2 sourcePos = addSubShapeData.mouseDown ? addSubShapeData.sourcePos : translatedMousePos;
        drawSubshape(renderer, addSubShapeData, scale, sourcePos);

        if (addSubShapeData.mouseDown)
        {
            for (int i = 0; i < abs(addSubShapeData.repeatX); i++)
            {
                Vector2 subOffset(i * gridSize, 0);

                if (addSubShapeData.repeatX < 0)
                {
                    subOffset.x = -subOffset.x;
                }

                drawSubshape(renderer, addSubShapeData, scale, sourcePos + subOffset);
            }

            for (int i = 0; i < abs(addSubShapeData.repeatY); i++)
            {
                Vector2 subOffset(0.0f, i * gridSize);

                if (addSubShapeData.repeatY < 0)
                {
                    subOffset.y = -subOffset.y;
                }

                drawSubshape(renderer, addSubShapeData, scale, sourcePos + subOffset);
            }
        }

        for (int i = 0; i < addSubShapeData.numPoints; i++)
        {
            Vector2 p0 = addSubShapeData.points[i] + sourcePos;

            for (int j = 0; j < physicsSpace.points.size(); j++)
            {
                Vector2 p1 = physicsSpace.points.pos[j];
                if (Vector2::vec2distance(p0.x, p0.y, p1.x, p1.y) < minPointSnapDist)
                {
                    addCircle(m->vertices, p0.x, p0.y, scale / 1.25f, {0, 255, 0, 255});
                }
            }
        }
    }

    PhysicsSpace &prevPhysicsSpace = game.lastCollisionSpace();
    PointMassesRange lastCollisionPoints = physicsSpace.points.range();

    GameVertex *vtx_buffer = &m->vertices[0];
    const float *xy = (const float *)(const void *)((const char *)(vtx_buffer) + offsetof(GameVertex, pos));
    const float *uv = (const float *)(const void *)((const char *)(vtx_buffer) + offsetof(GameVertex, uv));
    const SDL_Color *color = (const SDL_Color *)(const void *)((const char *)(vtx_buffer) + offsetof(GameVertex, color)); // SDL 2.0.19+

    for (int i = 0; i < m->vertices.size(); i++)
    {
        // Scale coordinates by scale and translate by offset
        m->vertices[i].pos.x = m->vertices[i].pos.x * scale + offset.x;
        m->vertices[i].pos.y = m->vertices[i].pos.y * scale + offset.y;
    }

    SDL_RenderGeometryRaw(renderer, m->texture,
                          xy, (int)sizeof(GameVertex),
                          color, (int)sizeof(GameVertex),
                          uv, (int)sizeof(GameVertex),
                          m->vertices.size(), nullptr, 0, 0);
}

void GameRenderer::renderShapes(SDL_Renderer *renderer, int selectedShapeIndex, Range<Shape> shapes, PointMassesRange &pointMasses, PhysicsSpace &space, bool renderVelocity, bool renderShapeMatching, bool renderPointIndices, float scale, const ShapeMatchDragData &dragData)
{
    for (int i = 0; i < shapes.size; i++)
    {
        const Shape &shape = shapes.data[i];
        ShapeIndexedRange shapeRange(shape);
        Vector2 startPos = pointMasses.pos[shapeRange[0]];
        Vector2 pos = startPos;

        SDL_Color color = {0, 0, 0, 255};

        if (i == selectedShapeIndex)
        {
            color = {50, 200, 50, 255};
        }

        SDL_Color colorInterior = {0, 0, 255, 255};

        bool interiorEdge = shapeRange.hasInteriorEdge(0);

        Vector2 startVelocity = pointMasses.velocity[shapeRange[0]];

        if (renderVelocity)
        {
            Console::drawVelocityVector(startPos, startVelocity, 0xFF0000);
        }

        if (renderPointIndices)
        {
            Console::logFrame(startPos.x, startPos.y, "%d", shapeRange[0]);
        }

        for (int pointIndex = 1; pointIndex < shapeRange.size(); pointIndex++)
        {
            Vector2 nextPos = pointMasses.pos[shapeRange[pointIndex]];
            Vector2 nextVelocity = pointMasses.velocity[shapeRange[pointIndex]];

            if (renderPointIndices)
            {
                Console::logFrame(nextPos.x, nextPos.y, "%d", shapeRange[pointIndex]);
            }

            if (isnan(pos.x) || isnan(pos.y) || isnan(nextPos.x) || isnan(nextPos.y))
            {
                continue;
            }

            if (renderVelocity)
            {
                Console::drawVelocityVector(nextPos, nextVelocity, 0xFF0000);
            }

            addLine(m->vertices, pos.x, pos.y, nextPos.x, nextPos.y, scale, interiorEdge ? colorInterior : color);
            pos = nextPos;
            interiorEdge = shapeRange.hasInteriorEdge(pointIndex);
        }

        addLine(m->vertices, pos.x, pos.y, startPos.x, startPos.y, scale, interiorEdge ? colorInterior : color);

        if (!renderShapeMatching || shape.isStatic)
        {
            continue;
        }

        ShapeProperties averages = ShapeUtils::getShapeProperties(pointMasses, shape);
        ShapeIterator itr(shape);

        // TODO: Add rendering of shape matching
        startPos = ShapeUtils::getShapePos(pointMasses, shape, itr.index(), averages, dragData);
        pos = startPos;
        itr.next();

        SDL_Color lineColor = {0, 255, 0, 255};

        while (itr.isValid())
        {
            Vector2 nextPos = ShapeUtils::getShapePos(pointMasses, shape, itr.index(), averages, dragData);

            if (isnan(pos.x) || isnan(pos.y) || isnan(nextPos.x) || isnan(nextPos.y))
            {
                itr.next();
                continue;
            }

            addLine(m->vertices, pos.x, pos.y, nextPos.x, nextPos.y, scale, lineColor);
            pos = nextPos;
            itr.next();
        }

        addLine(m->vertices, pos.x, pos.y, startPos.x, startPos.y, scale, lineColor);
    }
}

inline void addSpring(Array<GameVertex> &vertices, float p0x, float p0y, float p1x, float p1y, float springLength, float scale)
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

void GameRenderer::renderGrid(SDL_Renderer *renderer, Game &game, float scale)
{
    unsigned char gray = 240;
    SDL_Color color = {gray, gray, gray, 255};

    for (int i = 0; i < 200; i++)
    {
        float x = -2000.0f + i * gridSize;
        addLine(m->vertices, x, -10000, x, 10000, scale, color);
    }

    for (int i = 0; i < 200; i++)
    {
        float y = -2000.0f + i * gridSize;
        addLine(m->vertices, -10000, y, 10000, y, scale, color);
    }
}
