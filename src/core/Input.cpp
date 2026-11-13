#include "core/Input.hpp"

#include <algorithm>

Input::Input()
{
    using Key = sf::Keyboard::Key;

    m_bindings[InputAction::MoveUp] = {Key::W, Key::Up};
    m_bindings[InputAction::MoveDown] = {Key::S, Key::Down};
    m_bindings[InputAction::MoveLeft] = {Key::A, Key::Left};
    m_bindings[InputAction::MoveRight] = {Key::D, Key::Right};
}

bool Input::isHeld(InputAction action) const
{
    const auto it = m_bindings.find(action);
    if (it == m_bindings.end())
    {
        return false;
    }

    return std::any_of(it->second.begin(), it->second.end(),[](sf::Keyboard::Key key) { return sf::Keyboard::isKeyPressed(key); });
}

sf::Vector2f Input::movementAxis() const
{
    sf::Vector2f axis{0.f, 0.f};
    if (isHeld(InputAction::MoveUp))
    {
        axis.y -= 1.f;
    }
    if (isHeld(InputAction::MoveDown))
    {
        axis.y += 1.f;
    }
    if (isHeld(InputAction::MoveLeft))
    {
        axis.x -= 1.f;
    }
    if (isHeld(InputAction::MoveRight))
    {
        axis.x += 1.f;
    }

    if (axis.x != 0.f && axis.y != 0.f)
    {
        axis = axis.normalized();
    }

    return axis;
}