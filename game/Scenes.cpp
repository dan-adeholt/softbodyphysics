#include "Scenes.h"
#include <stdio.h>
#include "Shapes.h"
#include "Game.h"
#include "GameKeyCode.h"
#include "../physics/PhysicsSpace.h"
#include "../physics/PhysicsSpaceStorage.h"
#include "../physics/CollisionSolver.h"
#include "../physics/ShapeAxisSeparator.h"
#include "../physics/ShapeUtils.h"
#include "../containers/Array.h"
#include "../utils/Console.h"
#include "../utils/MinMax.h"
#include <string.h>
#include <math.h>
#include "stdint.h"

#define ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR)))) // Size of a static C-style array. Don't use on pointers!

namespace
{
    struct TankRecoveryState
    {
        int parentId = -1;
        int hullShapeIndex = -1;
        float controlLockTimerMs = 0.0f;
    };

    static void setTankMotorCommand(PhysicsSpace &space, int parentId, float command)
    {
        for (int i = 0; i < space.wheelMotors.size(); i++)
        {
            WheelMotor &wheelMotor = space.wheelMotors[i];
            if (wheelMotor.parentId != parentId)
            {
                continue;
            }

            wheelMotor.enabled = command != 0.0f;
            wheelMotor.command = command;
        }
    }

    static Vector2 averageParentPosition(const PhysicsSpace &space, int parentId, bool useOriginalPositions)
    {
        Vector2 center;
        int count = 0;

        for (int i = 0; i < space.shapes.size(); i++)
        {
            const Shape &shape = space.shapes[i];
            if (shape.parentId != parentId)
            {
                continue;
            }

            for (ShapeIterator iter(shape); iter.isValid(); iter.next())
            {
                int pointIndex = iter.index();
                center += useOriginalPositions ? space.points.shapeOriginalPos[pointIndex] : space.points.pos[pointIndex];
                count++;
            }
        }

        return count > 0 ? center / (float)count : Vector2();
    }

    static void resetTankPose(PhysicsSpace &space, const TankRecoveryState &state, float angle)
    {
        const Vector2 currentCenter = averageParentPosition(space, state.parentId, false);
        const Vector2 originalCenter = averageParentPosition(space, state.parentId, true);
        const Vector2 targetCenter = currentCenter;

        for (int i = 0; i < space.shapes.size(); i++)
        {
            const Shape &shape = space.shapes[i];
            if (shape.parentId != state.parentId)
            {
                continue;
            }

            for (ShapeIterator iter(shape); iter.isValid(); iter.next())
            {
                int pointIndex = iter.index();
                Vector2 localOffset = space.points.shapeOriginalPos[pointIndex] - originalCenter;
                space.points.pos[pointIndex] = targetCenter + localOffset.rotate(angle);
                space.points.velocity[pointIndex] = Vector2();
            }
        }
    }
}

SceneDefinition deformedScenes[] = {
    {"Failed triangulation", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_44.txt");
         game->scale() = 1.00f;
         game->offset() = Vector2(166.00f, 80.00f);
         game->setPaused();
     }},
    {"Deformed circle 1", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_4.txt");
         game->setPaused();
     }},
    {"Deforrmed circle 2", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_1.txt");
         space.gravityEnabled = true;
         game->setPaused();
     }},

    {"Deformed quad", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_3.txt");

         Shapes::createRoundedQuad(space, 400.0f, 200.0f, 100.0f, 100.0f, 1.0f);

         space.gravityEnabled = false;

         game->setPaused();
     }},

    {"Deformed circle", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_2.txt");

         GameRenderSettings renderSettings;
         renderSettings.renderShapeLines = false;
         game->setRenderSettings(renderSettings);

         Shapes::createCircle(space, 1200.0f, 200.0f, 100.0f, 1.0f);
         space.gravityEnabled = false;
         game->offset() = Vector2(200.0f, 0.0f);

         game->setPaused();
     }},
};

SceneDefinition collisionScenes[] = {
    {"New Unit Test 45", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_45.txt");

         GameRenderSettings renderSettings;
         renderSettings.renderShapeMatching = true;
         game->setRenderSettings(renderSettings);
         game->scale() = 1.00f;
         game->offset() = Vector2(-41.44f, 568.12f);
         game->setPaused();
     }},
    {"New Unit Test 32", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_32.txt");
         game->scale() = 0.85f;
         game->offset() = Vector2(91.30f, 722.68f);
         game->setPaused();
     }},

    {"New Unit Test 29", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_29.txt");
         space.gravityEnabled = false;
         GameRenderSettings renderSettings;
         renderSettings.renderSprings = false;
         renderSettings.renderShapeMatching = true;
         game->setRenderSettings(renderSettings);
         for (int i = 0; i < space.points.velocity.size(); i++)
         {
             space.points.velocity[i] = Vector2();
         }
         game->scale() = 1.00f;
         game->offset() = Vector2(400.00f, 0.00f);
         game->setPaused();
     }},

    {"New Unit Test 28", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_28.txt");
         game->scale() = 1.00f;
         game->offset() = Vector2(-231.00f, -560.00f);
         game->setPaused();
     }},
    {"New Unit Test 24", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_24.txt");
         GameRenderSettings renderSettings;
         renderSettings.renderVelocityVectors = true;
         renderSettings.renderSprings = false;
         renderSettings.renderShapeMatching = false;
         renderSettings.renderPointIndices = true;
         game->setRenderSettings(renderSettings);
         game->setPaused();
     }},
    {"New Unit Test 21", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_21.txt");
         space.gravityEnabled = false;
         //  space.collisionsEnabled = false;
         game->setPaused();
     }},

    {"New Unit Test 19", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_19.txt");
         game->setPaused();
     }},

    {"New Unit Test 16", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_16.txt");
         game->setPaused();
     }},

    {"New Unit Test 15", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_15.txt");
         space.gravityEnabled = false;
         game->setPaused();
     }},

    {"New Unit Test 13", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_13.txt");
         space.gravityEnabled = false;
         game->offset() = Vector2(-300.0f, 504.0f);
         game->setPaused();
     }},

    {"New Unit Test 11", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_11.txt");
         game->offset() = Vector2(200.0f, 304.0f);

         game->runFor(41, true);
     }},

    {"UNFIXED New Unit Test 8", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_8.txt");
         game->setPaused();
     }},
    {"UNFIXED New Unit Test 7", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_7.txt");
         game->offset() = Vector2(-580.0f, 304.0f);

         game->runFor(57, true);
     }},

    {"New Unit Test 6", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_6.txt");
         game->offset() = Vector2(-680.0f, -4704.0f);
         game->scale() = 7.85f;
         game->runFor(42, true);
         game->setPaused();
     }},

    {"New Unit Test 5", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_5.txt");
         game->offset() = Vector2(-680.0f, -4704.0f);
         game->scale() = 7.85f;
         game->runFor(33, true);
     }},
    {"Line split", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();

         GameRenderSettings renderSettings;
         renderSettings.renderSprings = false;
         game->setRenderSettings(renderSettings);

         Shapes::createQuad(space, 200.0f, 200.0f, 100.0f, 100.0f, 1.0f);
         Shapes::createQuad(space, 210.0f, 180.0f, 80.0f, 70.0f, 1.0f);
         int lastPoint = space.points.size() - 1;
         space.points.pos[lastPoint].x -= 20.0f;
         space.points.pos[lastPoint - 1].x += 20.0f;

         space.gravityEnabled = false;
         space.collisionsEnabled = false;
         space.springsEnabled = false;

         game->setPaused();
         static bool separated = false;

         game->scheduleFrameCallback(
             [](Game *game, void *data)
             {
                 PhysicsSpace &space = game->physicsSpace();

                 if (separated)
                 {
                     return;
                 }
                 separated = true;
                 Shape &shape = space.shapes[0];
                 Shape &otherShape = space.shapes[1];

                 PointMassesRange range = space.points.range();

                 ShapeAxisSeparator::separateShapesFromIntersectionAxis(range, shape, otherShape);
             },
             nullptr);
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
         game->setPaused();
     }},
    {"Boxes overlapping", [](Game *game)
     {
         ShapeEntry shape0;
         shape0.isStatic = false;
         shape0.points.replace(PointMasses()
                               .withPos({
                                   {150.000000f, 668.858093f},
                                   {180.000000f, 668.858093f},
                                   {180.000000f, 698.858154f},
                                   {150.000000f, 698.858154f},
                               })
                               .withVelocity({
                                   {-0.000001f, 0.877949f},
                                   {0.000001f, 0.877949f},
                                   {0.000001f, 0.877954f},
                                   {-0.000001f, 0.877954f},
                               })
                               .withMass({
                                   0.250000f,
                                   0.250000f,
                                   0.250000f,
                                   0.250000f,
                               })
                               .withShapeOriginalPos({
                                   {-15.000000f, -15.000000f},
                                   {15.000000f, -15.000000f},
                                   {15.000000f, 15.000000f},
                                   {-15.000000f, 15.000000f},
                               }));

         ShapeEntry shape1;
         shape1.isStatic = false;
         shape1.points.replace(PointMasses()
                               .withPos({
                                   {148.365570f, 699.373230f},
                                   {176.053162f, 700.380371f},
                                   {176.345566f, 736.231323f},
                                   {152.611252f, 727.010132f},
                               })
                               .withVelocity({
                                   {0.061911f, -0.113225f},
                                   {0.085712f, 0.114880f},
                                   {-0.168945f, -0.115960f},
                                   {-0.036014f, 0.112005f},
                               })
                               .withMass({
                                   0.250000f,
                                   0.250000f,
                                   0.250000f,
                                   0.250000f,
                               })
                               .withShapeOriginalPos({
                                   {-15.000000f, -15.000000f},
                                   {15.000000f, -15.000000f},
                                   {15.000000f, 15.000000f},
                                   {-15.000000f, 15.000000f},
                               }));
         Array<ShapeEntry> shapes = {
             shape0,
             shape1,
         };
         PhysicsSpace &space = game->physicsSpace();
         space.initFromEntries(shapes);
         space.gravityEnabled = false;
         game->setPaused();
         game->offset() = Vector2(-600.0f, -5310.0f);
         game->scale() = 8.0f;
     }},
    {"Colliding boxes", [](Game *game)
     {
         float size = 100.0f;
         Vector2 box2Pos(700.0f, 290.0f);
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createQuad(space, 400.0f, 220.0f, size, size, 1.0f);
         Shapes::createQuad(space, 700.0f, 220.0f, size, size, 1.0f);
         Shape &firstBox = space.shapes[0];
         Shape &secondBox = space.shapes[1];

         ShapeIndexedRange firstBoxRange = ShapeIndexedRange(firstBox);

         for (int i = 0; i < firstBoxRange.size(); i++)
         {
             space.points.velocity[firstBoxRange[i]].x = 0.3f;
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

         ShapeIndexedRange firstBoxRange = ShapeIndexedRange(firstBox);

         for (int i = 0; i < firstBoxRange.size(); i++)
         {
             space.points.velocity[firstBoxRange[i]].y = 1.0f;
             space.points.pos[firstBoxRange[i]] = space.points.pos[firstBoxRange[i]].rotate(0.3f);
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
    {"Intersecting boxes", [](Game *game)
     {
         float size = 70.0f;
         float spacing = (size + 1.0f) * 1.5f;
         PhysicsSpace &space = game->physicsSpace();
         for (int y = 0; y < 2; y++)
         {
             for (int x = 0; x < 1; x++)
             {
                 float xf = static_cast<float>(x);
                 float yf = static_cast<float>(y);
                 Shapes::createQuad(space, spacing + xf * spacing + yf * 3.0f, spacing + yf * spacing + xf * 0.01f - 430.0f, size, size, 5.5f);
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
         game->runFor(3640, true);
     }},
    {"Intersecting boxes #2", [](Game *game)
     {
         float size = 30.0f;
         float spacing = (size + 1.0f) * 3.0f;
         PhysicsSpace &space = game->physicsSpace();
         for (int y = 3; y < 5; y++)
         {
             float yf = static_cast<float>(y);
             Shapes::createQuad(space, 150.0f + yf * 2.0f, 100.0f + yf * spacing, size, size, 0.25f);

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
                 float xf = static_cast<float>(x);
                 float yf = static_cast<float>(y);
                 Shapes::createQuad(space, spacing + xf * spacing + yf * 2.0f, spacing + yf * spacing + xf * 0.01f - 900.0f, size, size, 0.25f);

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
         game->offset() = Vector2(250, -5500);
         //  game->offset() = Vector2(-2092.0f, -12084.00f);
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

         ShapeIndexedRange firstBoxRange(firstBox);

         for (int i = 0; i < firstBoxRange.size(); i++)
         {
             space.points.velocity[firstBoxRange[i]].x = 3.5f;
         }

         game->physicsSpace().gravityEnabled = false;
     }},
    {"Rotation bug", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createStaticQuad(space, 50.0f, 520.0f, 700.0f, 40.0f, 1.0f);
         int wheelParentId = space.nextParentId();
         Shape wheel = Shapes::createCircle(space, 400.0f, 445.0f, 50.0f, 1.0f);
         space.shapes[wheel.index].parentId = wheelParentId;

         WheelMotor wheelMotor;
         wheelMotor.shapeIndex = wheel.index;
         wheelMotor.parentId = wheelParentId;
         wheelMotor.enabled = true;
         wheelMotor.command = 1.0f;
         wheelMotor.targetSurfaceSpeed = 0.18f;
         wheelMotor.maxDriveImpulsePerStep = 0.06f;
         wheelMotor.maxBrakeImpulsePerStep = 0.18f;
         wheelMotor.reverseEngageSpeed = 0.04f;
         wheelMotor.freeSpinDamping = 0.01f;
         space.wheelMotors.push(wheelMotor);

         GameRenderSettings renderSettings;
         renderSettings.renderShapeMatching = true;
         game->setRenderSettings(renderSettings);

         game->scale() = 1.2f;
         game->offset() = Vector2(50.0f, 50.0f);
     }},
};

SceneDefinition shapeScenes[] = {
    {"Bridge loose", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/unit_31.txt");
         game->scale() = 1.00f;
         game->offset() = Vector2(-109.00f, 69.00f);
     }},

    {"Mesh", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createMesh(space, 400.0f, 250.0f, 400.0f, 400.0f, 10, 44.0f);

         Shapes::createStaticQuad(space, 149.0f, 91.0f, 50.0f, 598.0f, 1.0f);
         Shapes::createStaticQuad(space, 1099.0f, 91.0f, 50.0f, 598.0f, 1.0f);
         Shapes::createStaticQuad(space, 149.0f, 690.0f, 1000.0f, 650.0f, 1.0f);
         Shapes::createStaticQuad(space, 149.0f, 40.0f, 1000.0f, 50.0f, 1.0f);
         space.gravityEnabled = true;
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
    {"Rounded rect", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createRoundedQuad(space, 600.0f, 350.0f, 110.0f, 110.0f, 0.2f);

         Shapes::createStaticQuad(space, 149.0f, 91.0f, 50.0f, 598.0f, 1.0f);
         Shapes::createStaticQuad(space, 1099.0f, 91.0f, 50.0f, 598.0f, 1.0f);
         Shapes::createStaticQuad(space, 149.0f, 690.0f, 1000.0f, 50.0f, 1.0f);
         Shapes::createStaticQuad(space, 149.0f, 40.0f, 1000.0f, 50.0f, 1.0f);
         space.gravityEnabled = false;
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
    {"Line spring", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createLine(space, 400.0f, 100.0f, 700.0f, 500.0f, 1.0f);
         StaticJoint joint = {0, Vector2(400.0f, 100.0f)};
         space.staticJoints.push(joint);
     }},
    {"Falling box with shelf", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createQuad(space, 400.0f, 40.0f, 100.0f, 100.0f, 1.0f);
         Shape shelf = Shapes::createTriangle(space, true, 50.0f, 700.0f, 800.0f, 500.0f, 800.0f, 700.0f, 0.0f);

         ShapeIndexedRange range(shelf);
         for (int i = 0; i < range.size(); i++)
         {
             space.points.mass[range[i]] = 10000000.0f;
         }
     }},
    {"Crusher", [](Game *game)
     {
         float size = 170.0f;
         float spacing = (size + 1.0f) * 2.0f;
         PhysicsSpace &space = game->physicsSpace();

         Shapes::createQuad(space, 400.0f, 460.0f, 480.0f, 80.0f, 15.0f);
         Shapes::createQuad(space, 600.0f, 660.0f, 80.0f, 80.0f, 0.25f);

         space.gravityEnabled = true;

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 30.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 30.0f);
         Shapes::createStaticQuad(space, 1.0f, 755.0f, 1217.0f, 80.0f, 30.0f);
     }},
};

SceneDefinition collectionScenes[] = {
    {"Stacked boxes", [](Game *game)
     {
         float size = 30.0f;
         float spacing = (size + 1.0f) * 3.0f;
         PhysicsSpace &space = game->physicsSpace();
         for (int y = 0; y < 20; y++)
         {
             float yf = static_cast<float>(y);
             Shapes::createQuad(space, 150.0f + yf * 2.0f, -1900.0f + yf * spacing, size, size, 0.25f);

             //  if (y < 4)
             //  {

             //  }
         }

         //  space.gravityEnabled = false;

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1.0f, 755.0f, 1217.0f, 80.0f, 1.0f);

         //  game->scale() = 4.14f;
         game->offset() = Vector2(400.0f, -400.0f);
         //  game->runFor(2430, true);
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
                 float xf = static_cast<float>(x);
                 float yf = static_cast<float>(y);
                 Shapes::createRoundedQuad(space, spacing + xf * spacing + yf * 2.0f, spacing + yf * spacing + xf * 0.01f - 900.0f, size, size, 1.0f);
             }
         }

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);

         //   space.gravityEnabled = false;
     }},
    {"Falling boxes", [](Game *game)
     {
         float size = 25.0f;
         float spacing = (size + 1.0f) * 1.5f;
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createStaticQuad(space, -50.0f, 755.0f, 1317.0f, 80.0f, 1.0f);

         for (int y = 0; y < 20; y++)
         {
             for (int x = 0; x < 30; x++)
             {
                 float xf = static_cast<float>(x);
                 float yf = static_cast<float>(y);
                 Shapes::createQuad(space, spacing + xf * spacing + yf * 2.0f, spacing + yf * spacing + xf * 0.01f - 900.0f, size, size, 1.0f);
             }
         }

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);

         space.gravityEnabled = true;

         //  game->offset() = Vector2(-200, -200);
         game->offset() = Vector2(400, 0);
     }},
    {"Falling circles", [](Game *game)
     {
         float size = 15.0f;
         float spacing = (size + 1.0f) * 2.5f;
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createStaticQuad(space, -50.0f, 755.0f, 1317.0f, 80.0f, 1.0f);

         for (int y = 0; y < 12; y++)
         {
             for (int x = 0; x < 25; x++)
             {
                 float xf = static_cast<float>(x);
                 float yf = static_cast<float>(y);
                 Shapes::createCircle(space, spacing + xf * spacing + yf * 2.0f, spacing + yf * spacing + xf * 0.01f - 500.0f, size, 1.0f);
             }
         }

         Shapes::createStaticQuad(space, -36.0f, 0.0f, 40.0f, 755.0f, 1.0f);
         Shapes::createStaticQuad(space, 1214.0f, 0.0f, 40.0f, 755.0f, 1.0f);

         space.gravityEnabled = true;

         //  game->offset() = Vector2(-200, -200);
         game->offset() = Vector2(400, 0);
     }},
    {"Circle grid", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         float size = 15.0f;
         float spacing = (size + 4.0f) * 2.0f;
         for (int y = 0; y < 20; y++)
         {
             for (int x = 0; x < 20; x++)
             {
                 float xf = static_cast<float>(x);
                 float yf = static_cast<float>(y);
                 Shapes::createCircle(space, spacing + xf * spacing, spacing * 2.0f + yf * spacing, size, 0.25f);
             }
         }

         space.gravityEnabled = false;
     }},
};

SceneDefinition gameScenes[] = {
    {"Bridge", [](Game *game)
     {
         int numSegments = 13;
         float x = 200.0f;
         float y = 340.0f;
         PhysicsSpace &space = game->physicsSpace();
         float segmentWidth = 70.0f;
         float segmentHeight = 80.0f;
         float bridgeSpan = segmentWidth * static_cast<float>(numSegments);
         Shapes::createBridge(space, x, y, 1.0f, numSegments, segmentWidth, segmentHeight);
         space.staticJoints.push({0, Vector2(x, y)});
         space.staticJoints.push({numSegments, Vector2(x + bridgeSpan, y)});
         space.staticJoints.push({numSegments + 1, Vector2(x + bridgeSpan, y + segmentHeight)});
         space.staticJoints.push({numSegments * 2 + 1, Vector2(x, y + segmentHeight)});

         Shapes::createStaticQuad(space, 149.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         Shapes::createStaticQuad(space, x + bridgeSpan + 1.0f, 90.0f, 50.0f, 600.0f, 1.0f);

         for (int i = 0; i < 10; i++)
         {
             Shape s = Shapes::createCircle(space, 440.0f + static_cast<float>(i) * 55.0f, 50.0f, 40.0f, 1.0f);
         }
     }},
    {"Playground", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "scenedefs/playground.txt");
     }},
};

SceneDefinition vehicleScenes[] = {
    {"Car", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "levels/car_scene.txt");

         PhysicsSpace prefabSpace;
         PhysicsSpaceStorage::loadFromFile(prefabSpace, "levels/prefab_3.txt");
         int carParentId = Shapes::createCar(space, 400.0f, 440.0f, prefabSpace);

         struct CallbackData
         {
             int carParentId;
             float jumpFuel = 1.0f;
         };

         CallbackData *callbackData = new CallbackData();
         callbackData->carParentId = carParentId;
         constexpr float jumpFuelLoss = 0.015f;

         game->scheduleFrameCallback(
             [](Game *game, void *data)
             {
                 PhysicsSpace &space = game->physicsSpace();
                 CallbackData *callbackData = (CallbackData *)data;

                 Console::logFrame(200, 200, "Jump fuel: %.2f", callbackData->jumpFuel);

                 callbackData->jumpFuel += 0.0065f;
                 if (callbackData->jumpFuel > 1.0f)
                 {
                     callbackData->jumpFuel = 1.0f;
                 }

                 if (game->keyWasPressed(GameKeyCode::SPACE) && callbackData->jumpFuel > jumpFuelLoss)
                 {
                     for (int i = 0; i < space.shapes.size(); i++)
                     {
                         const Shape &shape = space.shapes[i];
                         if (shape.parentId != callbackData->carParentId)
                         {
                             continue;
                         }

                         callbackData->jumpFuel -= jumpFuelLoss;
                         ShapeIndexedRange range(shape);
                         for (int j = 0; j < range.size(); j++)
                         {
                             space.points.velocity[range[j]].y -= 0.015f;
                         }
                     }
                 }

                 float command = 0.0f;
                 if (game->keyIsPressed(GameKeyCode::D))
                 {
                     command = 1.0f;
                 }
                 else if (game->keyIsPressed(GameKeyCode::A))
                 {
                     command = -1.0f;
                 }

                 for (int i = 0; i < space.wheelMotors.size(); i++)
                 {
                     space.wheelMotors[i].enabled = command != 0.0f;
                     space.wheelMotors[i].command = command;
                 }
             },
             callbackData);

         GameRenderSettings renderSettings;
         //  renderSettings.renderPointIndices = true;
         renderSettings.renderShapeMatching = false;
         renderSettings.clearDebugGeometryWhenPaused = true;
         game->setRenderSettings(renderSettings);

         game->scale() = 1.00f;
         game->offset() = Vector2(270.00f, 47.00f);
     }},
    {"Tank", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "levels/car_scene.txt");
         Shapes::TankInstance tank = Shapes::createTank(space, 400.0f, 440.0f);

         TankRecoveryState *state = new TankRecoveryState();
         state->parentId = tank.parentId;
         state->hullShapeIndex = tank.hullShapeIndex;

         game->scheduleFrameCallback(
             [](Game *game, void *data)
             {
                 PhysicsSpace &space = game->physicsSpace();
                 TankRecoveryState *state = (TankRecoveryState *)data;
                 const float deltaMs = (float)game->lastElapsedTimeMilliseconds();

                 if (state->controlLockTimerMs > 0.0f)
                 {
                     state->controlLockTimerMs = max(0.0f, state->controlLockTimerMs - deltaMs);
                     setTankMotorCommand(space, state->parentId, 0.0f);
                     return;
                 }

                 if (state->hullShapeIndex < 0 || state->hullShapeIndex >= space.shapes.size())
                 {
                     setTankMotorCommand(space, state->parentId, 0.0f);
                     return;
                 }

                 const Shape &hull = space.shapes[state->hullShapeIndex];
                 ShapeProperties hullProperties = ShapeUtils::getShapeProperties(space.points.range(), hull);
                 const Vector2 worldUp = Vector2(0.0f, -1.0f).rotate(hullProperties.diffAngle);
                 const float uprightDot = worldUp.dot(Vector2(0.0f, -1.0f));
                 const bool inverted = uprightDot < -0.35f;

                 Console::logFrame(200.0f, 200.0f, inverted ? "Press SPACE to recover" : "Press SPACE to flip");
                 if (game->keyWasPressed(GameKeyCode::SPACE))
                 {
                     resetTankPose(space, *state, inverted ? 0.0f : 3.14159265f);
                     state->controlLockTimerMs = 500.0f;
                     setTankMotorCommand(space, state->parentId, 0.0f);
                     return;
                 }

                 float command = 0.0f;
                 if (game->keyIsPressed(GameKeyCode::D))
                 {
                     command = 1.0f;
                 }
                 else if (game->keyIsPressed(GameKeyCode::A))
                 {
                     command = -1.0f;
                 }

                 setTankMotorCommand(space, state->parentId, command);
             },
             state);

         GameRenderSettings renderSettings;
         renderSettings.renderShapeMatching = false;
         renderSettings.clearDebugGeometryWhenPaused = true;
         game->setRenderSettings(renderSettings);

         game->scale() = 1.00f;
         game->offset() = Vector2(270.00f, 47.00f);
     }},
    {"Tank accelerate", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         PhysicsSpaceStorage::loadFromFile(space, "levels/car_scene.txt");
         Shapes::createTank(space, 400.0f, 440.0f);

         for (int i = 0; i < space.wheelMotors.size(); i++)
         {
             space.wheelMotors[i].enabled = true;
             space.wheelMotors[i].command = 1.0f;
         }

         GameRenderSettings renderSettings;
         renderSettings.renderShapeMatching = false;
         game->setRenderSettings(renderSettings);

         game->scale() = 1.00f;
         game->offset() = Vector2(270.00f, 47.00f);
     }},
};

template <int N>
SceneDefinitionFolder makeFolder(const char *name, SceneDefinition (&arr)[N], bool hiddenInWebDemo = false)
{
    SceneDefinitionFolder folder;
    folder.name = name;
    folder.scenes = &arr[0]; // pointer to the first element
    folder.numScenes = N;
    folder.hiddenInWebDemo = hiddenInWebDemo;
    return folder;
}

// Folders hidden in the web demo are regression, debugging and work-in-progress scenes.
// They are left out of the scene list there but can still be loaded by name, e.g. by the editor tests.
SceneDefinitionFolder folders[] = {
    makeFolder("Shapes", shapeScenes),
    makeFolder("Collisions", collisionScenes, true),
    makeFolder("Collections", collectionScenes),
    makeFolder("Deformed shapes", deformedScenes, true),
    makeFolder("Game", gameScenes),
    makeFolder("Vehicles", vehicleScenes, true),
};

const char *SceneDefinition::defaultSceneName = "Bridge";

int SceneDefinitionFolder::numFolders = ARRAYSIZE(folders);
SceneDefinitionFolder *SceneDefinitionFolder::allFolders = folders;

SceneDefinition *SceneDefinition::getDefinitionFromName(const char *name)
{
    int numFolders = ARRAYSIZE(folders);

    for (int i = 0; i < numFolders; i++)
    {
        SceneDefinitionFolder &folder = folders[i];
        for (int j = 0; j < folder.numScenes; j++)
        {
            if (strcmp(folder.scenes[j].name, name) == 0)
            {
                return &folder.scenes[j];
            }
        }
    }

    return nullptr;
}
