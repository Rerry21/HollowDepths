#pragma once

#include "world/Entity.hpp"
#include "world/Health.hpp"

enum class Facing
{
    Left,
    Right
};

class Player : public Entity
{
public:
    explicit Player(sf::Vector2f position);

    void update(sf::Time dt) override;
    void setMoveDirection(sf::Vector2f direction);

    bool takeDamage(int amount);
    void heal(int amount);

    const Health& health() const;
    Facing facing() const;
    bool isInvulnerable() const;
    float invulnerabilityRemaining() const;

private:
    sf::Vector2f m_moveDirection{0.f, 0.f};
    float m_speed;
    Health m_health;
    float m_invulnerableTimer = 0.f;
    Facing m_facing = Facing::Right;
};