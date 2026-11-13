#include "world/Player.hpp"

namespace
{
    constexpr float kDefaultSpeed = 220.f;
    constexpr sf::Vector2f kPlayerSize{32.f, 32.f};
}

Player::Player(sf::Vector2f position)
    : Entity(position, kPlayerSize)
    , m_speed(kDefaultSpeed)
{
}

void Player::update(sf::Time dt)
{
    m_position += m_moveDirection * m_speed * dt.asSeconds();
}

void Player::setMoveDirection(sf::Vector2f direction)
{
    m_moveDirection = direction;
}