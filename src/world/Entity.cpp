#include "world/Entity.hpp"

Entity::Entity(sf::Vector2f position, sf::Vector2f size)
    : m_position(position)
    , m_size(size)
{
}

sf::Vector2f Entity::position() const
{
    return m_position;
}

sf::Vector2f Entity::size() const
{
    return m_size;
}

sf::FloatRect Entity::bounds() const
{
    return sf::FloatRect(m_position - m_size / 2.f, m_size);
}

void Entity::setPosition(sf::Vector2f position)
{
    m_position = position;
}