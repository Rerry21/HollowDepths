#pragma once

#include <SFML/Graphics.hpp>

#include "world/Dungeon.hpp"

class DungeonView
{
public:
    DungeonView();

    void rebuild(const Dungeon& dungeon, float tileSize, sf::Vector2f origin);
    void draw(sf::RenderTarget& target) const;

private:
    sf::VertexArray m_vertices;
};