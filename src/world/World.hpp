#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include "world/Dungeon.hpp"
#include "world/Player.hpp"

inline constexpr float kTileSize = 32.f;

class World
{
public:
    explicit World(Dungeon dungeon);

    void update(sf::Time dt, sf::Vector2f playerDirection);

    const Dungeon& dungeon() const;
    const Player& player() const;
    Player& player();

private:
    Dungeon m_dungeon;
    Player m_player;
};