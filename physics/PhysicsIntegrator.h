#ifndef __GAME_PHYSICSINTEGRATOR_H__
#define __GAME_PHYSICSINTEGRATOR_H__

#include "Physics.h"

struct ConsoleProfileInfo;
struct PhysicsSpace;

struct RK4Integrator
{
    Array<PointDerivative> rk1;
    Array<PointDerivative> rk2;
    Array<PointDerivative> rk3;
    Array<PointDerivative> rk4;
    PointMasses rkTemp;
    Array<PointDerivative> rkEmptyDerivatives;

    void prepareRK4Step(PhysicsSpace &PhysicsSpace, float dt, Array<PointDerivative> &derivatives, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo);
    void updateRK4Springs(PhysicsSpace &spaces, Array<PointDerivative> &outDerivatives, ConsoleProfileInfo &profileInfo);    
    void performRK4Integration(PhysicsSpace &space, ConsoleProfileInfo &profileInfo);

    void testRK4Performance(int iterations, PhysicsSpace &space);
    void testRK4PreparePerformance(int iterations, PhysicsSpace &space);
    void testSpringPerformance(int iterations, PhysicsSpace &space);
};

struct PhysicsIntegrator
{
    RK4Integrator rk4Integrator;
    void performIntegration(PhysicsSpace &space, ConsoleProfileInfo &profileInfo);
};

#endif // __GAME_PHYSICSINTEGRATOR_H__