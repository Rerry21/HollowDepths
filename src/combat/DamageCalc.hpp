#pragma once

#include "generation/Rng.hpp"

struct DamageResult
{
    int amount;
    bool critical;
};

DamageResult rollDamage(int baseDamage, Rng& rng, int critChancePercent, float critMultiplier);