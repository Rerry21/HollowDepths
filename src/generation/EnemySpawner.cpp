#include "generation/EnemySpawner.hpp"

#include <algorithm>

#include "ai/Perception.hpp"
#include "generation/Rng.hpp"

namespace
{
    constexpr int kSpawnStream = 1000;
    constexpr int kTilesPerEnemy = 45;
    constexpr int kMaxPerRoom = 5;
    constexpr int kHeavyMinArea = 80;
    constexpr int kPlacementAttempts = 10;
    constexpr int kGruntWeight = 60;
    constexpr int kRunnerWeight = 25;
    constexpr int kHeavyWeight = 15;

    EnemyType pickType(Rng& rng, int roomArea)
    {
        const int heavyWeight = roomArea >= kHeavyMinArea ? kHeavyWeight : 0;
        const int total = kGruntWeight + kRunnerWeight + heavyWeight;
        const int roll = rng.range(0, total - 1);

        if (roll < kGruntWeight)
        {
            return EnemyType::Grunt;
        }
        if (roll < kGruntWeight + kRunnerWeight)
        {
            return EnemyType::Runner;
        }
        return EnemyType::Heavy;
    }
}

std::vector<Enemy> EnemySpawner::spawn(const Dungeon& dungeon, float tileSize)
{
    Rng rng(Rng::deriveSeed(dungeon.seed(), kSpawnStream));
    std::vector<Enemy> enemies;
    int enemyIndex = 0;

    for (const Room& room : dungeon.rooms())
    {
        if (room.type == RoomType::Start)
        {
            continue;
        }

        const int area = room.bounds.size.x * room.bounds.size.y;
        int count = std::max(1, area / kTilesPerEnemy);
        if (room.type == RoomType::Treasure || room.type == RoomType::Exit)
        {
            ++count;
        }
        count = std::min(count, kMaxPerRoom);

        const int innerLeft = room.bounds.position.x + 1;
        const int innerTop = room.bounds.position.y + 1;
        const int innerWidth = std::max(1, room.bounds.size.x - 2);
        const int innerHeight = std::max(1, room.bounds.size.y - 2);

        std::vector<sf::Vector2i> used;
        for (int n = 0; n < count; ++n)
        {
            sf::Vector2i tile{};
            bool placed = false;

            for (int attempt = 0; attempt < kPlacementAttempts; ++attempt)
            {
                tile = {innerLeft + rng.range(0, innerWidth - 1), innerTop + rng.range(0, innerHeight - 1)};
                if (std::find(used.begin(), used.end(), tile) == used.end())
                {
                    placed = true;
                    break;
                }
            }

            if (!placed)
            {
                break;
            }

            used.push_back(tile);
            const EnemyType type = pickType(rng, area);
            const std::uint64_t enemySeed = Rng::deriveSeed(dungeon.seed(), kSpawnStream + 1 + enemyIndex);
            ++enemyIndex;
            enemies.emplace_back(type, tileCentre(tile, tileSize), enemySeed);
        }
    }

    return enemies;
}