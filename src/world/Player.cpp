#include "world/Player.hpp"

#include <algorithm>

namespace
{
    constexpr float kDefaultSpeed = 220.f;
    constexpr sf::Vector2f kPlayerSize{24.f, 24.f};
    constexpr int kMaxHealth = 100;
    constexpr float kInvulnerabilityTime = 0.8f;
    constexpr float kAttackCooldown = 0.4f;
    constexpr int kAttackDamage = 12;
    constexpr float kShotCooldown = 0.6f;
    constexpr int kShotDamage = 8;
}

Player::Player(sf::Vector2f position)
    : Entity(position, kPlayerSize)
    , m_speed(kDefaultSpeed)
    , m_health(kMaxHealth)
{
}

void Player::update(sf::Time dt)
{
    const float seconds = dt.asSeconds();
    m_invulnerableTimer = std::max(0.f, m_attackTimer - seconds);
    m_attackTimer = std::max(0.f, m_attackTimer - seconds);
    m_shotTimer = std::max(0.f, m_shotTimer - seconds);

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

bool Player::tryAttack()
{
    if (m_health.isDead() || m_attackTimer > 0.f)
    {
        return false;
    }

    m_attackTimer = kAttackCooldown;
    return true;
}

void Player::faceTowards(float dx)
{
    if (dx > 0.f)
    {
        m_facing = Facing::Right;
    }
    else if (dx < 0.f)
    {
        m_facing = Facing::Left;
    }
}

int Player::attackDamage() const
{
    return kAttackDamage;
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

bool Player::tryShoot()
{
    if (m_health.isDead() || m_shotTimer > 0.f)
    {
        return false;
    }

    m_shotTimer = kShotCooldown;
    return true;
}

int Player::shotDamage() const
{
    return kShotDamage;
}