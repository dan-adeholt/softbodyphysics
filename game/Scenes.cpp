#include "Scenes.h"
#include <cstdio>
#include "Shapes.h"
#include "Game.h"
#include "Physics.h"
#include "../containers/Array.h"
#include "../utils/Console.h"
#include <cstring>
#include <math.h>

void rotateShape(Shape &shape, Array<PointMass> &points, float deg)
{
    ShapeBoundingBox box = calculateShapeBoundingBox(shape, 0, points);

    float width = box.width();
    float height = box.height();
    Vector2 center = box.center();

    for (PointMass &point : points.range(shape))
    {
        // Rotate point.pos.x and point.pos.y by 20 degrees
        point.pos.x -= center.x;
        point.pos.y -= center.y;

        float x = point.pos.x;
        float y = point.pos.y;
        point.pos.x = (point.pos.x * cos(deg)) - (point.pos.y * sin(deg));
        point.pos.y = (point.pos.x * sin(deg)) + (point.pos.y * cos(deg));

        point.pos.x += center.x;
        point.pos.y += center.y;
    }
}

SceneDefinition scenes[] = {
    {"Colliding boxes", [](Game *game)
     {
         float size = 100.0f;
         Vector2 box2Pos(700.0f, 290.0f);
         game->shapes().push(Shapes::createQuad(game->points(), game->springs(), 400.0f, 220.0f, size, size, 1.0f));
         game->shapes().push(Shapes::createQuad(game->points(), game->springs(), 700.0f, 220.0f, size, size, 1.0f));
         Shape &firstBox = game->shapes()[0];
         Shape &secondBox = game->shapes()[1];
         firstBox.name = "Moving box";
         secondBox.name = "Static box";

         for (PointMass &point : game->points().range(firstBox))
         {
             point.velocity.x = 0.1f;
         }

         game->setGravityEnabled(false);
     }},
    {"Circle", [](Game *game)
     {
         //  Shape fallingBox = Shapes::createQuad(game->points(), game->springs(), 400.0f, 40.0f, 100.0f, 100.0f, 1.0f);
         //  game->shapes().push(fallingBox);

         Shape circle = Shapes::createCircle(game->points(), game->springs(), 600.0f, 350.0f, 110.0f, 0.2f);
         game->shapes().push(circle);
         //  game->setGravityEnabled(false);

         Shape side1 = Shapes::createStaticQuad(game->staticPoints(), 149.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         game->staticShapes().push(side1);

         Shape side2 = Shapes::createStaticQuad(game->staticPoints(), 1099.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         game->staticShapes().push(side2);

         Shape side3 = Shapes::createStaticQuad(game->staticPoints(), 149.0f, 690.0f, 1000.0f, 50.0f, 1.0f);
         game->staticShapes().push(side3);

         Shape side4 = Shapes::createStaticQuad(game->staticPoints(), 149.0f, 40.0f, 1000.0f, 50.0f, 1.0f);
         game->staticShapes().push(side4);
     }},
    {"Circle grid", [](Game *game)
     {
         float size = 15.0f;
         float spacing = (size + 4.0f) * 2.0f;
         for (int y = 0; y < 25; y++)
         {
             for (int x = 0; x < 25; x++)
             {
                 Shape circle = Shapes::createCircle(game->points(), game->springs(), spacing + x * spacing, spacing * 2 + y * spacing, size, 0.25f);
                 game->shapes().push(circle);
             }
         }

         game->setGravityEnabled(false);

         //  Shape side1 = Shapes::createStaticQuad(game->staticPoints(), 149.0f, 90.0f, 50.0f, 600.0f, 1.0f);

         //  game->staticShapes().push(side1);

         //  Shape side2 = Shapes::createStaticQuad(game->staticPoints(), 1099.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         //  game->staticShapes().push(side2);
     }},
    {"Bridge", [](Game *game)
     {
         int numSegments = 13;
         float x = 200.0f;
         float y = 340.0f;

         Shape bridge = Shapes::createBridge(game->points(), game->springs(), x, y, 1.0f, numSegments);
         game->shapes().push(bridge);

         game->staticJoints().push({0, Vector2(x, y)});
         game->staticJoints().push({numSegments - 1, Vector2(x + 70.0f * numSegments, y)});
         game->staticJoints().push({numSegments, Vector2(x + 70.0f * numSegments, y + 70.0f)});
         game->staticJoints().push({numSegments * 2 - 1, Vector2(x, y + 70.0f)});

         Shape side1 = Shapes::createStaticQuad(game->staticPoints(), 149.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         game->staticShapes().push(side1);

         Shape side2 = Shapes::createStaticQuad(game->staticPoints(), 1110.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         game->staticShapes().push(side2);

         for (int i = 0; i < 10; i++)
         {
             Shape circle = Shapes::createCircle(game->points(), game->springs(), 440.0f + i * 55.0f, 50.0f, 40.0f, 0.25f);
             game->shapes().push(circle);
         }

         //  Shape fallingBox = Shapes::createQuad(game->points(), game->springs(), 600.0f, 190.0f, 100.0f, 100.0f, 1.0f);
         //  game->shapes().push(fallingBox);
     }},
    {"Box", [](Game *game)
     {
         Shape fallingBox = Shapes::createQuad(game->points(), game->springs(), 400.0f, 140.0f, 100.0f, 100.0f, 1.0f);

         for (PointMass &point : game->points().range(fallingBox))
         {
             point.mass = 1.0f;
         }

         StaticJoint joint = {0, Vector2(400.0f, 140.0f)};
         game->staticJoints().push(joint);
         StaticJoint joint2 = {1, Vector2(500.0f, 140.0f)};
         game->staticJoints().push(joint2);
         game->shapes().push(fallingBox);
     }},
    {"Falling box with shelf", [](Game *game)
     {
         Shape fallingBox = Shapes::createQuad(game->points(), game->springs(), 400.0f, 40.0f, 100.0f, 100.0f, 1.0f);
         Shape shelf = Shapes::createTriangle(game->staticPoints(), 50.0f, 700.0f, 800.0f, 500.0f, 800.0f, 700.0f, 0.0f);

         for (PointMass &point : game->points().range(fallingBox))
         {
             point.mass = 1.0f;
         }

         for (PointMass &point : game->staticPoints().range(shelf))
         {
             point.mass = 10000000.0f;
         }

         game->shapes().push(fallingBox);
         game->staticShapes().push(shelf);
     }},
    {"Line spring", [](Game *game)
     {
         Shape shape = Shapes::createLine(game->points(), game->springs(), 400.0f, 100.0f, 700.0f, 500.0f, 1.0f);
         StaticJoint joint = {0, Vector2(400.0f, 100.0f)};
         game->staticJoints().push(joint);
         game->shapes().push(shape);
     }}};

int SceneDefinition::numScenes = sizeof(scenes) / sizeof(scenes[0]);
SceneDefinition *SceneDefinition::allScenes = scenes;

SceneDefinition *SceneDefinition::getDefinitionFromName(const char *name)
{
    for (int i = 0; i < numScenes; i++)
    {
        if (strcmp(scenes[i].name, name) == 0)
        {
            return &scenes[i];
        }
    }
    return nullptr;
}
