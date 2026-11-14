#pragma once

#include <SFML/Graphics.hpp>

class Camera
{
public:
    explicit Camera(sf::Vector2f viewSize);

    void setBounds(sf::FloatRect bounds);
    void snapTo(sf::Vector2f target);
    void follow(sf::Vector2f target, sf::Time dt);

    const sf::View& view() const;

private:
    sf::Vector2f clampCentre(sf::Vector2f centre) const;
    void applyPosition();

    sf::View m_view;
    sf::Vector2f m_position;
    sf::FloatRect m_bounds;
};