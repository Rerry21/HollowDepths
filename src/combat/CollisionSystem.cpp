#include "combat/CollisionSystem.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kEpsilon = 0.001f;
    constexpr float kMaxStepFraction = 0.5f;

    int tileIndex(float coordinate, float tileSize)
    {
        return static_cast<int>(std::floor(coordinate / tileSize));
    }

    int firstTile(float edge, float tileSize)
    {
        return tileIndex(edge + kEpsilon, tileSize);
    }

    int lastTile(float edge, float tileSize)
    {
        return tileIndex(edge - kEpsilon, tileSize);
    }

    bool overlapsSolid(const TileMap& tiles, float tileSize, sf::Vector2f centre, sf::Vector2f half)
    {
        const int left = firstTile(centre.x - half.x, tileSize);
        const int right = lastTile(centre.x + half.x, tileSize);
        const int top = firstTile(centre.y - half.y, tileSize);
        const int bottom = lastTile(centre.y + half.y, tileSize);

        for (int y = top; y <= bottom; ++y)
        {
            for (int x = left; x <= right; ++x)
            {
                if (!tiles.isWalkable(x, y))
                {
                    return true;
                }
            }
        }
        return false;
    }

    void stepHorizontal(const TileMap& tiles, float tileSize, sf::Vector2f& position, sf::Vector2f half, float amount)
    {
        if (amount == 0.f)
        {
            return;
        }

        position.x += amount;
        if (!overlapsSolid(tiles, tileSize, position, half))
        {
            return;
        }

        if (amount > 0.f)
        {
            const int column = lastTile(position.x + half.x, tileSize);
            position.x = static_cast<float>(column) * tileSize - half.x;
        }
        else
        {
            const int column = firstTile(position.x - half.x, tileSize);
            position.x = static_cast<float>(column + 1) * tileSize + half.x;
        }
    }

    void stepVertical(const TileMap& tiles, float tileSize, sf::Vector2f& position, sf::Vector2f half, float amount)
    {
        if (amount == 0.f)
        {
            return;
        }

        position.y += amount;
        if (!overlapsSolid(tiles, tileSize, position, half))
        {
            return;
        }

        if (amount > 0.f)
        {
            const int row = lastTile(position.y + half.y, tileSize);
            position.y = static_cast<float>(row) * tileSize - half.y;
        }
        else
        {
            const int row = firstTile(position.y - half.y, tileSize);
            position.y = static_cast<float>(row + 1) * tileSize + half.y;
        }
    }
}

sf::Vector2f CollisionSystem::moveAndCollide(const TileMap& tiles,
                                             float tileSize,
                                             sf::Vector2f position,
                                             sf::Vector2f size,
                                             sf::Vector2f delta)
{
    const sf::Vector2f half = size / 2.f;

    const float longest = std::max(std::abs(delta.x), std::abs(delta.y));
    const int steps = std::max(1, static_cast<int>(std::ceil(longest / (tileSize * kMaxStepFraction))));
    const sf::Vector2f stepDelta = delta / static_cast<float>(steps);

    for (int i = 0; i < steps; ++i)
    {
        stepHorizontal(tiles, tileSize, position, half, stepDelta.x);
        stepVertical(tiles, tileSize, position, half, stepDelta.y);
    }

    return position;
}