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
#include "./Textures.h"

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

        for (int i = 0; i < TextureType::NUM_TEXTURE_TYPES; i++)
        {
            SDL_Surface *surfaceTexture = loadImage(textureLookup[i].path);
            textures[i] = SDL_CreateTextureFromSurface(renderer, surfaceTexture);
            SDL_SetTextureBlendMode(textures[i], SDL_BLENDMODE_BLEND);
            SDL_DestroySurface(surfaceTexture);
        }
    }

    SDL_Texture *texture;

    SDL_Texture *textures[TextureType::NUM_TEXTURE_TYPES];
    Array<GameVertex> foregroundVertices;
    Array<GameVertex> backgroundVertices;
    Array<GameVertex> verticesByTex[TextureType::NUM_TEXTURE_TYPES];
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
AtlasCoordinate fabricData(64, 0, 128, 128);

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
    renderGrid(renderer, game, scale);
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
    else
    {
        for (int i = 0; i < physicsSpace.shapes.size(); i++)
        {
            const Shape &shape = physicsSpace.shapes[i];
            SDL_FColor color = {1.0f, 1.0f, 1.0f, 1.0f};

            for (int i = shape.triangleStart; i < shape.triangleEnd; i += 3)
            {
                int p0 = physicsSpace.triangleIndices[i];
                int p1 = physicsSpace.triangleIndices[i + 1];
                int p2 = physicsSpace.triangleIndices[i + 2];

                Vector2 p0Pos = points.pos[p0];
                Vector2 p1Pos = points.pos[p1];
                Vector2 p2Pos = points.pos[p2];

                Vector2 uv0 = physicsSpace.uvCoordinates[i];
                Vector2 uv1 = physicsSpace.uvCoordinates[i + 1];
                Vector2 uv2 = physicsSpace.uvCoordinates[i + 2];

                m->verticesByTex[shape.texture].push({color,
                                                      {p0Pos.x, p0Pos.y},
                                                      uv0});
                m->verticesByTex[shape.texture].push({color,
                                                      {p1Pos.x, p1Pos.y},
                                                      uv1});
                m->verticesByTex[shape.texture].push({color,
                                                      {p2Pos.x, p2Pos.y},
                                                      uv2});
            }
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

    for (int i = 0; i < TextureType::NUM_TEXTURE_TYPES; i++)
    {
        renderVertices(renderer, m->verticesByTex[i], offset, scale, m->textures[i]);
        m->verticesByTex[i].clear();
    }

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

void GameRenderer::renderGrid(SDL_Renderer *renderer, Game &game, float scale)
{
    float gray = 0.941f;
    SDL_FColor color = {gray, gray, gray, 1.0f};

    for (int i = 0; i < 200; i++)
    {
        float x = -2000.0f + static_cast<float>(i) * gridSize;
        addLine(m->backgroundVertices, x, -10000, x, 10000, scale, color);
    }

    for (int i = 0; i < 200; i++)
    {
        float y = -2000.0f + static_cast<float>(i) * gridSize;
        addLine(m->backgroundVertices, -10000, y, 10000, y, scale, color);
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
