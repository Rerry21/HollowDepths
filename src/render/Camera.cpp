#include "render/Camera.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kSmoothing = 8.f;
}

Camera::Camera(sf::Vector2f viewSize)
    : m_view(viewSize / 2.f, viewSize)
    , m_position(viewSize / 2.f)
    , m_bounds({0.f, 0.f}, viewSize)
{
}

void Camera::setBounds(sf::FloatRect bounds)
{
    m_bounds = bounds;
    m_position = clampCentre(m_position);
    applyPosition();
}

void Camera::snapTo(sf::Vector2f target)
{
    m_position = clampCentre(target);
    applyPosition();
}

void Camera::follow(sf::Vector2f target, sf::Time dt)
{
    const float factor = 1.f - std::exp(-kSmoothing * dt.asSeconds());
    m_position = clampCentre(m_position + (target - m_position) * factor);
    applyPosition();
}

const sf::View& Camera::view() const
{
    return m_view;
}

sf::Vector2f Camera::clampCentre(sf::Vector2f centre) const
{
    const sf::Vector2f viewSize = m_view.getSize();
    const sf::Vector2f half = viewSize / 2.f;
    sf::Vector2f result = centre;

    if (m_bounds.size.x <= viewSize.x)
    {
        result.x = m_bounds.position.x + m_bounds.size.x / 2.f;
    }
    else
    {
        result.x = std::clamp(centre.x, m_bounds.position.x + half.x, m_bounds.position.x + m_bounds.size.x - half.x);
    }

    if (m_bounds.size.y <= viewSize.y)
    {
        result.y = m_bounds.position.y + m_bounds.size.y / 2.f;
    }
    else
    {
        result.y = std::clamp(centre.y, m_bounds.position.y + half.y, m_bounds.position.y + m_bounds.size.y - half.y);
    }

    return result;
}

void Camera::applyPosition()
{
    m_view.setCenter({std::round(m_position.x), std::round(m_position.y)});
}