#pragma once

enum class EnemyType
{
    Grunt,
    Runner,
    Heavy,
    Archer
};

struct EnemyStats
{
    const char* name;
    int maxHealth;
    float speed;
    int damage;
    float size;
    float detectRadius;
    float loseRadius;
    float attackRange;
    float windupTime;
    float recoverTime;
    float retreatTime;
    float projectileSpeed = 0.f;
};

const EnemyStats& statsFor(EnemyType type);