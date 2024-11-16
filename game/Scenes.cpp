#include "Scenes.h"
#include <cstdio>
#include "Shapes.h"
#include "Game.h"
#include "PhysicsSpace.h"
#include "../containers/Array.h"
#include "../utils/Console.h"
#include <cstring>
#include <math.h>

SceneDefinition scenes[] = {
    {"Colliding boxes", [](Game *game)
     {
         float size = 100.0f;
         Vector2 box2Pos(700.0f, 290.0f);
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createQuad(space, 400.0f, 220.0f, size, size, 1.0f);
         Shapes::createQuad(space, 700.0f, 220.0f, size, size, 1.0f);
         Shape &firstBox = space.shapes[0];
         Shape &secondBox = space.shapes[1];

         PointMassesRange firstBoxPoints = space.points.range(firstBox);
         for (Vector2 &velocity : firstBoxPoints.velocity)
         {
             velocity.x = 0.1f;
         }

         game->physicsSpace().gravityEnabled = false;
     }},
    {"Stuck quads", [](Game *game)
     {
         float size = 100.0f;
         Vector2 box2Pos(700.0f, 290.0f);
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createQuad(space, 400.0f, 220.0f, size, size, 1.0f);
         Shapes::createQuad(space, 440.0f, 225.0f, size, size, 1.0f);
         Shape &firstBox = space.shapes[0];
         Shape &secondBox = space.shapes[1];

         game->physicsSpace().gravityEnabled = false;
     }},
    {"Colliding circles", [](Game *game)
     {
         float size = 70.0f;
         Vector2 box2Pos(700.0f, 290.0f);
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createCircle(space, 400.0f, 320.0f, size, 1.0f);
         Shapes::createCircle(space, 800.0f, 320.5f, size, 1.0f);
         Shape &firstBox = space.shapes[0];
         Shape &secondBox = space.shapes[1];

         Shapes::createStaticQuad(space, 1099.0f, 90.0f, 50.0f, 600.0f, 1.0f);

         PointMassesRange firstBoxPoints = space.points.range(firstBox);
         for (Vector2 &velocity : firstBoxPoints.velocity)
         {
             velocity.x = 3.5f;
             //  velocity.y = 0.01f;
         }

         game->physicsSpace().gravityEnabled = false;
     }},
    {"Circle", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createCircle(space, 600.0f, 350.0f, 110.0f, 0.2f);

         Shapes::createStaticQuad(space, 149.0f, 91.0f, 50.0f, 598.0f, 1.0f);
         Shapes::createStaticQuad(space, 1099.0f, 91.0f, 50.0f, 598.0f, 1.0f);
         Shapes::createStaticQuad(space, 149.0f, 690.0f, 1000.0f, 50.0f, 1.0f);
         Shapes::createStaticQuad(space, 149.0f, 40.0f, 1000.0f, 50.0f, 1.0f);
         space.gravityEnabled = false;
     }},
    {"Stacked boxes", [](Game *game)
     {
         float size = 30.0f;
         float spacing = (size + 1.0f) * 3.0f;
         PhysicsSpace &space = game->physicsSpace();
         for (int y = 0; y < 5; y++)
         {
             float x = 15;

             Shapes::createQuad(space, 150.0f + y * 2.0f, 100.0f + y * spacing, size, size, 0.25f);

             //  if (y < 4)
             //  {

             //  }
         }

         //  space.gravityEnabled = false;

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1.0f, 755.0f, 1217.0f, 80.0f, 1.0f);
     }},

    {"Inside shape", [](Game *game)
     {
         float size = 7.0f;
         float spacing = (size + 1.0f) * 2.0f;
         PhysicsSpace &space = game->physicsSpace();
         float mass = 0.25f;
         Shapes::createQuad(space, 200.0f, 200.0f, 50.0f, 50.0f, mass);

         Shapes::createQuad(space, 100.0f, 100.0f, 500.0f, 500.0f, mass);

         game->scheduleCallback([](Game *game, void *data)
                                {
            PhysicsSpace &space = game->physicsSpace();   
            for (int i = 0; i < space.points.size(); i++)
            {
                space.points.velocity[i].y = 0.00002f;
            } }, nullptr, 1000.0f);

         space.gravityEnabled = false;
     }},
    {"Crusher", [](Game *game)
     {
         float size = 170.0f;
         float spacing = (size + 1.0f) * 2.0f;
         PhysicsSpace &space = game->physicsSpace();

         Shapes::createQuad(space, 400.0f, 460.0f, 480.0f, 80.0f, 15.0f);
         Shapes::createQuad(space, 600.0f, 660.0f, 80.0f, 80.0f, 0.25f);
         //  for (int y = 0; y < 32; y++)
         //  {
         //      for (int x = 0; x < 7; x++)
         //      {
         //          Shapes::createQuad(space, spacing + x * spacing + y * 1.0f + 200.0f, spacing + y * spacing + x * 0.01f, size, size, 0.25f);

         //          //  if (x < 10)
         //          //  {
         //          //      for (int i = 0; i < 4; i++)
         //          //      {
         //          //          space.points.velocity[space.points.size() - 1 - i].x = 0.15f;
         //          //      }
         //          //  }
         //      }
         //  }

         space.gravityEnabled = true;

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 30.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 30.0f);
         Shapes::createStaticQuad(space, 1.0f, 755.0f, 1217.0f, 80.0f, 30.0f);
     }},
    {"Collision grid", [](Game *game)
     {
         float size = 14.0f;
         float spacing = (size + 1.0f) * 1.5f;
         PhysicsSpace &space = game->physicsSpace();
         for (int y = 0; y < 75; y++)
         {
             for (int x = 0; x < 50; x++)
             {
                 Shapes::createQuad(space, spacing + x * spacing + y * 1.0f, spacing + y * spacing + x * 0.01f, size, size, 0.25f);

                 //  if (x < 10)
                 //  {
                 //      for (int i = 0; i < 4; i++)
                 //      {
                 //          space.points.velocity[space.points.size() - 1 - i].x = 0.15f;
                 //      }
                 //  }
             }
         }

         space.gravityEnabled = true;

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, -50.0f, 755.0f, 1317.0f, 80.0f, 1.0f);
     }},
    {"Circle grid", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         float size = 15.0f;
         float spacing = (size + 4.0f) * 2.0f;
         for (int y = 0; y < 35; y++)
         {
             for (int x = 0; x < 35; x++)
             {
                 Shapes::createCircle(space, spacing + x * spacing, spacing * 2 + y * spacing, size, 0.25f);
             }
         }

         space.gravityEnabled = false;

         //  Shapes::createStaticQuad(space.nextStaticShapeIndex(), space.staticPoints, 1.0f, 2.0f, 10.0f, 730.0f, 1.0f);
         //  Shapes::createStaticQuad(space.nextStaticShapeIndex(), space.staticPoints, 1200.0f, 2.0f, 10.0f, 730.0f, 1.0f);
         //  Shapes::createStaticQuad(space.nextStaticShapeIndex(), space.staticPoints, 1.0f, 735.0f, 1200.0f, 10.0f, 1.0f);
     }},
    {"Bridge", [](Game *game)
     {
         int numSegments = 13;
         float x = 200.0f;
         float y = 340.0f;
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createBridge(space, x, y, 1.0f, numSegments);
         space.staticJoints.push({0, Vector2(x, y)});
         space.staticJoints.push({numSegments - 1, Vector2(x + 70.0f * numSegments, y)});
         space.staticJoints.push({numSegments, Vector2(x + 70.0f * numSegments, y + 70.0f)});
         space.staticJoints.push({numSegments * 2 - 1, Vector2(x, y + 70.0f)});

         Shapes::createStaticQuad(space, 149.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         Shapes::createStaticQuad(space, 1110.0f, 90.0f, 50.0f, 600.0f, 1.0f);

         for (int i = 0; i < 10; i++)
         {
             Shapes::createCircle(space, 440.0f + i * 55.0f, 50.0f, 40.0f, 0.25f);
         }

         //  Shapes::createQuad(space.points, space.springs, 600.0f, 190.0f, 100.0f, 100.0f, 1.0f);
     }},
    {"Box", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createQuad(space, 400.0f, 140.0f, 100.0f, 100.0f, 1.0f);

         StaticJoint joint = {0, Vector2(400.0f, 140.0f)};
         space.staticJoints.push(joint);
         StaticJoint joint2 = {1, Vector2(500.0f, 140.0f)};
         space.staticJoints.push(joint2);
     }},
    {"Falling box with shelf", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createQuad(space, 400.0f, 40.0f, 100.0f, 100.0f, 1.0f);
         Shape shelf = Shapes::createTriangle(space, true, 50.0f, 700.0f, 800.0f, 500.0f, 800.0f, 700.0f, 0.0f);

         for (float &mass : space.staticPoints.range(shelf).mass)
         {
             mass = 10000000.0f;
         }
     }},
    {"Line spring", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createLine(space, 400.0f, 100.0f, 700.0f, 500.0f, 1.0f);
         StaticJoint joint = {0, Vector2(400.0f, 100.0f)};
         space.staticJoints.push(joint);
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
