#pragma once

#include <SFML/System/Vector2.hpp>

#include "world/TileMap.hpp"

class CollisionSystem
{
public:
    static sf::Vector2f moveAndCollide(const TileMap& tiles,
                                       float tileSize,
                                       sf::Vector2f position,
                                       sf::Vector2f size,
                                       sf::Vector2f delta);
};