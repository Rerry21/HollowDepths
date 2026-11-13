#pragma once

#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>

enum class InputAction
{
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight
};

class Input
{
public:
    Input();

    bool isHeld(InputAction action) const;
    sf::Vector2f movementAxis() const;

private:
    std::unordered_map<InputAction, std::vector<sf::Keyboard::Key>> m_bindings;
};