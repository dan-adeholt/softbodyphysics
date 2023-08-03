
#include "./Game.h"
#include "../utils/MinMax.h"
#include "../containers/Range.h"
#include "../containers/Array.h"
#include "./Physics.h"

#define NUM_SHAPES 100
#define NUM_POINTS 1024

struct Game::Impl
{
    int count;
    Array<Shape> shapes;
    Array<PointMass> points;
};

Game::Game() : m_impl(new Game::Impl)
{
    Array<Shape> &shapes = m_impl->shapes;
    Array<PointMass> &points = m_impl->points;

    shapes.reserve(NUM_SHAPES);
    points.reserve(NUM_POINTS);

    for (int i = 0; i < NUM_SHAPES; i++)
    {
        PointMass point = {
            .x = (float)i * 16.0f,
            .y = 100,
            .velocity = 0.0f,
            .mass = 1.0f};

        points.push(point);

        Shape shape = {
            .points = points.range(points.size() - 1, points.size())};

        shapes.push(shape);
    }
}

Game::~Game()
{
    delete m_impl;
}

float terminalVelocity = 1000.0f;

void Game::getPointMasses(Range<PointMass> &pointMasses) const
{
    pointMasses = m_impl->points.range(0, m_impl->points.size());
}

void Game::handleGravity(Range<PointMass> &points, double elapsedTimeMilliseconds)
{
    // Apply gravity
    for (int i = 0; i < points.size; i++)
    {
        points[i].velocity = min(terminalVelocity, (float)(points[i].velocity + elapsedTimeMilliseconds * 0.0005));
        points[i].y += points[i].velocity * elapsedTimeMilliseconds;
    }
}

void Game::handleCollisions(Range<PointMass> &points)
{
    for (int i = 0; i < points.size; i++)
    {
        if (points[i].y > 700)
        {
            points[i].y = 700.0f;
            points[i].velocity = -points[i].velocity * 0.4;
        }
    }
}

void Game::update(double elapsedTimeMilliseconds)
{
    Range<PointMass> points = m_impl->points.range();
    handleGravity(points, elapsedTimeMilliseconds);
    handleCollisions(points);
}
