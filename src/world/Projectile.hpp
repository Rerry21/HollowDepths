#pragma once

#include "world/Entity.hpp"

class Projectile : public Entity
{
public:
    Projectile(sf::Vector2f position, sf::Vector2f direction, float speed, int damage, float range);

    void update(sf::Time dt) override;
    void expire();

    int damage() const;
    bool isAlive() const;

private:
    int m_damage;
    float m_remainingRange;
    bool m_alive = true;
};