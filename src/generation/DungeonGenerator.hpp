#pragma once

#include <cstdint>

#include "world/Dungeon.hpp"

struct GeneratorConfig
{
    int width = 64;
    int height = 48;
    int minLeafSize = 10;
    int minRoomSize = 4;
    int maxRoomSize = 14;
    int roomMargin = 1;
    int minRooms = 6;
    int extraLoops = 3;
    int maxTreasureRooms = 3;
    int maxAttempts = 20;
};

class DungeonGenerator
{
public:
    explicit DungeonGenerator(GeneratorConfig config = {});

    Dungeon generate(std::uint64_t seed) const;

private:
    GeneratorConfig m_config;
};