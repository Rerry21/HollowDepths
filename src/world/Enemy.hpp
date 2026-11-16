#pragma once

#include <cstdint>

#include "ai/EnemyAI.hpp"
#include "world/EnemyStats.hpp"
#include "world/Entity.hpp"
#include "world/Health.hpp"

class Enemy : public Entity
{
public:
    Enemy(EnemyType type, sf::Vector2f position, std::uint64_t seed);

    void update(sf::Time dt) override;
    void think(const AIContext& context, sf::Time dt);

    bool consumeStrike();
    bool takeDamage(int amount);

    EnemyType type() const;
    const EnemyStats& stats() const;
    const Health& health() const;
    AIState aiState() const;
    bool isWindingUp() const;
    bool isDead() const;
    bool isHurtFlashing() const;

private:
    EnemyType m_type;
    const EnemyStats* m_stats;
    Health m_health;
    EnemyAI m_ai;
    bool m_strikePending = false;
    float m_hurtTimer = 0.f;
};