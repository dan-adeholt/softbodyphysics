#include "ShapeMatching.h"

void ShapeMatching::shapeMatchAlignInit(PointMassesRange points, Shape &shape, Range<ShapeQuad> partialShapes)
{
    if (shape.subShapeSpan.isValid())
    {
        Range<ShapeQuad> subShapes = partialShapes.slice(shape.subShapeSpan);
        for (int i = 0; i < subShapes.size; i++)
        {
            ShapeQuad &subShape = subShapes[i];
            Vector2 center;

            for (int i = 0; i < subShape.size; i++)
            {
                center += subShape.originalPos[i];
            }

            center /= subShape.size;

            for (int i = 0; i < subShape.size; i++)
            {
                subShape.originalPos[i] -= center;
                subShape.shapePos[i] -= center;
            }
        }
    }
    else
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
}

void ShapeMatching::shapeMatchAlign(PointMassesRange points, Array<Shape> &shapes, Range<ShapeQuad> partialShapes, int draggingShapeIndex)
{
    for (int i = 0; i < shapes.size(); i++)
    {
        Shape &shape = shapes[i];
        if (i == draggingShapeIndex)
        {
            continue;
        }

        if (shape.subShapeSpan.isValid())
        {
            Range<ShapeQuad> subShapes = partialShapes.slice(shape.subShapeSpan);
            for (int j = 0; j < subShapes.size; j++)
            {
                ShapeQuad &subShape = subShapes[j];
                Vector2 center;

                for (int k = 0; k < subShape.size; k++)
                {
                    center += points.pos[subShape.indices[k]];
                }

                center /= subShape.size;
                float avgDiffAngle = 0.0f;

                for (int k = 0; k < subShape.size; k++)
                {
                    Vector2 translatedPos = points.pos[subShape.indices[k]] - center;
                    avgDiffAngle += subShape.originalPos[k].angle(translatedPos);
                }

                avgDiffAngle /= subShape.size;

                for (int k = 0; k < subShape.size; k++)
                {
                    subShape.shapePos[k] = subShape.originalPos[k].rotate(avgDiffAngle) + center;
                }
            }
        }
        else
        {
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
}
