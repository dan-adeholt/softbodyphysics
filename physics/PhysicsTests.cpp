#include "PhysicsTests.h"
#include "../game/Scenes.h"
#include "Physics.h"
#include "PhysicsSpace.h"
#include <cstring>
#include <cstdio>
#include "../game/Game.h"
#include "../game/GameKeyCode.h"
#include "../game/Shapes.h"
#include "../containers/Array.h"
#include "../utils/Console.h"
#include "ShapeUtils.h"
#include <cassert>
#include <cmath>

namespace
{
    static bool isWheelShape(const PhysicsSpace &space, int shapeIndex)
    {
        for (int i = 0; i < space.wheelMotors.size(); i++)
        {
            if (space.wheelMotors[i].shapeIndex == shapeIndex)
            {
                return true;
            }
        }

        return false;
    }

    static int findTankHullShapeIndex(const PhysicsSpace &space, int parentId)
    {
        for (int i = 0; i < space.shapes.size(); i++)
        {
            const Shape &shape = space.shapes[i];
            if (shape.parentId == parentId && !isWheelShape(space, shape.index))
            {
                return shape.index;
            }
        }

        return -1;
    }

}

PhysicsTestDefinition tests[] = {
    PhysicsTestDefinition(
        "Line spring rotation",
        350,
        [](Game *game)
        {
            Console::clear();
            Console::log("Initing test");
            SceneDefinition *def = SceneDefinition::getDefinitionFromName("Line spring");
            assert(def != nullptr);
            def->initFunc(game);
            PhysicsSpace &space = game->physicsSpace();
            space.collisionsEnabled = false;
            space.gravityEnabled = false;

            Shape &lineShape = space.shapes[0];
            int p1Index = lineShape.end - 1;
            PointMasses &points = space.points;
            Vector2 &p1 = points.pos[p1Index];
            p1.x += 100.0f;
            p1.y += 100.0f;
            Vector2 &v1 = points.velocity[p1Index];
            v1.x = 0.0f;
            v1.y = 0.0f;
        },
        [](Game *game, int time)
        {
            PhysicsSpace &space = game->physicsSpace();
            Shape &lineShape = space.shapes[0];
            PointMasses &points = space.points;
            int p0Index = lineShape.start;
            int p1Index = lineShape.end - 1;

            Vector2 p0 = points.pos[p0Index];
            Vector2 p1 = points.pos[p1Index];
            float angle = atan2(p1.y - p0.y, p1.x - p0.x);
            float validAngle = 0.89f;

            if (fabs(angle - validAngle) > 0.01f)
            {
                return "Spring damping causes rotation";
            }

            return (const char *)nullptr;
        },
        [](Game *game, int time) {

        }),
    PhysicsTestDefinition(
        "Static box",
        100,
        [](Game *game)
        {
            Console::clear();
            SceneDefinition *def = SceneDefinition::getDefinitionFromName("Box");
            assert(def != nullptr);
            def->initFunc(game);
            PhysicsSpace &space = game->physicsSpace();
            space.collisionsEnabled = false;
            space.gravityEnabled = false;
        },
        [](Game *game, int time)
        {
            return (const char *)nullptr;
        },
        [](Game *game, int time) {

        })
    ,
    PhysicsTestDefinition(
        "Tank manual flip",
        60,
        [](Game *game)
        {
            Console::clear();
            SceneDefinition *def = SceneDefinition::getDefinitionFromName("Tank");
            assert(def != nullptr);
            def->initFunc(game);

            PhysicsSpace &space = game->physicsSpace();
            assert(space.wheelMotors.size() > 0);

            const int parentId = space.wheelMotors[0].parentId;
            const int hullShapeIndex = findTankHullShapeIndex(space, parentId);
            assert(hullShapeIndex != -1);
        },
        [](Game *game, int time)
        {
            if (time < 20)
            {
                return (const char *)nullptr;
            }

            PhysicsSpace &space = game->physicsSpace();
            if (space.wheelMotors.size() == 0)
            {
                return "Tank wheel motors missing";
            }

            const int parentId = space.wheelMotors[0].parentId;
            const int hullShapeIndex = findTankHullShapeIndex(space, parentId);
            if (hullShapeIndex == -1)
            {
                return "Tank hull shape missing";
            }

            ShapeProperties hullProperties = ShapeUtils::getShapeProperties(space.points.range(), space.shapes[hullShapeIndex]);
            Vector2 worldUp = Vector2(0.0f, -1.0f).rotate(hullProperties.diffAngle);
            if (worldUp.dot(Vector2(0.0f, -1.0f)) > -0.75f)
            {
                return "Tank did not flip upside down";
            }

            return (const char *)nullptr;
        },
        [](Game *game, int time)
        {
            ConsoleProfileInfo profileInfo = {};
            if (time == 5)
            {
                game->keyDown(GameKeyCode::SPACE, 0, profileInfo);
            }
            else if (time == 6)
            {
                game->keyUp(GameKeyCode::SPACE, 0, profileInfo);
            }
        })

};

int PhysicsTestDefinition::numTests = sizeof(tests) / sizeof(tests[0]);
PhysicsTestDefinition *PhysicsTestDefinition::allTests = tests;

PhysicsTestDefinition::PhysicsTestDefinition(const char *name,
                                             int duration,
                                             PhysicsTestFunction initFunc,
                                             PhysicsInvariantFunction invariantFunction,
                                             PhysicsTimeFunction actionFunction) : name(name), invariantResult(nullptr), duration(duration), initFunc(initFunc), invariantFunction(invariantFunction), actionFunction(actionFunction), time(0)
{
}

void PhysicsTestDefinition::start(Game *game)
{
    game->clear();
    initFunc(game);
    time = 0;
}

void PhysicsTestDefinition::step(Game *game, double elapsedMilliseconds, ConsoleProfileInfo &profileInfo)
{
    if (actionFunction != nullptr)
    {
        actionFunction(game, time);
    }

    invariantResult = invariantFunction(game, time);

    game->update(elapsedMilliseconds, false, profileInfo);
    this->time++;
}

void PhysicsTestDefinition::end(Game *game)
{
}
