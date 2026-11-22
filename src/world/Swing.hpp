#pragma once

#include <SFML/System/Vector2.hpp>

struct Swing
{
    sf::Vector2f origin;
    sf::Vector2f direction{1.f, 0.f};
    float range = 0.f;
    float duration = 0.15f;
    float timeLeft = 0.f;
};