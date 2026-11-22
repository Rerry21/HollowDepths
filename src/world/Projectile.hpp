#pragma once

#include "world/Entity.hpp"

enum class Faction
{
    Player,
    Enemy
};

class Projectile : public Entity
{
public:
    Projectile(Faction faction, sf::Vector2f position, sf::Vector2f direction, float speed, int damage, float range);

    void update(sf::Time dt) override;
    void expire();

    Faction faction() const;
    int damage() const;
    bool isAlive() const;

private:
    Faction m_faction;
    int m_damage;
    float m_remainingRange;
    bool m_alive = true;
};