#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

enum class RoomType
{
    Normal,
    Start,
    Exit,
    Treasure
};

struct Room
{
    sf::IntRect bounds;
    RoomType type = RoomType::Normal;

    sf::Vector2i center() const
    {
        return {bounds.position.x + bounds.size.x / 2, bounds.position.y + bounds.size.y / 2};
    }
};