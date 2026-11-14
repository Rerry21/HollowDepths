#include "world/Player.hpp"

#include <algorithm>

namespace
{
    constexpr float kDefaultSpeed = 220.f;
    constexpr sf::Vector2f kPlayerSize{24.f, 24.f};
    constexpr int kMaxHealth = 100;
    constexpr float kInvulnerabilityTime = 0.8f;
}

Player::Player(sf::Vector2f position)
    : Entity(position, kPlayerSize)
    , m_speed(kDefaultSpeed)
    , m_health(kMaxHealth)
{
}

void Player::update(sf::Time dt)
{
    if (m_invulnerableTimer > 0.f)
    {
        m_invulnerableTimer = std::max(0.f, m_invulnerableTimer - dt.asSeconds());
    }

    if (m_health.isDead())
    {
        m_velocity = {0.f, 0.f};
        return;
    }

    m_velocity = m_moveDirection * m_speed;

    if (m_moveDirection.x > 0.f)
    {
        m_facing = Facing::Right;
    }
    else if (m_moveDirection.x < 0.f)
    {
        m_facing = Facing::Left;
    }
}

void Player::setMoveDirection(sf::Vector2f direction)
{
    m_moveDirection = direction;
}

bool Player::takeDamage(int amount)
{
    if (amount <= 0 || m_health.isDead() || isInvulnerable())
    {
        return false;
    }

    m_health.damage(amount);
    m_invulnerableTimer = kInvulnerabilityTime;
    return true;
}

void Player::heal(int amount)
{
    m_health.heal(amount);
}

const Health& Player::health() const
{
    return m_health;
}

Facing Player::facing() const
{
    return m_facing;
}

bool Player::isInvulnerable() const
{
    return m_invulnerableTimer > 0.f;
}

float Player::invulnerabilityRemaining() const
{
    return m_invulnerableTimer;
}