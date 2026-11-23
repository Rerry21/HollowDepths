#include "items/LootGenerator.hpp"

#include <cstddef>

namespace
{
    constexpr int kDropChancePercent = 35;

    Rarity rollRarity(Rng& rng)
    {
        const int roll = rng.range(0, 99);
        if (roll < 60)
        {
            return Rarity::Common;
        }
        if (roll < 85)
        {
            return Rarity::Uncommon;
        }
        if (roll < 95)
        {
            return Rarity::Rare;
        }
        if (roll < 99)
        {
            return Rarity::Epic;
        }
        return Rarity::Legendary;
    }

    ItemType rollType(Rng& rng)
    {
        const int roll = rng.range(0, 99);
        if (roll < 50)
        {
            return ItemType::HealthPotion;
        }
        if (roll < 80)
        {
            return ItemType::Weapon;
        }
        return ItemType::Armour;
    }

    int powerFor(ItemType type, Rarity rarity)
    {
        constexpr int kPotion[5] = {20, 30, 40, 60, 80};
        constexpr int kWeapon[5] = {2, 3, 5, 8, 12};
        constexpr int kArmour[5] = {1, 2, 3, 4, 6};

        const std::size_t index = static_cast<std::size_t>(rarity);
        switch (type)
        {
        case ItemType::HealthPotion:
            return kPotion[index];
        case ItemType::Weapon:
            return kWeapon[index];
        case ItemType::Armour:
            return kArmour[index];
        }
        return 0;
    }
}

std::optional<Item> rollEnemyDrop(Rng& rng)
{
    if (!rng.chance(kDropChancePercent))
    {
        return std::nullopt;
    }

    const ItemType type = rollType(rng);
    const Rarity rarity = rollRarity(rng);
    return Item{type, rarity, powerFor(type, rarity)};
}