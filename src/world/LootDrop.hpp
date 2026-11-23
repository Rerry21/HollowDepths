#pragma once

#include <SFML/System/Vector2.hpp>

#include "items/Item.hpp"

struct LootDrop
{
    Item item;
    sf::Vector2f position;
};