#include "combat/DamageCalc.hpp"

#include <algorithm>
#include <cmath>

DamageResult rollDamage(int baseDamage, Rng& rng, int critChancePercent, float critMultiplier)
{
    const int percent = rng.range(80, 120);
    int amount = baseDamage * percent / 100;

    const bool critical = rng.chance(critChancePercent);
    if (critical)
    {
        amount = static_cast<int>(std::lround(static_cast<float>(amount) * critMultiplier));
    }

    return {std::max(1, amount), critical};
}