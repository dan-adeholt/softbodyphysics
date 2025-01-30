#include "ShapeMatching.h"
#include "../utils/Console.h"

void ShapeMatching::shapeMatchAlignInit(PointMassesRange points, Shape &shape)
{

    Vector2 center;
    for (int j = shape.start; j < shape.end; j++)
    {
        center += points.shapeOriginalPos[j];
    }

    int numPoints = shape.end - shape.start;
    center /= numPoints;

    for (int j = shape.start; j < shape.end; j++)
    {
        points.shapeOriginalPos[j] -= center;
    }
}

void ShapeMatching::shapeMatchAlign(PointMassesRange points, Array<Shape> &shapes, int draggingShapeIndex)
{
    for (int i = 0; i < shapes.size(); i++)
    {
        Shape &shape = shapes[i];

        if (i == draggingShapeIndex || shape.disableShapeMatching || shape.isStatic)
        {
            continue;
        }

        Vector2 center;

        for (int j = shape.start; j < shape.end; j++)
        {
            center += points.pos[j];
        }

        int numPoints = shape.end - shape.start;

        center /= numPoints;

        float avgDiffAngle = 0.0f;

        for (int j = shape.start; j < shape.end; j++)
        {
            Vector2 translatedPos = points.pos[j] - center;
            float angleDiff = points.shapeOriginalPos[j].angle(translatedPos);
            avgDiffAngle += angleDiff;
        }

        avgDiffAngle /= numPoints;

        for (int j = shape.start; j < shape.end; j++)
        {
            points.shapePos[j] = points.shapeOriginalPos[j].rotate(avgDiffAngle) + center;
        }
    }
}
