#include "ai/Perception.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    bool rayClear(const TileMap& tiles, float tileSize, sf::Vector2f from, sf::Vector2f to)
    {
        const sf::Vector2f delta = to - from;
        const float step = tileSize * 0.25f;
        const int samples = std::max(1, static_cast<int>(std::ceil(delta.length() / step)));

        for (int i = 0; i <= samples; ++i)
        {
            const float t = static_cast<float>(i) / static_cast<float>(samples);
            const sf::Vector2i tile = tileOf(from + delta * t, tileSize);
            if (!tiles.isWalkable(tile.x, tile.y))
            {
                return false;
            }
        }
        return true;
    }
}

sf::Vector2i tileOf(sf::Vector2f position, float tileSize)
{
    return {static_cast<int>(std::floor(position.x / tileSize)), static_cast<int>(std::floor(position.y / tileSize))};
}

sf::Vector2f tileCentre(sf::Vector2i tile, float tileSize)
{
    return {(static_cast<float>(tile.x) + 0.5f) * tileSize, (static_cast<float>(tile.y) + 0.5f) * tileSize};
}

bool hasLineOfSight(const TileMap& tiles, float tileSize, sf::Vector2f from, sf::Vector2f to, float radius)
{
    if (!rayClear(tiles, tileSize, from, to))
    {
        return false;
    }

    if (radius <= 0.f)
    {
        return true;
    }

    const sf::Vector2f delta = to - from;
    if (delta.lengthSquared() < 0.0001f)
    {
        return true;
    }

    const sf::Vector2f offset = delta.normalized().perpendicular() * radius;
    return rayClear(tiles, tileSize, from + offset, to + offset) &&
           rayClear(tiles, tileSize, from - offset, to - offset);
}