#include "Scenes.h"
#include <cstdio>
#include "Shapes.h"
#include "Game.h"
#include "GameKeyCode.h"
#include "../physics/PhysicsSpace.h"
#include "../physics/CollisionSolver.h"
#include "../containers/Array.h"
#include "../utils/Console.h"
#include <cstring>
#include <math.h>
#include "stdint.h"

#define ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR)))) // Size of a static C-style array. Don't use on pointers!

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
    {"Colliding boxes angle", [](Game *game)
     {
         float size = 100.0f;
         Vector2 box2Pos(700.0f, 290.0f);
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createQuad(space, 639.0f, -100.0f, size, size, 1.0f);
         Shapes::createRoundedQuad(space, 700.0f, 270.0f, size, size, 1.0f);
         Shape &firstBox = space.shapes[0];
         Shape &secondBox = space.shapes[1];

         PointMassesRange firstBoxPoints = space.points.range(firstBox);
          for (Vector2 &velocity : firstBoxPoints.velocity)
          {
              velocity.y = 1.0f;
          }

         for (Vector2 &pos : firstBoxPoints.pos)
         {
             pos = pos.rotate(0.3f);
         }

         GameRenderSettings renderSettings;
         renderSettings.renderSprings = false;
         game->setRenderSettings(renderSettings);

         game->physicsSpace().gravityEnabled = false;
         game->scale() = 3.28f;
         game->offset() = Vector2(-1754.0f, -630.0f);
         game->setPaused();
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
    {"Intersection test", [](Game *game)
     {
         struct TestBox
         {
             const char *name;
             Vector2 pos;
             Vector2 size;
             float mass;
             Vector2 boxVelocity;

             Vector2 point;
             Vector2 velocity;
             int offsetIndex;
             Vector2 offset;
         };

         static TestBox boxes[] = {
             {.name = "Box 1",
              .pos = Vector2(50.0f, 70.0f),
              .size = Vector2(150.0f, 150.0f),
              .mass = 1.0f,
              .boxVelocity = Vector2(0.0f, 0.0025f),
              .point = Vector2(70.0f, 70.0f),
              .velocity = Vector2(0.0f, 0.0035f),
              .offsetIndex = 0,
              .offset = Vector2()},
             {.name = "Box 2",
              .pos = Vector2(250.0f, 70.0f),
              .size = Vector2(150.0f, 150.0f),
              .mass = 1.0f,
              .boxVelocity = Vector2(0.0f, 0.0025f),
              .point = Vector2(70.0f, 70.0f),
              .velocity = Vector2(0.0f, -0.003f),
              .offsetIndex = 0,
              .offset = Vector2()},
             {.name = "Box 3",
              .pos = Vector2(450.0f, 70.0f),
              .size = Vector2(150.0f, 150.0f),
              .mass = 1.0f,
              .boxVelocity = Vector2(0.0f, 0.0025f),
              .point = Vector2(70.0f, 70.0f),
              .velocity = Vector2(0.0f, -0.003f),
              .offsetIndex = 0,
              .offset = Vector2(5.0f, 0.0f)},
             {.name = "Box 4",
              .pos = Vector2(650.0f, 70.0f),
              .size = Vector2(150.0f, 150.0f),
              .mass = 1.0f,
              .boxVelocity = Vector2(0.0055f, 0.0f),
              .point = Vector2(70.0f, 70.0f),
              .velocity = Vector2(0.0f, -0.003f),
              .offsetIndex = 0,
              .offset = Vector2(5.0f, 0.0f)},
             {.name = "Box 1a",
              .pos = Vector2(50.0f, 270.0f),
              .size = Vector2(150.0f, 150.0f),
              .mass = 1.0f,
              .boxVelocity = Vector2(0.0f, 0.0025f),
              .point = Vector2(70.0f, 110.0f),
              .velocity = Vector2(0.0f, 0.0025f),
              .offsetIndex = 0,
              .offset = Vector2()},
             {.name = "Box 2a",
              .pos = Vector2(250.0f, 270.0f),
              .size = Vector2(150.0f, 150.0f),
              .mass = 1.0f,
              .boxVelocity = Vector2(0.0f, 0.0025f),
              .point = Vector2(70.0f, 110.0f),
              .velocity = Vector2(0.0f, -0.0025f),
              .offsetIndex = 0,
              .offset = Vector2()},
             {.name = "Box 3a",
              .pos = Vector2(450.0f, 270.0f),
              .size = Vector2(150.0f, 150.0f),
              .mass = 1.0f,
              .boxVelocity = Vector2(0.0f, 0.0025f),
              .point = Vector2(70.0f, 110.0f),
              .velocity = Vector2(0.0f, -0.0025f),
              .offsetIndex = 3,
              .offset = Vector2(45.0f, 0.0f)},

         };

         PhysicsSpace &space = game->physicsSpace();
         space.gravityEnabled = false;
         space.collisionsEnabled = false;
         space.springsEnabled = false;

         game->setStopPointWhenDragging(false);

         static PointMasses points;
         for (TestBox &box : boxes)
         {
             Shape s = Shapes::createQuad(space, box.pos.x, box.pos.y, box.size.x, box.size.y, box.mass);
             space.points.pos[s.start + box.offsetIndex] += box.offset;

             points.mass.push(box.mass);
             points.pos.push(box.point + box.pos);
             points.velocity.push(box.velocity);
             points.shapePos.push(box.point + box.pos);
             points.shapeOriginalPos.push(box.point + box.pos);
         }

         static PointMasses copy;
         static EdgeStrategy strategy = EdgeStrategy::ClosestSegment;

         game->scheduleFrameCallback(
             [](Game *game, void *data)
             {
                 PhysicsSpace &space = game->physicsSpace();

                 if (game->keyWasPressed(GameKeyCode::I))
                 {
                     strategy = static_cast<EdgeStrategy>(
                         (static_cast<int>(strategy) + 1) % static_cast<int>(EdgeStrategy::NumStrategies));
                 }

                 Console::logFrame(46, 30, "Strategy: %s", edgeStrategyToString(strategy));

                 for (int i = 0; i < ARRAYSIZE(boxes); i++)
                 {
                     TestBox &box = boxes[i];
                     copy.replace(space.points);

                     Shape boxShape = game->physicsSpace().shapes[i];

                     for (int i = boxShape.start; i < boxShape.end; i++)
                     {
                         copy.velocity[i] = box.boxVelocity;
                     }

                     points.pos[i] = box.pos + box.point;
                     points.velocity[i] = box.velocity;
                     PointMassesRange originalBoxRange = space.points.range(boxShape);
                     PointMassesRange boxRange = copy.range(boxShape);
                     PointMassesRange pointRange = points.range(i, i + 1);
                     int vertexHitIndex = -1;
                     int edgeHitIndex = -1;
                     Vector2 hitPoint;
                     float t = 0.0f;
                     Vector2 offsetPoint = box.point + box.pos;
                     CollisionSolver::findEntryEdge(strategy, boxRange, boxRange, offsetPoint, offsetPoint, box.velocity, vertexHitIndex, edgeHitIndex, hitPoint, t);

                     ShapeBoundingBox bbox = CollisionSolver::calculateShapeBoundingBox(0, boxRange);
                     int numCollisions = CollisionSolver::calculateCollisions(copy.range(game->physicsSpace().shapes[i]), pointRange, copy.range(game->physicsSpace().shapes[i]), pointRange, bbox, bbox, false, false, strategy);

                     Console::addDebugVelocityVector(box.point + box.pos, box.velocity, 0xFF00FF);
                     Console::addDebugPoint(box.point + box.pos, 0xFF0000);
                     Console::addDebugPoint(hitPoint, 0x00FF00);
                     Console::addDebugPoint(pointRange.pos[0], 0x0000FF);
                     Console::addDebugVelocityVector(pointRange.pos[0], pointRange.velocity[0], 0xFF00FF);

                     for (int i = 0; i < boxRange.size(); i++)
                     {
                         Vector2 boxPos = boxRange.pos[i];
                         Vector2 boxVel = boxRange.velocity[i];
                         Vector2 boxOriginalPos = originalBoxRange.pos[i];

                         Console::addDebugVelocityVector(boxPos, boxVel, 0xFF00FF);
                         Console::addDebugVelocityVector(boxOriginalPos, box.boxVelocity, 0xFF0099);
                     }

                     //  if (numCollisions > 0)
                     //  {
                     //      Console::log("Collision detected: %s", box.name);
                     //  }
                 }

                 //  Console::logFrame(100, 100, "Test 123123");
             },
             nullptr);
     }},

    {"Intersecting boxes", [](Game *game)
     {
         float size = 70.0f;
         float spacing = (size + 1.0f) * 1.5f;
         PhysicsSpace &space = game->physicsSpace();
         for (int y = 0; y < 2; y++)
         {
             for (int x = 0; x < 1; x++)
             {
                 Shapes::createQuad(space, spacing + x * spacing + y * 3.0f, spacing + y * spacing + x * 0.01f - 430.0f, size, size, 5.5f);
             }
         }

         space.gravityEnabled = true;

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, -50.0f, 755.0f, 1317.0f, 80.0f, 1.0f);

         GameRenderSettings renderSettings;
         renderSettings.renderSprings = false;
         renderSettings.renderPoints = false;
         game->setRenderSettings(renderSettings);
         //   game->runFor(2600, false);

         game->scale() = 15.70f;
         game->offset() = Vector2(-1514.89f, -10402.74f);
         game->runFor(3540, true);
     }},
    {"Intersecting boxes #2", [](Game *game)
     {
         float size = 30.0f;
         float spacing = (size + 1.0f) * 3.0f;
         PhysicsSpace &space = game->physicsSpace();
         for (int y = 3; y < 5; y++)
         {
             float x = 15;

             Shapes::createQuad(space, 150.0f + y * 2.0f, 100.0f + y * spacing, size, size, 0.25f);

             //  if (y < 4)
             //  {

             //  }
         }

         //  space.gravityEnabled = false;

         GameRenderSettings renderSettings;
         renderSettings.renderSprings = false;
         renderSettings.renderPoints = false;
         game->setRenderSettings(renderSettings);

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1.0f, 755.0f, 1217.0f, 80.0f, 1.0f);

         game->scale() = 16.10f;
         game->offset() = Vector2(-2092.0f, -11084.00f);
         game->runFor(1990, true);
     }},
    {"Intersecting boxes #3", [](Game *game)
     {
         float size = 25.0f;
         float spacing = (size + 1.0f) * 1.5f;
         PhysicsSpace &space = game->physicsSpace();
         for (int y = 27; y < 30; y++)
         {
             for (int x = 0; x < 1; x++)
             {
                 Shapes::createQuad(space, spacing + x * spacing + y * 2.0f, spacing + y * spacing + x * 0.01f - 900.0f, size, size, 0.25f);

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

         GameRenderSettings renderSettings;
         renderSettings.renderSprings = false;
         renderSettings.renderPoints = false;
         game->setRenderSettings(renderSettings);
         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, -50.0f, 755.0f, 1317.0f, 80.0f, 1.0f);
         game->runFor(2490, true);

         game->scale() = 8.10f;
         game->offset() = Vector2(0, -5500);
         //  game->offset() = Vector2(-2092.0f, -12084.00f);
     }},
    {"Rounded rect grid", [](Game *game)
     {
         float size = 25.0f;
         float spacing = (size + 1.0f) * 1.5f;
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createStaticQuad(space, -50.0f, 755.0f, 1317.0f, 80.0f, 1.0f);

         for (int y = 0; y < 20; y++)
         {
             for (int x = 0; x < 30; x++)
             {
                 Shapes::createRoundedQuad(space, spacing + x * spacing + y * 2.0f, spacing + y * spacing + x * 0.01f - 900.0f, size, size, 5.5f, 10.0f);
             }
         }

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);

         //   space.gravityEnabled = false;
     }},
    {"Collision grid", [](Game *game)
     {
         float size = 25.0f;
         float spacing = (size + 1.0f) * 1.5f;
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createStaticQuad(space, -50.0f, 755.0f, 1317.0f, 80.0f, 1.0f);

         for (int y = 0; y < 20; y++)
         {
             for (int x = 0; x < 30; x++)
             {
                 Shapes::createQuad(space, spacing + x * spacing + y * 2.0f, spacing + y * spacing + x * 0.01f - 900.0f, size, size, 5.5f, 10.0f);

                 //  if (x < 10)
                 //  {
                 //      for (int i = 0; i < 4; i++)
                 //      {
                 //          space.points.velocity[space.points.size() - 1 - i].x = 0.15f;
                 //      }
                 //  }
             }
         }

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);

         space.gravityEnabled = true;

         game->offset() = Vector2(0, -300);
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
             Shape s = Shapes::createCircle(space, 440.0f + i * 55.0f, 50.0f, 40.0f, 0.25f);
         }

         //  game->scale() = 4.5f;
         //  game->offset() = Vector2(-2150.0f, -1200.0f);
         //  game->setPaused();
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

         for (float &mass : space.points.range(shelf).mass)
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
