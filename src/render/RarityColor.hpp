#pragma once

#include "items/Item.hpp"

inline sf::Color rarityColor(Rarity rarity)
{
    switch (rarity)
    {
        case Rarity::Common:
            return sf::Color(200, 200, 200);
        case Rarity::Uncommon:
            return sf::Color(90, 200, 90);
        case Rarity::Rare:
            return sf::Color(70, 130, 255);
        case Rarity::Epic:
            return sf::Color(180, 90, 230);
        case Rarity::Legendary:
            return sf::Color(255, 150, 30);
    }
    return sf::Color::White;
}