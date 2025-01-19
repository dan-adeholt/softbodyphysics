#include "Scenes.h"
#include <cstdio>
#include "Shapes.h"
#include "Game.h"
#include "GameKeyCode.h"
#include "../physics/PhysicsSpace.h"
#include "../physics/PhysicsSpaceStorage.h"
#include "../physics/CollisionSolver.h"
#include "../containers/Array.h"
#include "../utils/Console.h"
#include "../utils/MinMax.h"
#include <cstring>
#include <math.h>
#include "stdint.h"

float computeSignedArea(Vector2 A, Vector2 B, Vector2 C)
{
    return 0.5f * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
}

float computeWindingOrder(const PointMassesRange &points)
{
    float sum = 0.0f;
    int n = points.size();
    for (int i = 0; i < n; i++)
    {
        int next = (i + 1) % n; // Wrap around to the first point
        sum += points.pos[i].x * points.pos[next].y - points.pos[i].y * points.pos[next].x;
    }

    return sum; // Positive for counterclockwise, negative for clockwise
}

void findEntryEdgeClosestSegmentNew(PointMassesRange collisionShape,
                                    const Vector2 &currentPoint,
                                    const Vector2 &prevPoint,
                                    const Vector2 &nextPoint,
                                    int &entryEdgeIndex,
                                    Vector2 &entryPoint,
                                    float &entryTime)
{
    float minDistanceSquared = __FLT_MAX__;
    int collisionShapeSize = collisionShape.pos.size;
    for (int i = 0; i < collisionShapeSize; i++)
    {
        Vector2 segment0 = collisionShape.pos[i];
        Vector2 segment1 = collisionShape.pos[(i + 1) % collisionShapeSize];

        float distanceToVertex = (segment0 - currentPoint).length();

        // Vector from A to B
        Vector2 segment = segment1 - segment0;
        // Vector from A to P
        Vector2 segmentToPoint = currentPoint - segment0;
        Vector2 segmentNormal = segment.normalVector().normalized();

        Vector2 pointOutside = (segment0 + segment * 0.5f) - segmentNormal * 2.0f;

        float t_point, t_edge;

        if (lineSegmentIntersection(segment0, segment1, prevPoint, pointOutside, t_point, t_edge) || lineSegmentIntersection(segment0, segment1, pointOutside, nextPoint, t_point, t_edge))
        {
            continue;
        }

        Console::drawPoint(pointOutside, 0x00FF00);

        // The projection of point P onto the line defined by segment AB is given by:
        // v dot w / v dot v
        // Compute projection t
        float t = segmentToPoint.dot(segment) / segment.dot();
        t = clamp(t, 0.0f, 1.0f);

        Vector2 closestPoint = segment0 + segment * t;
        Vector2 pointToClosestPoint = closestPoint - currentPoint;

        float distanceToClosestPointSquared = (pointToClosestPoint).lengthSquared();

        if (distanceToClosestPointSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceToClosestPointSquared;
            entryEdgeIndex = i;
            entryPoint = closestPoint;
            entryTime = t;
        }
    }
}

#define ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR)))) // Size of a static C-style array. Don't use on pointers!

PointMassesRange getPointMasses(Range<Vector2> pos, Range<float> mass, Range<Vector2> velocity)
{
    // Just assign pos to shapeoriginalpos/shapepos for now, not used in collision stuff
    return {.pos = pos, .shapeOriginalPos = pos, .shapePos = pos, mass, velocity};
}

SceneDefinition deformedScenes[] = {
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
         ShapeEntry shape0 = {
             .isStatic = false,
             .points = PointMasses()
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
                           })
                           .withShapePos({
                               {150.000000f, 667.980347f},
                               {180.000000f, 667.980347f},
                               {180.000000f, 697.980347f},
                               {150.000000f, 697.980347f},
                           }),
             .springs = Array<Spring>({
                 Spring(0, 1, 30.000000f, 5.000000f, 7.225000f, 0),
                 Spring(1, 2, 30.000000f, 5.000000f, 7.225000f, 0),
                 Spring(2, 3, 30.000000f, 5.000000f, 7.225000f, 0),
                 Spring(3, 0, 30.000000f, 5.000000f, 7.225000f, 0),
                 Spring(0, 2, 42.426407f, 5.000000f, 7.225000f, 0),
                 Spring(1, 3, 42.426407f, 5.000000f, 7.225000f, 0),
             })};
         ShapeEntry shape1 = {
             .isStatic = false,
             .points = PointMasses()
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
                           })
                           .withShapePos({
                               {148.868561f, 700.255920f},
                               {178.851761f, 701.259766f},
                               {177.847870f, 731.242981f},
                               {147.864670f, 730.239136f},
                           }),
             .springs = Array<Spring>({
                 Spring(4, 5, 30.000000f, 5.000000f, 7.225000f, 1),
                 Spring(5, 6, 30.000000f, 5.000000f, 7.225000f, 1),
                 Spring(6, 7, 30.000000f, 5.000000f, 7.225000f, 1),
                 Spring(7, 4, 30.000000f, 5.000000f, 7.225000f, 1),
                 Spring(4, 6, 42.426407f, 5.000000f, 7.225000f, 1),
                 Spring(5, 7, 42.426407f, 5.000000f, 7.225000f, 1),
             })};
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

         PointMassesRange firstBoxPoints = space.points.range(firstBox);
         for (Vector2 &velocity : firstBoxPoints.velocity)
         {
             velocity.x = 0.3f;
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

         PointMassesRange firstBoxPoints = space.points.range(firstBox);
         for (Vector2 &velocity : firstBoxPoints.velocity)
         {
             velocity.x = 3.5f;
             //  velocity.y = 0.01f;
         }

         game->physicsSpace().gravityEnabled = false;
     }}};

SceneDefinition shapeScenes[] = {
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
    {"Loose circle", [](Game *game)
     {
         PhysicsSpace &space = game->physicsSpace();
         Shapes::createLooseCircle(space, 600.0f, 350.0f, 110.0f, 0.2f);

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
     }}};

SceneDefinition collectionScenes[] = {
    {"Stacked boxes", [](Game *game)
     {
         float size = 30.0f;
         float spacing = (size + 1.0f) * 3.0f;
         PhysicsSpace &space = game->physicsSpace();
         for (int y = 0; y < 20; y++)
         {
             float x = 15;

             Shapes::createQuad(space, 150.0f + y * 2.0f, -1900.0f + y * spacing, size, size, 0.25f);

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
                 Shapes::createRoundedQuad(space, spacing + x * spacing + y * 2.0f, spacing + y * spacing + x * 0.01f - 900.0f, size, size, 5.5f, 30.0f);
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
                 Shapes::createQuad(space, spacing + x * spacing + y * 2.0f, spacing + y * spacing + x * 0.01f - 900.0f, size, size, 5.5f, 30.0f);
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
                 Shapes::createCircle(space, spacing + x * spacing + y * 2.0f, spacing + y * spacing + x * 0.01f - 500.0f, size, 30.0f);
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
         for (int y = 0; y < 35; y++)
         {
             for (int x = 0; x < 35; x++)
             {
                 Shapes::createCircle(space, spacing + x * spacing, spacing * 2 + y * spacing, size, 0.25f);
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
         Shapes::createBridge(space, x, y, 1.0f, numSegments);
         space.staticJoints.push({0, Vector2(x, y)});
         space.staticJoints.push({numSegments - 1, Vector2(x + 70.0f * numSegments, y)});
         space.staticJoints.push({numSegments, Vector2(x + 70.0f * numSegments, y + 70.0f)});
         space.staticJoints.push({numSegments * 2 - 1, Vector2(x, y + 70.0f)});

         Shapes::createStaticQuad(space, 149.0f, 90.0f, 50.0f, 600.0f, 1.0f);
         Shapes::createStaticQuad(space, 1110.0f, 90.0f, 50.0f, 600.0f, 1.0f);

         for (int i = 0; i < 10; i++)
         {
             Shape s = Shapes::createCircle(space, 440.0f + i * 55.0f, 50.0f, 40.0f, 0.25f, 4.0f);
         }

         //  game->scale() = 4.5f;
         //  game->offset() = Vector2(-2150.0f, -1200.0f);
         //  game->setPaused();
         //  Shapes::createQuad(space.points, space.springs, 600.0f, 190.0f, 100.0f, 100.0f, 1.0f);
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

template <int N>
SceneDefinitionFolder makeFolder(const char *name, SceneDefinition (&arr)[N])
{
    SceneDefinitionFolder folder;
    folder.name = name;
    folder.scenes = &arr[0]; // pointer to the first element
    folder.numScenes = N;
    return folder;
}

SceneDefinitionFolder folders[] = {
    makeFolder("Shapes", shapeScenes),
    makeFolder("Collisions", collisionScenes),
    makeFolder("Collections", collectionScenes),
    makeFolder("Deformed shapes", deformedScenes),
    makeFolder("Game", gameScenes),
};

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
