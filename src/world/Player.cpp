#include "world/Player.hpp"

namespace
{
    constexpr float kDefaultSpeed = 220.f;
    constexpr sf::Vector2f kPlayerSize{24.f, 24.f};
}

Player::Player(sf::Vector2f position)
    : Entity(position, kPlayerSize)
    , m_speed(kDefaultSpeed)
{
}

void Player::update(sf::Time)
{
    m_velocity = m_moveDirection * m_speed;
}

void Player::setMoveDirection(sf::Vector2f direction)
{
    m_moveDirection = direction;
}