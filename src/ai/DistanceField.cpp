#include "ai/DistanceField.hpp"

#include <cstddef>
#include <queue>

namespace
{
    constexpr int kDx[4] = {1, -1, 0, 0};
    constexpr int kDy[4] = {0, 0, 1, -1};
}

void DistanceField::rebuild(const TileMap& tiles, sf::Vector2i target)
{
    m_width = tiles.width();
    m_height = tiles.height();
    m_distance.assign(static_cast<std::size_t>(m_width) * static_cast<std::size_t>(m_height), -1);

    if (!tiles.isWalkable(target.x, target.y))
    {
        return;
    }

    std::queue<sf::Vector2i> frontier;
    m_distance[static_cast<std::size_t>(target.y * m_width + target.x)] = 0;
    frontier.push(target);

    while (!frontier.empty())
    {
        const sf::Vector2i current = frontier.front();
        frontier.pop();
        const int currentDistance = m_distance[static_cast<std::size_t>(current.y * m_width + current.x)];

        for (int i = 0; i < 4; ++i)
        {
            const int nx = current.x + kDx[i];
            const int ny = current.y + kDy[i];
            if (!tiles.isWalkable(nx, ny))
            {
                continue;
            }

            const std::size_t index = static_cast<std::size_t>(ny * m_width + nx);
            if (m_distance[index] != -1)
            {
                continue;
            }

            m_distance[index] = currentDistance + 1;
            frontier.push({nx, ny});
        }
    }
}

int DistanceField::distance(int x, int y) const
{
    if (x < 0 || y < 0 || x >= m_width || y >= m_height)
    {
        return -1;
    }
    return m_distance[static_cast<std::size_t>(y * m_width + x)];
}

sf::Vector2i DistanceField::nextStep(sf::Vector2i from) const
{
    const int current = distance(from.x, from.y);
    if (current <= 0)
    {
        return from;
    }

    sf::Vector2i best = from;
    int bestDistance = current;

    for (int i = 0; i < 4; ++i)
    {
        const sf::Vector2i neighbour{from.x + kDx[i], from.y + kDy[i]};
        const int neighbourDistance = distance(neighbour.x, neighbour.y);
        if (neighbourDistance >= 0 && neighbourDistance < bestDistance)
        {
            best = neighbour;
            bestDistance = neighbourDistance;
        }
    }

    return best;
}