// Benchmarks for the broadphase and narrowphase strategies. These are driven
// from Game::testCollisionPerformance (the Profiler tab and perftest.cpp) and
// are kept out of CollisionSolver.cpp so that file only holds the simulation.

#include "./CollisionSolverInternal.h"
#include "./PhysicsSpace.h"
#include "../utils/Console.h"

struct BroadphasePair
{
    int shapeIndex1;
    int shapeIndex2;
};

// Sweep the x-sorted bounding boxes and collect the pairs that survive.
// Because the boxes are sorted we already know otherBox.x1 >= box.x1, so only
// the other axis needs checking.
static void collectBroadphasePairs(const Array<BoundingBox> &sortedBoundingBoxes,
                                   const Array<int> &sortedBoundingBoxShapeIndices,
                                   ConsoleProfileInfo &profileInfo,
                                   Array<BroadphasePair> &pairs)
{
    for (int i = 0; i < sortedBoundingBoxes.size(); i++)
    {
        const BoundingBox &box = sortedBoundingBoxes[i];

        for (int j = i + 1; j < sortedBoundingBoxes.size(); j++)
        {
            const BoundingBox &otherBox = sortedBoundingBoxes[j];
            profileInfo.numBbboxChecks++;

            if (otherBox.x1 >= box.x2)
            {
                break;
            }
            else if (otherBox.y1 >= box.y2 || otherBox.y2 <= box.y1)
            {
                continue;
            }

            BroadphasePair pair = {sortedBoundingBoxShapeIndices[i], sortedBoundingBoxShapeIndices[j]};
            pairs.push(pair);
        }
    }
}

int CollisionSolver::testBoundingBoxesPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations)
{
    updateBoundingBoxes(space, profileInfo);

    // This one measures the sweep itself, so it repeats the whole sweep rather
    // than collecting pairs up front.
    int numRejects = 0;
    for (int iteration = 0; iteration < numIterations; iteration++)
    {
        for (int i = 0; i < m->sortedBoundingBoxes.size(); i++)
        {
            const BoundingBox &box = m->sortedBoundingBoxes[i];

            for (int j = i + 1; j < m->sortedBoundingBoxes.size(); j++)
            {
                const BoundingBox &otherBox = m->sortedBoundingBoxes[j];
                profileInfo.numBbboxChecks++;

                if (otherBox.x1 >= box.x2)
                {
                    numRejects++;
                    break;
                }
                else if (otherBox.y1 >= box.y2 || otherBox.y2 <= box.y1)
                {
                    numRejects++;
                    continue;
                }
            }
        }
    }

    return numRejects / numIterations;
}

int CollisionSolver::testAlignedBoundingBoxesPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations)
{
    struct OrientedBoundingBoxPair
    {
        OrientedBoundingBox box1;
        OrientedBoundingBox box2;
    };

    updateBoundingBoxes(space, profileInfo);

    Array<BroadphasePair> broadphasePairs;
    collectBroadphasePairs(m->sortedBoundingBoxes, m->sortedBoundingBoxShapeIndices, profileInfo, broadphasePairs);

    // Pack the boxes up front so the timed loop measures the overlap test
    // rather than the gather.
    Array<OrientedBoundingBoxPair> pairs;
    for (int i = 0; i < broadphasePairs.size(); i++)
    {
        OrientedBoundingBoxPair pair = {m->orientedBoundingBoxes[broadphasePairs[i].shapeIndex1],
                                        m->orientedBoundingBoxes[broadphasePairs[i].shapeIndex2]};
        pairs.push(pair);
    }

    int numRejects = 0;
    for (int iteration = 0; iteration < numIterations; iteration++)
    {
        for (int i = 0; i < pairs.size(); i++)
        {
            if (!pairs[i].box1.overlaps(pairs[i].box2))
            {
                numRejects++;
            }
        }
    }

    return numRejects / numIterations;
}

int CollisionSolver::testKdopPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations)
{
    struct KdopProjectionPair
    {
        KDOPProjection box1;
        KDOPProjection box2;
    };

    updateBoundingBoxes(space, profileInfo);

    Array<BroadphasePair> broadphasePairs;
    collectBroadphasePairs(m->sortedBoundingBoxes, m->sortedBoundingBoxShapeIndices, profileInfo, broadphasePairs);

    Array<KdopProjectionPair> pairs;
    for (int i = 0; i < broadphasePairs.size(); i++)
    {
        KdopProjectionPair pair = {m->kdopProjections[broadphasePairs[i].shapeIndex1],
                                   m->kdopProjections[broadphasePairs[i].shapeIndex2]};
        pairs.push(pair);
    }

    int numRejects = 0;
    for (int iteration = 0; iteration < numIterations; iteration++)
    {
        for (int i = 0; i < pairs.size(); i++)
        {
            if (!kdopOverlap(pairs[i].box1, pairs[i].box2))
            {
                numRejects++;
            }
        }
    }

    return numRejects / numIterations;
}

int CollisionSolver::testSimpleOverlapPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations)
{
    struct OverlapPair
    {
        Shape shape1;
        Shape shape2;
    };

    updateBoundingBoxes(space, profileInfo);

    Array<BroadphasePair> broadphasePairs;
    collectBroadphasePairs(m->sortedBoundingBoxes, m->sortedBoundingBoxShapeIndices, profileInfo, broadphasePairs);

    Array<OverlapPair> pairs;
    for (int i = 0; i < broadphasePairs.size(); i++)
    {
        OverlapPair pair = {space.shapes[broadphasePairs[i].shapeIndex1],
                            space.shapes[broadphasePairs[i].shapeIndex2]};
        pairs.push(pair);
    }

    int numRejects = 0;
    for (int iteration = 0; iteration < numIterations; iteration++)
    {
        for (int i = 0; i < pairs.size(); i++)
        {
            if (!shapesOverlapSimple(space.points.range(), pairs[i].shape1, pairs[i].shape2))
            {
                numRejects++;
            }
        }
    }

    return numRejects / numIterations;
}

int CollisionSolver::testOverlapPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations)
{
    struct OverlapPair
    {
        Shape shape1;
        Shape shape2;
    };

    updateBoundingBoxes(space, profileInfo);

    Array<BroadphasePair> broadphasePairs;
    collectBroadphasePairs(m->sortedBoundingBoxes, m->sortedBoundingBoxShapeIndices, profileInfo, broadphasePairs);

    Array<OverlapPair> pairs;
    for (int i = 0; i < broadphasePairs.size(); i++)
    {
        OverlapPair pair = {space.shapes[broadphasePairs[i].shapeIndex1],
                            space.shapes[broadphasePairs[i].shapeIndex2]};
        pairs.push(pair);
    }

    int numRejects = 0;
    for (int iteration = 0; iteration < numIterations; iteration++)
    {
        for (int i = 0; i < pairs.size(); i++)
        {
            if (!shapesOverlap(space.points.range(), pairs[i].shape1, pairs[i].shape2))
            {
                numRejects++;
            }
        }
    }

    return numRejects / numIterations;
}

int CollisionSolver::testCollisionPerformance(PhysicsSpace &space, ConsoleProfileInfo &profileInfo, int numIterations, bool testMidPoint)
{
    struct OverlapPair
    {
        Shape shape1;
        Shape shape2;
    };

    updateBoundingBoxes(space, profileInfo);

    Array<BroadphasePair> broadphasePairs;
    collectBroadphasePairs(m->sortedBoundingBoxes, m->sortedBoundingBoxShapeIndices, profileInfo, broadphasePairs);

    // Unlike the other benchmarks this one also applies the OBB filter, so it
    // measures the narrowphase on the pairs the solver would actually reach.
    Array<OverlapPair> pairs;
    for (int i = 0; i < broadphasePairs.size(); i++)
    {
        int shapeIndex1 = broadphasePairs[i].shapeIndex1;
        int shapeIndex2 = broadphasePairs[i].shapeIndex2;

        if (!m->orientedBoundingBoxes[shapeIndex1].overlaps(m->orientedBoundingBoxes[shapeIndex2]))
        {
            continue;
        }

        OverlapPair pair = {space.shapes[shapeIndex1], space.shapes[shapeIndex2]};
        pairs.push(pair);
    }

    PhysicsSpace copy;
    for (int iteration = 0; iteration < numIterations; iteration++)
    {
        copy.assign(space);

        for (int i = 0; i < pairs.size(); i++)
        {
            const Shape &shape1 = pairs[i].shape1;
            const Shape &shape2 = pairs[i].shape2;
            const BoundingBox &box1 = m->boundingBoxes[shape1.index];
            const BoundingBox &box2 = m->boundingBoxes[shape2.index];

            if (testMidPoint)
            {
                calculateCollisionsMidPoint(copy.points.range(), shape1, shape2, box1, box2);
                calculateCollisionsMidPoint(copy.points.range(), shape2, shape1, box2, box1);
            }
            else
            {
                calculateCollisions(copy.points.range(), shape1, shape2, box1, box2);
                calculateCollisions(copy.points.range(), shape2, shape1, box2, box1);
            }
        }
    }

    return 0;
}
