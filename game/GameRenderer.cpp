#include "GameRenderer.h"
#include <SDL3/SDL.h>
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
#include "../physics/CollisionSolver.h"
#include "Game.h"
#include <math.h>

float MIN_LINE_POS = -100000;
float MAX_LINE_POS = 1000000;

struct GameVertex
{
    SDL_FColor color;
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

    SDL_PixelFormat format = SDL_GetPixelFormatForMasks(
        bytesPerPixel * 8, Rmask, Gmask, Bmask, Amask);

    // Create the SDL3 surface from our pixel data buffer :contentReference[oaicite:2]{index=2}
    SDL_Surface *surface = SDL_CreateSurfaceFrom(
        width, height, format, data, pitch);
    if (!surface)
    {
        SDL_Log("Unable to create surface from image %s: %s",
                filename, SDL_GetError());
        stbi_image_free(data);
        return NULL;
    }

    return surface;
}

void renderVertices(SDL_Renderer *renderer, Array<GameVertex> &vertices, const Vector2 &offset, float scale, SDL_Texture *texture)
{
    if (vertices.size() == 0)
    {
        return;
    }

    GameVertex *vtx_buffer = &vertices[0];
    const float *xy = (const float *)(const void *)((const char *)(vtx_buffer) + offsetof(GameVertex, pos));
    const float *uv = (const float *)(const void *)((const char *)(vtx_buffer) + offsetof(GameVertex, uv));
    const SDL_FColor *color = (const SDL_FColor *)(const void *)((const char *)(vtx_buffer) + offsetof(GameVertex, color)); // SDL 2.0.19+

    for (int i = 0; i < vertices.size(); i++)
    {
        // Scale coordinates by scale and translate by offset
        vertices[i].pos.x = vertices[i].pos.x * scale + offset.x;
        vertices[i].pos.y = vertices[i].pos.y * scale + offset.y;
    }

    SDL_RenderGeometryRaw(renderer, texture,
                          xy, (int)sizeof(GameVertex),
                          color, (int)sizeof(GameVertex),
                          uv, (int)sizeof(GameVertex),
                          vertices.size(), nullptr, 0, 0);
}

struct GameRenderer::Impl
{
    Impl(SDL_Renderer *renderer) : texture(nullptr)
    {
        SDL_Surface *surface = loadImage("./data/atlas.png");
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
    }

    SDL_Texture *texture;

    Array<GameVertex> foregroundVertices;
    Array<GameVertex> backgroundVertices;

    // Styled rendering layers, drawn back to front
    Array<GameVertex> shadowVertices;
    Array<GameVertex> fillVertices;
    Array<GameVertex> outlineVertices;
    Array<GameVertex> detailVertices;
    Array<Vector2> outlinePoints;
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
    AtlasCoordinate(float x, float y, float width, float height) : x(x), y(y), w(width), h(height), normWidth(width / 512.0f), normHeight(height / 512.0f)
    {
        topLeft = {x / 512.0f, y / 512.0f};
        topRight = {(x + width) / 512.0f, y / 512.0f};
        bottomLeft = {x / 512.0f, (y + height) / 512.0f};
        bottomRight = {(x + width) / 512.0f, (y + height) / 512.0f};
    }

    float x, y, w, h;
    float normWidth;
    float normHeight;

    Vector2 topLeft;
    Vector2 topRight;
    Vector2 bottomLeft;
    Vector2 bottomRight;
};

AtlasCoordinate whiteColor(0, 0, 15, 15);
AtlasCoordinate circle(17, 1, 13, 13);
AtlasCoordinate springData(50, 0, 3, 512);

void addCircle(Array<GameVertex> &vertices, float x, float y, float scale, SDL_FColor color)
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

void addLine(Array<GameVertex> &vertices, float p0x, float p0y, float p1x, float p1y, float scale, SDL_FColor color, float baseLineWidth = 2.0f)
{
    float lineWidth = baseLineWidth / scale;
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
        addLine(m->foregroundVertices, pos.x, pos.y, nextPos.x, nextPos.y, scale, {255, 0, 0, 255});
    }
}

void GameRenderer::renderGame(SDL_Renderer *renderer, Game &game, ConsoleProfileInfo &profileInfo)
{
    m->foregroundVertices.clear();
    m->backgroundVertices.clear();

    Vector2 mousePos(game.mousePos());
    Vector2 translatedMousePos = (mousePos - game.offset()) / game.scale();
    Vector2 &offset = game.offset();
    float &scale = game.scale();

    if (!game.debugDraw())
    {
        renderStyled(renderer, game);
        return;
    }

    renderGrid(renderer, game, scale, 2.0f, {0.941f, 0.941f, 0.941f, 1.0f});
    // renderCollisionGrid(renderer, game, scale);

    PhysicsSpace &physicsSpace = game.physicsSpace();

    Range<Shape> shapes;

    PointMassesRange points = physicsSpace.points.range();
    float scaleForGeometry = min(3.1f, scale);

    GameRenderSettings renderSettings = game.renderSettings();

    if (renderSettings.renderTriangles)
    {
        for (int i = 0; i < physicsSpace.triangleIndices.size(); i += 3)
        {
            int p0 = physicsSpace.triangleIndices[i];
            int p1 = physicsSpace.triangleIndices[i + 1];
            int p2 = physicsSpace.triangleIndices[i + 2];

            Vector2 p0Pos = points.pos[p0];
            Vector2 p1Pos = points.pos[p1];
            Vector2 p2Pos = points.pos[p2];

            addLine(m->foregroundVertices, p0Pos.x, p0Pos.y, p1Pos.x, p1Pos.y, scaleForGeometry, {0, 0, 0, 255}, 1.0f);
            addLine(m->foregroundVertices, p1Pos.x, p1Pos.y, p2Pos.x, p2Pos.y, scaleForGeometry, {0, 0, 0, 255}, 1.0f);
            addLine(m->foregroundVertices, p2Pos.x, p2Pos.y, p0Pos.x, p0Pos.y, scaleForGeometry, {0, 0, 0, 255}, 1.0f);
        }
    }

    if (renderSettings.renderShapeLines)
    {
        renderShapes(renderer,
                     game.selectedShapeIndex(),
                     game.collisionCandidates(),
                     physicsSpace.shapes.range(),
                     points,
                     physicsSpace,
                     renderSettings.renderVelocityVectors,
                     renderSettings.renderShapeMatching,
                     renderSettings.renderPointIndices,
                     scaleForGeometry,
                     game.shapeMatchDragData());
    }

    if (renderSettings.renderShapeJoints)
    {
        for (int i = 0; i < physicsSpace.shapeJoints.size(); i++)
        {
            const ShapeJoint &joint = physicsSpace.shapeJoints[i];
            const Shape &shapeA = physicsSpace.shapes[joint.shapeIndex1];
            const Shape &shapeB = physicsSpace.shapes[joint.shapeIndex2];

            Vector2 p0;
            PositionPair posPair = ShapeUtils::getShapeJointPositions(points, joint);

            SDL_FColor color = {0.0f, 1.0f, 0.0f, 1.0f};
            addLine(m->foregroundVertices, posPair.p0.x, posPair.p0.y, posPair.p1.x, posPair.p1.y, scaleForGeometry, color);
        }
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
                addCircle(m->foregroundVertices, pos.x, pos.y, scale, {0, 64, 255, 255});
            }
            else
            {
                addCircle(m->foregroundVertices, pos.x, pos.y, scale, {255, 0, 0, 255});
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
                Vector2 subOffset(static_cast<float>(i) * gridSize, 0.0f);

                if (addSubShapeData.repeatX < 0)
                {
                    subOffset.x = -subOffset.x;
                }

                drawSubshape(renderer, addSubShapeData, scale, sourcePos + subOffset);
            }

            for (int i = 0; i < abs(addSubShapeData.repeatY); i++)
            {
                Vector2 subOffset(0.0f, static_cast<float>(i) * gridSize);

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
                    addCircle(m->foregroundVertices, p0.x, p0.y, scale / 1.25f, {0, 255, 0, 255});
                }
            }
        }
    }

    PointMassesRange lastCollisionPoints = physicsSpace.points.range();

    renderVertices(renderer, m->backgroundVertices, offset, scale, m->texture);
    renderVertices(renderer, m->foregroundVertices, offset, scale, m->texture);
}

void GameRenderer::renderShapes(SDL_Renderer *renderer,
                                int selectedShapeIndex,
                                Range<int> collisionCandidates,
                                Range<Shape> shapes,
                                PointMassesRange &pointMasses,
                                PhysicsSpace &space,
                                bool renderVelocity,
                                bool renderShapeMatching,
                                bool renderPointIndices,
                                float scale,
                                const ShapeMatchDragData &dragData)
{
    for (int i = 0; i < shapes.size; i++)
    {
        const Shape &shape = shapes.data[i];
        ShapeIndexedRange shapeRange(shape);
        Vector2 startPos = pointMasses.pos[shapeRange[0]];
        Vector2 pos = startPos;

        SDL_FColor color = {0.0f, 0.0f, 0.0f, 1.0f};

        if (i == selectedShapeIndex)
        {
            color = {0.25f, 0.8f, 0.25f, 1.0f};
        }
        else if (collisionCandidates.contains(shape.index))
        {
            color = {0.8f, 0.25f, 0.25f, 1.0f};
        }

        SDL_FColor colorInterior = {0, 0, 255, 255};

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

            addLine(m->foregroundVertices, pos.x, pos.y, nextPos.x, nextPos.y, scale, interiorEdge ? colorInterior : color);
            pos = nextPos;
            interiorEdge = shapeRange.hasInteriorEdge(pointIndex);
        }

        addLine(m->foregroundVertices, pos.x, pos.y, startPos.x, startPos.y, scale, interiorEdge ? colorInterior : color);

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

        SDL_FColor lineColor = {0.0f, 1.0f, 0.0f, 1.0f};

        while (itr.isValid())
        {
            Vector2 nextPos = ShapeUtils::getShapePos(pointMasses, shape, itr.index(), averages, dragData);

            if (isnan(pos.x) || isnan(pos.y) || isnan(nextPos.x) || isnan(nextPos.y))
            {
                itr.next();
                continue;
            }

            addLine(m->foregroundVertices, pos.x, pos.y, nextPos.x, nextPos.y, scale, lineColor);
            pos = nextPos;
            itr.next();
        }

        addLine(m->foregroundVertices, pos.x, pos.y, startPos.x, startPos.y, scale, lineColor);
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
    SDL_FColor color = {0.25f, 0.25f, 0.25f, 1.0f};

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

void GameRenderer::renderGrid(SDL_Renderer *renderer, Game &game, float scale, float lineWidth, SDL_FColor color)
{
    for (int i = 0; i < 200; i++)
    {
        float x = -2000.0f + static_cast<float>(i) * gridSize;
        addLine(m->backgroundVertices, x, -10000, x, 10000, scale, color, lineWidth);
    }

    for (int i = 0; i < 200; i++)
    {
        float y = -2000.0f + static_cast<float>(i) * gridSize;
        addLine(m->backgroundVertices, -10000, y, 10000, y, scale, color, lineWidth);
    }
}

void GameRenderer::renderCollisionGrid(SDL_Renderer *renderer, Game &game, float scale)
{
    float gray = 0.941f;
    SDL_FColor color = {gray, 0.0f, gray, 1.0f};

    CollisionGridSimple &collisionGrid = game.collisionGrid();
    const float cellSize = static_cast<float>(collisionGrid.cellSize);
    const float gridHeight = static_cast<float>(collisionGrid.height) * cellSize;
    const float gridWidth = static_cast<float>(collisionGrid.width) * cellSize;

    Array<OrientedBoundingBox> &orientedBoundingBoxes = game.shapeOrientedBoundingBoxes();

    // SDL_FColor obbColor = {0.0f, 0.0f, 1.0f, 1.0f};
    // for (int i = 0; i < orientedBoundingBoxes.size(); i++)
    // {
    //     const OrientedBoundingBox &obb = orientedBoundingBoxes[i];
    //     Vector2 right = obb.axisX * obb.halfX;
    //     Vector2 up = obb.axisY * obb.halfY;

    //     Vector2 c0 = obb.center + right + up; // top-right
    //     Vector2 c1 = obb.center - right + up; // top-left
    //     Vector2 c2 = obb.center - right - up; // bottom-left
    //     Vector2 c3 = obb.center + right - up; // bottom-right
    //     addLine(m->foregroundVertices, c0.x, c0.y, c1.x, c1.y, scale, obbColor);
    //     addLine(m->foregroundVertices, c1.x, c1.y, c2.x, c2.y, scale, obbColor);
    //     addLine(m->foregroundVertices, c2.x, c2.y, c3.x, c3.y, scale, obbColor);
    //     addLine(m->foregroundVertices, c3.x, c3.y, c0.x, c0.y, scale, obbColor);
    // }

    for (int i = 0; i < collisionGrid.width; i++)
    {
        float x = collisionGrid.originX + static_cast<float>(i) * cellSize;
        addLine(m->backgroundVertices, x, collisionGrid.originY, x, collisionGrid.originY + gridHeight, scale, color);
    }

    for (int i = 0; i < collisionGrid.height; i++)
    {
        float y = collisionGrid.originY + static_cast<float>(i) * cellSize;
        addLine(m->backgroundVertices, collisionGrid.originX, y, collisionGrid.originX + gridWidth, y, scale, color);
    }

    for (int x = 0; x < collisionGrid.width; x++)
    {
        float xp = collisionGrid.originX + static_cast<float>(x) * cellSize;

        for (int y = 0; y < collisionGrid.height; y++)
        {
            float yp = collisionGrid.originY + static_cast<float>(y) * cellSize;
            CollisionGridCell &cell = collisionGrid.cells[x + y * collisionGrid.width];
            if (cell.numIndices > 0)
            {
                Console::logFrame(xp + cellSize * 0.5f, yp + cellSize * 0.5f, "%d", cell.numIndices);
            }
        }
    }
}

// Styled rendering, used unless debug draw is on

static SDL_FColor hexColor(uint32_t hex, float alpha = 1.0f)
{
    return {static_cast<float>((hex >> 16) & 0xFF) / 255.0f,
            static_cast<float>((hex >> 8) & 0xFF) / 255.0f,
            static_cast<float>(hex & 0xFF) / 255.0f,
            alpha};
}

static SDL_FColor mixColor(SDL_FColor a, SDL_FColor b, float t)
{
    return {a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            a.a + (b.a - a.a) * t};
}

static const uint32_t backgroundColor = 0xF8F5EF;
static const uint32_t gridColor = 0xECE6DC;
static const uint32_t outlineColor = 0x22303F;
static const uint32_t staticFillColor = 0x4B5D73;
static const uint32_t structureFillColor = 0x5BA8AE;
static const uint32_t bodyColors[] = {0xE85D4F, 0xF39A33, 0x4F8FE0, 0x6BBE5E, 0xF4C542};
static const int numBodyColors = sizeof(bodyColors) / sizeof(bodyColors[0]);

// A texel inside the solid white square of the atlas
static const Vector2 solidUv(7.5f / 512.0f, 7.5f / 512.0f);

enum class ShapeStyle
{
    Static,    // Walls and posts
    Structure, // Shapes built from connected parts, like the bridge
    Body       // Free-moving bodies such as balls and boxes
};

static ShapeStyle getShapeStyle(const Shape &shape)
{
    if (shape.isStatic)
    {
        return ShapeStyle::Static;
    }

    if (shape.hasIndices() || shape.parentId != -1)
    {
        return ShapeStyle::Structure;
    }

    return ShapeStyle::Body;
}

static void addTriangle(Array<GameVertex> &vertices, Vector2 p0, Vector2 p1, Vector2 p2, SDL_FColor c0, SDL_FColor c1, SDL_FColor c2)
{
    vertices.push({c0, p0, solidUv});
    vertices.push({c1, p1, solidUv});
    vertices.push({c2, p2, solidUv});
}

// Disc with a world space radius, drawn with the antialiased circle sprite from the atlas
static void addDisc(Array<GameVertex> &vertices, Vector2 center, float radius, SDL_FColor color)
{
    float x0 = center.x - radius;
    float y0 = center.y - radius;
    float x1 = center.x + radius;
    float y1 = center.y + radius;

    vertices.push({color, {x0, y0}, circle.topLeft});
    vertices.push({color, {x1, y0}, circle.topRight});
    vertices.push({color, {x1, y1}, circle.bottomRight});
    vertices.push({color, {x1, y1}, circle.bottomRight});
    vertices.push({color, {x0, y1}, circle.bottomLeft});
    vertices.push({color, {x0, y0}, circle.topLeft});
}

static Vector2 catmullRom(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;
    return (p1 * 2.0f + (p2 - p0) * t + (p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * t2 + (p1 * 3.0f - p0 - p2 * 3.0f + p3) * t3) * 0.5f;
}

static void addJointDot(Array<GameVertex> &vertices, Vector2 pos, float pixel, SDL_FColor outline)
{
    addDisc(vertices, pos, 5.5f * pixel, outline);
    addDisc(vertices, pos, 3.8f * pixel, {1.0f, 1.0f, 1.0f, 1.0f});
}

void GameRenderer::renderStyled(SDL_Renderer *renderer, Game &game)
{
    const float scale = game.scale();
    const Vector2 offset = game.offset();
    PhysicsSpace &space = game.physicsSpace();
    PointMassesRange points = space.points.range();

    m->shadowVertices.clear();
    m->fillVertices.clear();
    m->outlineVertices.clear();
    m->detailVertices.clear();

    SDL_FColor background = hexColor(backgroundColor);
    SDL_SetRenderDrawColorFloat(renderer, background.r, background.g, background.b, 1.0f);
    SDL_RenderClear(renderer);
    renderGrid(renderer, game, scale, 1.0f, hexColor(gridColor));

    // Sizes given in screen pixels are converted to world units, so they stay the same when zooming
    const float pixel = 1.0f / scale;
    const Vector2 shadowOffset(3.0f * pixel, 7.0f * pixel);
    const SDL_FColor shadow = {0.0f, 0.0f, 0.0f, 0.10f};
    const SDL_FColor outline = hexColor(outlineColor);
    const SDL_FColor white = {1.0f, 1.0f, 1.0f, 1.0f};
    const SDL_FColor black = {0.0f, 0.0f, 0.0f, 1.0f};

    for (int i = 0; i < space.shapes.size(); i++)
    {
        const Shape &shape = space.shapes[i];
        ShapeIndexedRange range(shape);
        const int numPoints = range.size();

        // Two point shapes have no area, so draw them as a beam
        if (numPoints == 2)
        {
            Vector2 p0 = points.pos[range[0]];
            Vector2 p1 = points.pos[range[1]];

            if (!isnan(p0.x) && !isnan(p0.y) && !isnan(p1.x) && !isnan(p1.y))
            {
                addLine(m->shadowVertices, p0.x + shadowOffset.x, p0.y + shadowOffset.y, p1.x + shadowOffset.x, p1.y + shadowOffset.y, scale, shadow, 9.0f);
                addLine(m->outlineVertices, p0.x, p0.y, p1.x, p1.y, scale, outline, 9.0f);
                addLine(m->outlineVertices, p0.x, p0.y, p1.x, p1.y, scale, hexColor(structureFillColor), 5.0f);
                addJointDot(m->detailVertices, p0, pixel, outline);
                addJointDot(m->detailVertices, p1, pixel, outline);
            }

            continue;
        }

        if (numPoints < 3)
        {
            continue;
        }

        Vector2 centroid;
        bool valid = true;

        for (int k = 0; k < numPoints; k++)
        {
            Vector2 pos = points.pos[range[k]];
            valid = valid && !isnan(pos.x) && !isnan(pos.y);
            centroid += pos;
        }

        if (!valid)
        {
            continue;
        }

        centroid /= static_cast<float>(numPoints);

        const ShapeStyle style = getShapeStyle(shape);
        SDL_FColor fill = hexColor(staticFillColor);

        if (style == ShapeStyle::Structure)
        {
            fill = hexColor(structureFillColor);
        }
        else if (style == ShapeStyle::Body)
        {
            fill = hexColor(bodyColors[(i * 7 + 3) % numBodyColors]);
        }

        // Round bodies are drawn through a spline of their points, so they don't look faceted.
        // Only as finely as their size on screen needs: a long edge gets up to four segments, a short one stays as is.
        Array<Vector2> &outlinePoints = m->outlinePoints;
        outlinePoints.clear();
        const bool roundBody = style == ShapeStyle::Body && numPoints >= 8;
        const float edgeLengthPixels = (points.pos[range[1]] - points.pos[range[0]]).length() * scale;
        const int subdivisions = roundBody ? clamp(static_cast<int>(ceilf(edgeLengthPixels / 6.0f)), 1, 4) : 1;
        const bool smooth = subdivisions > 1;

        for (int k = 0; k < numPoints; k++)
        {
            Vector2 p1 = points.pos[range[k]];

            if (!smooth)
            {
                outlinePoints.push(p1);
                continue;
            }

            Vector2 p0 = points.pos[range[(k + numPoints - 1) % numPoints]];
            Vector2 p2 = points.pos[range[(k + 1) % numPoints]];
            Vector2 p3 = points.pos[range[(k + 2) % numPoints]];

            for (int step = 0; step < subdivisions; step++)
            {
                outlinePoints.push(catmullRom(p0, p1, p2, p3, static_cast<float>(step) / static_cast<float>(subdivisions)));
            }
        }

        const int numOutlinePoints = outlinePoints.size();

        if (style == ShapeStyle::Body)
        {
            // Shade from a light spot at the upper left, so bodies look lit and slightly glossy
            float radius = 0.0f;

            for (int k = 0; k < numPoints; k++)
            {
                radius += (points.pos[range[k]] - centroid).length();
            }

            radius /= static_cast<float>(numPoints);

            const Vector2 lightPos = centroid + Vector2(-0.30f, -0.35f) * radius;
            const SDL_FColor lightColor = mixColor(fill, white, 0.35f);
            const SDL_FColor rimColor = mixColor(fill, black, 0.10f);

            for (int k = 0; k < numOutlinePoints; k++)
            {
                Vector2 p0 = outlinePoints[k];
                Vector2 p1 = outlinePoints[(k + 1) % numOutlinePoints];
                addTriangle(m->fillVertices, lightPos, p0, p1, lightColor, rimColor, rimColor);
                addTriangle(m->shadowVertices, centroid + shadowOffset, p0 + shadowOffset, p1 + shadowOffset, shadow, shadow, shadow);
            }

            // Specular highlight on round bodies, unless too small on screen to see
            if (roundBody && radius * scale >= 6.0f)
            {
                addDisc(m->detailVertices, centroid + Vector2(-0.38f, -0.42f) * radius, 0.16f * radius, {1.0f, 1.0f, 1.0f, 0.55f});
            }
        }
        else if (shape.triangleStart >= 0)
        {
            for (int t = shape.triangleStart; t < shape.triangleEnd; t += 3)
            {
                Vector2 p0 = points.pos[space.triangleIndices[t]];
                Vector2 p1 = points.pos[space.triangleIndices[t + 1]];
                Vector2 p2 = points.pos[space.triangleIndices[t + 2]];
                addTriangle(m->fillVertices, p0, p1, p2, fill, fill, fill);
                addTriangle(m->shadowVertices, p0 + shadowOffset, p1 + shadowOffset, p2 + shadowOffset, shadow, shadow, shadow);
            }
        }
        else
        {
            for (int k = 0; k < numOutlinePoints; k++)
            {
                Vector2 p0 = outlinePoints[k];
                Vector2 p1 = outlinePoints[(k + 1) % numOutlinePoints];
                addTriangle(m->fillVertices, centroid, p0, p1, fill, fill, fill);
                addTriangle(m->shadowVertices, centroid + shadowOffset, p0 + shadowOffset, p1 + shadowOffset, shadow, shadow, shadow);
            }
        }

        // Outline. Sharp corners get a disc to round off the join; on round bodies the corners are too shallow to show.
        for (int k = 0; k < numOutlinePoints; k++)
        {
            Vector2 p0 = outlinePoints[k];
            Vector2 p1 = outlinePoints[(k + 1) % numOutlinePoints];
            addLine(m->outlineVertices, p0.x, p0.y, p1.x, p1.y, scale, outline, 2.5f);

            if (!roundBody)
            {
                addDisc(m->outlineVertices, p0, 1.25f * pixel, outline);
            }
        }

        if (style == ShapeStyle::Structure)
        {
            for (int k = 0; k < numPoints; k++)
            {
                addJointDot(m->detailVertices, points.pos[range[k]], pixel, outline);
            }
        }

        // Rivets inset from each corner of static quads
        if (style == ShapeStyle::Static && numPoints == 4)
        {
            const float inset = 13.0f;
            const SDL_FColor rivetCenter = mixColor(fill, outline, 0.45f);

            for (int k = 0; k < numPoints; k++)
            {
                Vector2 corner = points.pos[range[k]];
                Vector2 toNext = points.pos[range[(k + 1) % numPoints]] - corner;
                Vector2 toPrev = points.pos[range[(k + numPoints - 1) % numPoints]] - corner;
                float nextLength = toNext.length();
                float prevLength = toPrev.length();

                if (min(nextLength, prevLength) < inset * 3.0f)
                {
                    continue;
                }

                Vector2 rivetPos = corner + toNext * (inset / nextLength) + toPrev * (inset / prevLength);
                addDisc(m->detailVertices, rivetPos, 5.0f, outline);
                addDisc(m->detailVertices, rivetPos, 3.0f, rivetCenter);
            }
        }
    }

    for (int i = 0; i < space.staticJoints.size(); i++)
    {
        addJointDot(m->detailVertices, points.pos[space.staticJoints[i].pointIndex], pixel, outline);
    }

    renderVertices(renderer, m->backgroundVertices, offset, scale, m->texture);
    renderVertices(renderer, m->shadowVertices, offset, scale, m->texture);
    renderVertices(renderer, m->fillVertices, offset, scale, m->texture);
    renderVertices(renderer, m->outlineVertices, offset, scale, m->texture);
    renderVertices(renderer, m->detailVertices, offset, scale, m->texture);
}
