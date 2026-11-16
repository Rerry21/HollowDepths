#pragma once

enum class EnemyType
{
    Grunt,
    Runner,
    Heavy
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
};

const EnemyStats& statsFor(EnemyType type);