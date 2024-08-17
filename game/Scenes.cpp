#include "Scenes.h"
#include <cstdio>
#include "Shapes.h"
#include "Game.h"
#include "Physics.h"
#include "../containers/Array.h"
#include "../utils/Console.h"
#include <cstring>
#include <math.h>

SceneDefinition scenes[] = {
    {"Colliding boxes", [](Game *game)
     {
         float size = 100.0f;
         Vector2 box2Pos(700.0f, 290.0f);
         PhysicsSpace &space = *game->physicsSpace();
         space.shapes.push(Shapes::createQuad(space, 400.0f, 220.0f, size, size, 1.0f));
         space.shapes.push(Shapes::createQuad(space, 700.0f, 220.0f, size, size, 1.0f));
         Shape &firstBox = space.shapes[0];
         Shape &secondBox = space.shapes[1];
         firstBox.name = "Moving box";
         secondBox.name = "Static box";

         PointMassesRange firstBoxPoints = space.points.range(firstBox);
         for (Vector2 &velocity : firstBoxPoints.velocity)
         {
             velocity.x = 0.1f;
         }

         game->physicsSpace()->gravityEnabled = false;
     }},
    {"Circle", [](Game *game)
     {
         //  Shape fallingBox = Shapes::createQuad(space.points, space.springs, 400.0f, 40.0f, 100.0f, 100.0f, 1.0f);
         //  space.shapes.push(fallingBox);
         PhysicsSpace &space = *game->physicsSpace();
         Shape circle = Shapes::createCircle(space, 600.0f, 350.0f, 110.0f, 0.2f);
         space.shapes.push(circle);
         //  game->setGravityEnabled(false);

         Shape side1 = Shapes::createStaticQuad(space, 149.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         space.staticShapes.push(side1);

         Shape side2 = Shapes::createStaticQuad(space, 1099.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         space.staticShapes.push(side2);

         Shape side3 = Shapes::createStaticQuad(space, 149.0f, 690.0f, 1000.0f, 50.0f, 1.0f);
         space.staticShapes.push(side3);

         Shape side4 = Shapes::createStaticQuad(space, 149.0f, 40.0f, 1000.0f, 50.0f, 1.0f);
         space.staticShapes.push(side4);
     }},
    {"Collision grid", [](Game *game)
     {
         float size = 7.0f;
         float spacing = (size + 1.0f) * 2.0f;
         PhysicsSpace &space = *game->physicsSpace();
         for (int y = 0; y < 45; y++)
         {
             for (int x = 0; x < 75; x++)
             {
                 Shape quad = Shapes::createQuad(space, spacing + x * spacing, spacing * 2 + y * spacing + x * 0.01f, size, size, 0.25f);
                 space.shapes.push(quad);

                 if (x < 10)
                 {
                     for (int i = 0; i < 4; i++)
                     {
                         space.points.velocity[space.points.size() - 1 - i].x = 0.15f;
                     }
                 }
             }
         }

         space.gravityEnabled = false;

         Shape side1 = Shapes::createStaticQuad(space, 1.0f, 0.0f, 4.0f, 755.0f, 1.0f);
         space.staticShapes.push(side1);

         Shape side2 = Shapes::createStaticQuad(space, 1214.0f, 0.0f, 4.0f, 755.0f, 1.0f);
         space.staticShapes.push(side2);

         Shape side3 = Shapes::createStaticQuad(space, 1.0f, 755.0f, 1217.0f, 4.0f, 1.0f);
         space.staticShapes.push(side3);
     }},
    {"Circle grid", [](Game *game)
     {
         PhysicsSpace &space = *game->physicsSpace();
         float size = 15.0f;
         float spacing = (size + 4.0f) * 2.0f;
         for (int y = 0; y < 35; y++)
         {
             for (int x = 0; x < 35; x++)
             {
                 Shape circle = Shapes::createCircle(space, spacing + x * spacing, spacing * 2 + y * spacing, size, 0.25f);
                 space.shapes.push(circle);
             }
         }

         space.gravityEnabled = false;

         //  Shape side1 = Shapes::createStaticQuad(space.nextStaticShapeIndex(), space.staticPoints, 1.0f, 2.0f, 10.0f, 730.0f, 1.0f);

         //  game->staticShapes().push(side1);

         //  Shape side2 = Shapes::createStaticQuad(space.nextStaticShapeIndex(), space.staticPoints, 1200.0f, 2.0f, 10.0f, 730.0f, 1.0f);

         //  game->staticShapes().push(side2);

         //  Shape side3 = Shapes::createStaticQuad(space.nextStaticShapeIndex(), space.staticPoints, 1.0f, 735.0f, 1200.0f, 10.0f, 1.0f);

         //  game->staticShapes().push(side3);
     }},
    {"Bridge", [](Game *game)
     {
         int numSegments = 13;
         float x = 200.0f;
         float y = 340.0f;
         PhysicsSpace &space = *game->physicsSpace();
         Shape bridge = Shapes::createBridge(space, x, y, 1.0f, numSegments);
         space.shapes.push(bridge);

         space.staticJoints.push({0, Vector2(x, y)});
         space.staticJoints.push({numSegments - 1, Vector2(x + 70.0f * numSegments, y)});
         space.staticJoints.push({numSegments, Vector2(x + 70.0f * numSegments, y + 70.0f)});
         space.staticJoints.push({numSegments * 2 - 1, Vector2(x, y + 70.0f)});

         Shape side1 = Shapes::createStaticQuad(space, 149.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         space.staticShapes.push(side1);

         Shape side2 = Shapes::createStaticQuad(space, 1110.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         space.staticShapes.push(side2);

         for (int i = 0; i < 10; i++)
         {
             Shape circle = Shapes::createCircle(space, 440.0f + i * 55.0f, 50.0f, 40.0f, 0.25f);
             space.shapes.push(circle);
         }

         //  Shape fallingBox = Shapes::createQuad(space.points, space.springs, 600.0f, 190.0f, 100.0f, 100.0f, 1.0f);
         //  space.shapes.push(fallingBox);
     }},
    {"Box", [](Game *game)
     {
         PhysicsSpace &space = *game->physicsSpace();
         Shape fallingBox = Shapes::createQuad(space, 400.0f, 140.0f, 100.0f, 100.0f, 1.0f);

         for (float &mass : space.points.range(fallingBox).mass)
         {
             mass = 1.0f;
         }

         StaticJoint joint = {0, Vector2(400.0f, 140.0f)};
         space.staticJoints.push(joint);
         StaticJoint joint2 = {1, Vector2(500.0f, 140.0f)};
         space.staticJoints.push(joint2);
         space.shapes.push(fallingBox);
     }},
    {"Falling box with shelf", [](Game *game)
     {
         PhysicsSpace &space = *game->physicsSpace();
         Shape fallingBox = Shapes::createQuad(space, 400.0f, 40.0f, 100.0f, 100.0f, 1.0f);
         Shape shelf = Shapes::createTriangle(space, true, 50.0f, 700.0f, 800.0f, 500.0f, 800.0f, 700.0f, 0.0f);

         for (float &mass : space.points.range(fallingBox).mass)
         {
             mass = 1.0f;
         }

         for (float &mass : space.staticPoints.range(shelf).mass)
         {
             mass = 10000000.0f;
         }

         space.shapes.push(fallingBox);
         space.staticShapes.push(shelf);
     }},
    {"Line spring", [](Game *game)
     {
         PhysicsSpace &space = *game->physicsSpace();
         Shape shape = Shapes::createLine(space, 400.0f, 100.0f, 700.0f, 500.0f, 1.0f);
         StaticJoint joint = {0, Vector2(400.0f, 100.0f)};
         space.staticJoints.push(joint);
         space.shapes.push(shape);
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
