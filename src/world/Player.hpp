#pragma once

#include "world/Entity.hpp"

class Player : public Entity
{
public:
    explicit Player(sf::Vector2f position);

    void update(sf::Time dt) override;
    void setMoveDirection(sf::Vector2f direction);

private:
    sf::Vector2f m_moveDirection{0.f, 0.f};
    float m_speed;
};