#pragma once

#include <vector>

#include <SFML/System/Vector2.hpp>

#include "world/TileMap.hpp"

class DistanceField
{
public:
    void rebuild(const TileMap& tiles, sf::Vector2i target);

    int distance(int x, int y) const;
    sf::Vector2i nextStep(sf::Vector2i from) const;

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<int> m_distance;
};