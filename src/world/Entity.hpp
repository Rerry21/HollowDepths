#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

class Entity
{
public:
    Entity(sf::Vector2f position, sf::Vector2f size);
    virtual ~Entity() = default;

    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;

    virtual void update(sf::Time dt) = 0;

    sf::Vector2f position() const;
    sf::Vector2f size() const;
    sf::Vector2f velocity() const;
    sf::FloatRect bounds() const;
    void setPosition(sf::Vector2f position);

protected:
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    sf::Vector2f m_velocity{0.f, 0.f};
};