#include "./SDLGameRenderer.h"
#include <SDL.h>
#include "containers/Range.h"
#include "game/Physics.h"
#include "game/Game.h"

float MIN_LINE_POS = -100000;
float MAX_LINE_POS = 1000000;

SDLGameRenderer::SDLGameRenderer() {
    
}

SDLGameRenderer::~SDLGameRenderer() {
    
}

void renderSprings(SDL_Renderer* renderer, Range<Spring> springs, const Range<PointMass>& points) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    for (int i = 0; i < springs.size; i++) {
        const Spring& spring = springs.data[i];
        const PointMass& pointA = points[spring.pointA];
        const PointMass& pointB = points[spring.pointB];
        float currentLength = (pointA.pos - pointB.pos).length();
        float tension = fabs(1.0f - (spring.length / currentLength));

        SDL_SetRenderDrawColor(renderer, 255, (int)(255 - tension * 10), (int)(255 - tension * 10), 255);

        // To prevent SDL taking extremely long to render degenerate lines
        if (pointA.pos.x > MIN_LINE_POS && pointA.pos.x < MAX_LINE_POS &&
            pointA.pos.y > MIN_LINE_POS && pointA.pos.y < MAX_LINE_POS &&
            pointB.pos.x > MIN_LINE_POS && pointB.pos.x < MAX_LINE_POS &&
            pointB.pos.y > MIN_LINE_POS && pointB.pos.y < MAX_LINE_POS) 
        {
            Vector2 direction = (pointB.pos - pointA.pos);
            Vector2 ortho = Vector2(-direction.y, direction.x).normalized();

            Vector2 prevPoint = pointA.pos;
            for (int i = 0; i <= 10; i++) {
                float relPos = i / 10.0f;
                float springSize = 10.0f;
                Vector2 pos = pointA.pos + (direction * relPos);
                float sign = (i % 2 == 0) ? 1.0f : -1.0f;
                Vector2 springPos = pos + ortho * springSize * sign;

                SDL_RenderDrawLine(renderer, prevPoint.x, prevPoint.y, springPos.x, springPos.y);
                prevPoint = springPos;
            }
            // SDL_RenderDrawLine(renderer, pointA.pos.x, pointA.pos.y, pointB.pos.x, pointB.pos.y);
        }
    }
}

void renderPointMasses(SDL_Renderer *renderer, Range<PointMass> pointMasses)
{
    SDL_Rect rectangle;
    rectangle.h = 4;
    rectangle.w = 4;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    for (int i = 0; i < pointMasses.size; i++)
    {
        rectangle.x = pointMasses.data[i].pos.x - 2;
        rectangle.y = pointMasses.data[i].pos.y - 2;
        SDL_RenderFillRect(renderer, &rectangle);
    }
}

void renderShapes(SDL_Renderer *renderer, Range<Shape> shapes, Range<PointMass> pointMasses)
{
    SDL_Rect rectangle;
    rectangle.h = 4;
    rectangle.w = 4;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i < shapes.size; i++) {
        const Shape &shape = shapes.data[i];
        PointMass firstPoint = pointMasses[shape.start];
        PointMass p0 = firstPoint;

        for (int pointIndex = shape.start + 1; pointIndex < shape.end; pointIndex++) {
            const PointMass &p1 = pointMasses[pointIndex];
            SDL_RenderDrawLine(renderer, p0.pos.x, p0.pos.y, p1.pos.x, p1.pos.y);
            p0 = p1;
        }

        SDL_RenderDrawLine(renderer, p0.pos.x, p0.pos.y, firstPoint.pos.x, firstPoint.pos.y);
    }
}


void SDLGameRenderer::render(SDL_Renderer *renderer, Game* game) {
    Range<Shape> shapes;
    Range<PointMass> pointMasses;
    game->getDynamicShapes(shapes);
    game->getDynamicPoints(pointMasses);
    renderShapes(renderer, shapes, pointMasses);

    Range<Spring> springs;
    game->getSprings(springs);
    renderSprings(renderer, springs, pointMasses);
    game->getStaticShapes(shapes);
    game->getStaticPoints(pointMasses);
    renderShapes(renderer, shapes, pointMasses);

}