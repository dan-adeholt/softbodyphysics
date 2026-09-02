#ifndef __PHYSICS__COLLISION_SOLVER_INTERNAL_H
#define __PHYSICS__COLLISION_SOLVER_INTERNAL_H

// Implementation detail shared between CollisionSolver.cpp and the benchmarks
// in CollisionSolverPerf.cpp. Not part of the public CollisionSolver interface.

#include "./CollisionSolver.h"
#include "./CollisionMap.h"
#include "./Physics.h"

struct CollisionSolver::Impl
{
    Impl() : collisionMap(0),
             gridCollisionMap(0),
             boundingBoxes(0),
             sortedBoundingBoxes(0),
             sortedBoundingBoxShapeIndices(0)
    {
    }

    CollisionMap collisionMap;
    CollisionMap gridCollisionMap;
    Array<ShapeBoundingBox> boundingBoxes;
    Array<OrientedBoundingBox> orientedBoundingBoxes;
    Array<KDOPProjection> kdopProjections;
    Array<BoundingBox> sortedBoundingBoxes;
    Array<int> sortedBoundingBoxShapeIndices;
    CollisionGridSimple collisionGridSimple;
    bool useCollisionGrid = false;
};

// Direction a penetrating point is nudged after being snapped onto the entry
// edge. Exposed for tests; see the notes in CollisionSolver.cpp.
Vector2 depenetrationDirection(const Vector2 &pointVelocity, const Vector2 &segmentNormal, float windingSign);

bool kdopOverlap(const KDOPProjection &a, const KDOPProjection &b);
bool shapesOverlap(const PointMassesRange &points, const Shape &shape1, const Shape &shape2);
bool shapesOverlapSimple(const PointMassesRange &points, const Shape &shape1, const Shape &shape2);

#endif
