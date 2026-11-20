#include "world/World.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>

#include "ai/Perception.hpp"
#include "combat/CollisionSystem.hpp"
#include "generation/EnemySpawner.hpp"

namespace
{
    constexpr float kStrikeSlack = 8.f;
    constexpr float kProjectileRangeFactor = 1.5f;

    sf::Vector2f spawnPosition(const Dungeon& dungeon)
    {
        const Room* start = dungeon.findRoom(RoomType::Start);
        if (start == nullptr)
        {
            throw std::runtime_error("Dungeon has no start room");
        }

        const sf::Vector2i centre = start->center();
        return tileCentre(centre, kTileSize);
    }
}

World::World(Dungeon dungeon)
    : m_dungeon(std::move(dungeon))
    , m_player(spawnPosition(m_dungeon))
    , m_enemies(EnemySpawner::spawn(m_dungeon, kTileSize))
{
    updatePlayerField();
}

void World::update(sf::Time dt, sf::Vector2f playerDirection)
{
    m_player.setMoveDirection(playerDirection);
    m_player.update(dt);

    const sf::Vector2f delta = m_player.velocity() * dt.asSeconds();
    m_player.setPosition(CollisionSystem::moveAndCollide(m_dungeon.tiles(), kTileSize, m_player.position(),
                                                         m_player.size(), delta));

    updatePlayerField();
    updateEnemies(dt);
    updateProjectiles(dt);
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

const std::vector<Enemy>& World::enemies() const
{
    return m_enemies;
}

bool World::damageNearestEnemy(int amount)
{
    Enemy* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (Enemy& enemy : m_enemies)
    {
        const float distance = (enemy.position() - m_player.position()).lengthSquared();
        if (distance < nearestDistance)
        {
            nearestDistance = distance;
            nearest = &enemy;
        }
    }

    if (nearest == nullptr)
    {
        return false;
    }

    const bool hit = nearest->takeDamage(amount);
    std::erase_if(m_enemies, [](const Enemy& enemy) { return enemy.isDead(); });
    return hit;
}

void World::updatePlayerField()
{
    const sf::Vector2i tile = tileOf(m_player.position(), kTileSize);
    if (tile == m_playerTile)
    {
        return;
    }

    m_playerTile = tile;
    m_playerField.rebuild(m_dungeon.tiles(), tile);
}

void World::updateEnemies(sf::Time dt)
{
    const TileMap& tiles = m_dungeon.tiles();
    const bool playerAlive = !m_player.health().isDead();

    for (Enemy& enemy : m_enemies)
    {
        const AIContext context{enemy.position(), m_player.position(), playerAlive, tiles, m_playerField, kTileSize};
        enemy.think(context, dt);
        enemy.update(dt);

        const sf::Vector2f delta = enemy.velocity() * dt.asSeconds();
        enemy.setPosition(CollisionSystem::moveAndCollide(tiles, kTileSize, enemy.position(), enemy.size(), delta));

        if (enemy.consumeStrike())
        {
            const EnemyStats& stats = enemy.stats();
            if (stats.projectileSpeed > 0.f)
            {
                m_projectiles.emplace_back(enemy.position(), m_player.position(),
                                            stats.projectileSpeed, stats.damage,
                                            stats.attackRange * kProjectileRangeFactor);
            }
            else
            {
                const float distance = (m_player.position() - enemy.position()).length();
                if (distance <= stats.attackRange + kStrikeSlack)
                {
                    m_player.takeDamage(stats.damage);
                }
            }
        }

    separateEnemies();
    std::erase_if(m_enemies, [](const Enemy& enemy) { return enemy.isDead(); });
    }
}

void World::separateEnemies()
{
    const TileMap& tiles = m_dungeon.tiles();

    for (std::size_t i = 0; i < m_enemies.size(); ++i)
    {
        for (std::size_t j = i + 1; j < m_enemies.size(); ++j)
        {
            Enemy& a = m_enemies[i];
            Enemy& b = m_enemies[j];

            const sf::Vector2f offset = b.position() - a.position();
            const float minDistance = (a.size().x + b.size().x) / 2.f;
            const float distanceSquared = offset.lengthSquared();
            if (distanceSquared >= minDistance * minDistance)
            {
                continue;
            }

            float distance = std::sqrt(distanceSquared);
            sf::Vector2f direction{1.f, 0.f};
            if (distance >= 0.001f)
            {
                direction = offset / distance;
            }
            else
            {
                distance = 0.f;
            }

            const sf::Vector2f push = direction * ((minDistance - distance) / 2.f);
            a.setPosition(CollisionSystem::moveAndCollide(tiles, kTileSize, a.position(), a.size(), -push));
            b.setPosition(CollisionSystem::moveAndCollide(tiles, kTileSize, b.position(), b.size(), push));
        }
    }
}

const std::vector<Projectile>& World::projectiles() const
{
    return m_projectiles;
}

void World::updateProjectiles(sf::Time dt)
{
    const TileMap& tiles = m_dungeon.tiles();

    for (Projectile& projectile : m_projectiles)
    {
        projectile.update(dt);
        if (!projectile.isAlive())
        {
            continue;
        }

        const sf::Vector2i tile = tileOf(projectile.position(), kTileSize);
        if (!tiles.isWalkable(tile.x, tile.y))
        {
            projectile.expire();
        }
        else if (projectile.bounds().findIntersection(m_player.bounds()).has_value())
        {
            m_player.takeDamage(projectile.damage());
            projectile.expire();
        }
    }

    std::erase_if(m_projectiles, [](const Projectile& projectile) { return !projectile.isAlive(); });
}