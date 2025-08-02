#ifndef __GAME_Integrator_H__
#define __GAME_Integrator_H__

#include "Physics.h"
#include "Springs.h"

struct ConsoleProfileInfo;
struct PhysicsSpace;

struct RK4Integrator
{
    struct Impl;
    Impl *m;

    RK4Integrator();
    ~RK4Integrator();
    RK4Integrator(const RK4Integrator &) = delete;

    void clear();
    void prepareRK4Step(PhysicsSpace &PhysicsSpace, float dt, Array<PointDerivative> &derivatives, Array<PointDerivative> &outDerivatives, const ShapeMatchDragData &dragData, ConsoleProfileInfo &profileInfo);
    void updateRK4Springs(PhysicsSpace &spaces, Array<PointDerivative> &outDerivatives, const ShapeMatchDragData &dragData, ConsoleProfileInfo &profileInfo);
    void performRK4Integration(PhysicsSpace &space, const ShapeMatchDragData &dragData, ConsoleProfileInfo &profileInfo);

    void testRK4Performance(int iterations, PhysicsSpace &space);
    void testRK4PreparePerformance(int iterations, PhysicsSpace &space);
    void testSpringPerformance(int iterations, PhysicsSpace &space);
};

struct Integrator
{
    RK4Integrator rk4Integrator;
    void clear();
    void performIntegration(PhysicsSpace &space, const ShapeMatchDragData &dragData, ConsoleProfileInfo &profileInfo);
};

#endif // __GAME_Integrator_H__