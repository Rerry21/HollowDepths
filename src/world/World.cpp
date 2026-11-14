#include "world/World.hpp"

#include <stdexcept>
#include <utility>

#include "combat/CollisionSystem.hpp"

namespace
{
    sf::Vector2f spawnPosition(const Dungeon& dungeon)
    {
        const Room* start = dungeon.findRoom(RoomType::Start);
        if (start == nullptr)
        {
            throw std::runtime_error("Dungeon has no start room");
        }

        const sf::Vector2i centre = start->center();
        return {(static_cast<float>(centre.x) + 0.5f) * kTileSize, (static_cast<float>(centre.y) + 0.5f) * kTileSize};
    }
}

World::World(Dungeon dungeon)
    : m_dungeon(std::move(dungeon))
    , m_player(spawnPosition(m_dungeon))
{
}

void World::update(sf::Time dt, sf::Vector2f playerDirection)
{
    m_player.setMoveDirection(playerDirection);
    m_player.update(dt);

    const sf::Vector2f delta = m_player.velocity() * dt.asSeconds();
    m_player.setPosition(CollisionSystem::moveAndCollide(m_dungeon.tiles(), kTileSize, m_player.position(),
                                                         m_player.size(), delta));
}

const Dungeon& World::dungeon() const
{
    return m_dungeon;
}

const Player& World::player() const
{
    return m_player;
}

Player& World::player()
{
    return m_player;
}