#include "world/Projectile.hpp"

namespace
{
    constexpr sf::Vector2f kProjectileSize{8.f, 8.f};
}

Projectile::Projectile(Faction faction, sf::Vector2f position, sf::Vector2f direction, float speed, int damage,
                       float range)
    : Entity(position, kProjectileSize)
    , m_faction(faction)
    , m_damage(damage)
    , m_remainingRange(range)
{
    if (direction.lengthSquared() < 0.0001f)
    {
        m_alive = false;
        return;
    }

    m_velocity = direction.normalized() * speed;
}

void Projectile::update(sf::Time dt)
{
    if (!m_alive)
    {
        return;
    }

    const sf::Vector2f step = m_velocity * dt.asSeconds();
    m_position += step;
    m_remainingRange -= step.length();

    if (m_remainingRange <= 0.f)
    {
        m_alive = false;
    }
}

void Projectile::expire()
{
    m_alive = false;
}

Faction Projectile::faction() const
{
    return m_faction;
}

int Projectile::damage() const
{
    return m_damage;
}

bool Projectile::isAlive() const
{
    return m_alive;
}