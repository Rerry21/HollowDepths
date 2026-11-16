#pragma once

#include <vector>

#include "world/Dungeon.hpp"
#include "world/Enemy.hpp"

class EnemySpawner
{
public:
    static std::vector<Enemy> spawn(const Dungeon& dungeon, float tileSize);
};