#pragma once

#include <SFML/System/Vector2.hpp>

#include "world/TileMap.hpp"

sf::Vector2i tileOf(sf::Vector2f position, float tileSize);
sf::Vector2f tileCentre(sf::Vector2i tile, float tileSize);

bool hasLineOfSight(const TileMap& tiles, float tileSize, sf::Vector2f from, sf::Vector2f to, float radius = 0.f);