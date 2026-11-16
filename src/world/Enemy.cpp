#include "world/Enemy.hpp"

#include <algorithm>

namespace
{
    constexpr float kHurtFlashTime = 0.12f;
}

Enemy::Enemy(EnemyType type, sf::Vector2f position, std::uint64_t seed)
    : Entity(position, sf::Vector2f(statsFor(type).size, statsFor(type).size))
    , m_type(type)
    , m_stats(&statsFor(type))
    , m_health(statsFor(type).maxHealth)
    , m_ai(statsFor(type), position, seed)
{
}

void Enemy::update(sf::Time dt)
{
    if (m_hurtTimer > 0.f)
    {
        m_hurtTimer = std::max(0.f, m_hurtTimer - dt.asSeconds());
    }
}

void Enemy::think(const AIContext& context, sf::Time dt)
{
    const AICommand command = m_ai.update(context, dt);
    m_velocity = command.velocityScale * m_stats->speed;

    if (command.strike)
    {
        m_strikePending = true;
    }
}

bool Enemy::consumeStrike()
{
    const bool pending = m_strikePending;
    m_strikePending = false;
    return pending;
}

bool Enemy::takeDamage(int amount)
{
    if (amount <= 0 || m_health.isDead())
    {
        return false;
    }

    m_health.damage(amount);
    m_hurtTimer = kHurtFlashTime;
    return true;
}

EnemyType Enemy::type() const
{
    return m_type;
}

const EnemyStats& Enemy::stats() const
{
    return *m_stats;
}

const Health& Enemy::health() const
{
    return m_health;
}

AIState Enemy::aiState() const
{
    return m_ai.state();
}

bool Enemy::isWindingUp() const
{
    return m_ai.isWindingUp();
}

bool Enemy::isDead() const
{
    return m_health.isDead();
}

bool Enemy::isHurtFlashing() const
{
    return m_hurtTimer > 0.f;
}