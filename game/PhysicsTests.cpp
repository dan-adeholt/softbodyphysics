#include "PhysicsTests.h"
#include "Scenes.h"
#include "Physics.h"
#include <cstring>
#include <cstdio>
#include "Game.h"
#include "../containers/Array.h"
#include "../utils/Console.h"
#include <cassert>
#include <cmath>

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
            PhysicsSpace *space = game->physicsSpace();
            space->collisionsEnabled = false;
            space->gravityEnabled = false;

            Shape &lineShape = space->shapes[0];
            int p1Index = lineShape.end - 1;
            PointMasses &points = space->points;
            Vector2 &p1 = points.pos[p1Index];
            p1.x += 100.0f;
            p1.y += 100.0f;
            Vector2 &v1 = points.velocity[p1Index];
            v1.x = 0.0f;
            v1.y = 0.0f;
        },
        [](Game *game, int time)
        {
            PhysicsSpace *space = game->physicsSpace();
            Shape &lineShape = space->shapes[0];
            PointMasses &points = space->points;
            int p0Index = lineShape.start;
            int p1Index = lineShape.end - 1;

            Vector2 p0 = points.pos[p0Index];
            Vector2 p1 = points.pos[p1Index];
            float angle = atan2(p1.y - p0.y, p1.x - p0.x);
            float validAngle = 0.89;

            if (fabs(angle - validAngle) > 0.01)
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
            PhysicsSpace *space = game->physicsSpace();
            space->collisionsEnabled = false;
            space->gravityEnabled = false;
        },
        [](Game *game, int time)
        {
            return (const char *)nullptr;
        },
        [](Game *game, int time) {

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

    game->update(elapsedMilliseconds, profileInfo);
    this->time++;
}

void PhysicsTestDefinition::end(Game *game)
{
}
