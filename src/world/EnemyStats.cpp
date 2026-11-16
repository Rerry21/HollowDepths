#include "world/EnemyStats.hpp"

namespace
{
    const EnemyStats kGrunt{
        .name = "Grunt",
        .maxHealth = 30,
        .speed = 90.f,
        .damage = 10,
        .size = 24.f,
        .detectRadius = 220.f,
        .loseRadius = 360.f,
        .attackRange = 36.f,
        .windupTime = 0.35f,
        .recoverTime = 0.5f,
        .retreatTime = 0.f,
    };

    const EnemyStats kRunner{
        .name = "Runner",
        .maxHealth = 18,
        .speed = 170.f,
        .damage = 6,
        .size = 18.f,
        .detectRadius = 260.f,
        .loseRadius = 420.f,
        .attackRange = 30.f,
        .windupTime = 0.15f,
        .recoverTime = 0.3f,
        .retreatTime = 0.9f,
    };

    const EnemyStats kHeavy{
        .name = "Heavy",
        .maxHealth = 80,
        .speed = 55.f,
        .damage = 25,
        .size = 28.f,
        .detectRadius = 200.f,
        .loseRadius = 320.f,
        .attackRange = 42.f,
        .windupTime = 0.7f,
        .recoverTime = 0.9f,
        .retreatTime = 0.f,
    };
}

const EnemyStats& statsFor(EnemyType type)
{
    switch (type)
    {
    case EnemyType::Grunt:
        return kGrunt;
    case EnemyType::Runner:
        return kRunner;
    case EnemyType::Heavy:
        return kHeavy;
    }
    return kGrunt;
}