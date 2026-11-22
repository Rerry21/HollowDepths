#pragma once

#include <vector>

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include "ai/DistanceField.hpp"
#include "generation/Rng.hpp"
#include "world/Dungeon.hpp"
#include "world/Enemy.hpp"
#include "world/Player.hpp"
#include "world/Projectile.hpp"
#include "world/Swing.hpp"

inline constexpr float kTileSize = 32.f;

class World
{
public:
    explicit World(Dungeon dungeon);

    void update(sf::Time dt, sf::Vector2f playerDirection);
    void playerMeleeAttack(sf::Vector2f aim);

    const Dungeon& dungeon() const;
    const Player& player() const;
    Player& player();
    const std::vector<Enemy>& enemies() const;
    const std ::vector<Projectile>& projectiles() const;
    const Swing& swing() const;

    bool damageNearestEnemy(int amount);

private:
    void updatePlayerField();
    void updateEnemies(sf::Time dt);
    void separateEnemies();
    void updateProjectiles(sf::Time dt);

    Dungeon m_dungeon;
    Player m_player;
    std::vector<Enemy> m_enemies;
    std::vector<Projectile> m_projectiles;
    Rng m_combatRng;
    Swing m_swing;
    DistanceField m_playerField;
    sf::Vector2i m_playerTile{-1, -1};
};