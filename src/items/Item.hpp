#pragma once

enum class ItemType
{
    HealthPotion,
    Weapon,
    Armour
};

enum class Rarity
{
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary
};

struct Item
{
    ItemType type;
    Rarity rarity;
    int power;
};