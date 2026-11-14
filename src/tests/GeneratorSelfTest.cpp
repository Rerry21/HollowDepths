#include "tests/GeneratorSelfTest.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>

#include "generation/DungeonGenerator.hpp"

namespace
{
    constexpr int kMaxPrintedFailures = 10;

    bool sameDungeon(const Dungeon& a, const Dungeon& b)
    {
        const TileMap& tilesA = a.tiles();
        const TileMap& tilesB = b.tiles();

        if (tilesA.width() != tilesB.width() || tilesA.height() != tilesB.height())
        {
            return false;
        }

        for (int y = 0; y < tilesA.height(); ++y)
        {
            for (int x = 0; x < tilesA.width(); ++x)
            {
                if (tilesA.at(x, y) != tilesB.at(x, y))
                {
                    return false;
                }
            }
        }

        if (a.rooms().size() != b.rooms().size())
        {
            return false;
        }

        for (std::size_t i = 0; i < a.rooms().size(); ++i)
        {
            const Room& roomA = a.rooms()[i];
            const Room& roomB = b.rooms()[i];

            if (roomA.type != roomB.type || roomA.bounds.position.x != roomB.bounds.position.x ||
                roomA.bounds.position.y != roomB.bounds.position.y || roomA.bounds.size.x != roomB.bounds.size.x ||
                roomA.bounds.size.y != roomB.bounds.size.y)
            {
                return false;
            }
        }

        return a.seed() == b.seed();
    }

    int countFloorTiles(const TileMap& tiles)
    {
        int count = 0;
        for (int y = 0; y < tiles.height(); ++y)
        {
            for (int x = 0; x < tiles.width(); ++x)
            {
                if (tiles.isWalkable(x, y))
                {
                    ++count;
                }
            }
        }
        return count;
    }

    int reachableFloorTiles(const TileMap& tiles, sf::Vector2i start)
    {
        if (!tiles.isWalkable(start.x, start.y))
        {
            return 0;
        }

        const int width = tiles.width();
        std::vector<char> visited(static_cast<std::size_t>(width) * static_cast<std::size_t>(tiles.height()), 0);
        std::queue<sf::Vector2i> frontier;

        visited[static_cast<std::size_t>(start.y * width + start.x)] = 1;
        frontier.push(start);

        constexpr int kDx[4] = {1, -1, 0, 0};
        constexpr int kDy[4] = {0, 0, 1, -1};

        int count = 0;
        while (!frontier.empty())
        {
            const sf::Vector2i current = frontier.front();
            frontier.pop();
            ++count;

            for (int i = 0; i < 4; ++i)
            {
                const int nx = current.x + kDx[i];
                const int ny = current.y + kDy[i];
                if (!tiles.isWalkable(nx, ny))
                {
                    continue;
                }

                const std::size_t index = static_cast<std::size_t>(ny * width + nx);
                if (visited[index] != 0)
                {
                    continue;
                }

                visited[index] = 1;
                frontier.push({nx, ny});
            }
        }

        return count;
    }

    bool overlaps(const Room& a, const Room& b)
    {
        return a.bounds.position.x < b.bounds.position.x + b.bounds.size.x &&
               b.bounds.position.x < a.bounds.position.x + a.bounds.size.x &&
               a.bounds.position.y < b.bounds.position.y + b.bounds.size.y &&
               b.bounds.position.y < a.bounds.position.y + a.bounds.size.y;
    }

    std::string validate(const Dungeon& dungeon, const GeneratorConfig& config)
    {
        const TileMap& tiles = dungeon.tiles();
        const std::vector<Room>& rooms = dungeon.rooms();

        if (static_cast<int>(rooms.size()) < config.minRooms)
        {
            return "too few rooms: " + std::to_string(rooms.size());
        }

        int starts = 0;
        int exits = 0;
        int treasures = 0;

        for (const Room& room : rooms)
        {
            if (room.type == RoomType::Start)
            {
                ++starts;
            }
            else if (room.type == RoomType::Exit)
            {
                ++exits;
            }
            else if (room.type == RoomType::Treasure)
            {
                ++treasures;
            }

            const int left = room.bounds.position.x;
            const int top = room.bounds.position.y;
            const int right = left + room.bounds.size.x;
            const int bottom = top + room.bounds.size.y;

            if (left < 1 || top < 1 || right > tiles.width() - 1 || bottom > tiles.height() - 1)
            {
                return "room touches the map border";
            }

            for (int y = top; y < bottom; ++y)
            {
                for (int x = left; x < right; ++x)
                {
                    if (!tiles.isWalkable(x, y))
                    {
                        return "room contains a non-floor tile";
                    }
                }
            }
        }

        if (starts != 1)
        {
            return "expected 1 start room, found " + std::to_string(starts);
        }
        if (exits != 1)
        {
            return "expected 1 exit room, found " + std::to_string(exits);
        }
        if (treasures > config.maxTreasureRooms)
        {
            return "too many treasure rooms: " + std::to_string(treasures);
        }

        for (std::size_t i = 0; i < rooms.size(); ++i)
        {
            for (std::size_t j = i + 1; j < rooms.size(); ++j)
            {
                if (overlaps(rooms[i], rooms[j]))
                {
                    return "rooms " + std::to_string(i) + " and " + std::to_string(j) + " overlap";
                }
            }
        }

        for (int x = 0; x < tiles.width(); ++x)
        {
            if (tiles.isWalkable(x, 0) || tiles.isWalkable(x, tiles.height() - 1))
            {
                return "floor tile on the top or bottom border";
            }
        }
        for (int y = 0; y < tiles.height(); ++y)
        {
            if (tiles.isWalkable(0, y) || tiles.isWalkable(tiles.width() - 1, y))
            {
                return "floor tile on the left or right border";
            }
        }

        const Room* start = dungeon.findRoom(RoomType::Start);
        const int reachable = reachableFloorTiles(tiles, start->center());
        const int total = countFloorTiles(tiles);
        if (reachable != total)
        {
            return "disconnected: reached " + std::to_string(reachable) + " of " + std::to_string(total) +
                   " floor tiles";
        }

        return {};
    }

    struct TestCase
    {
        const char* name;
        GeneratorConfig config;
    };
}

int runGeneratorSelfTest(int seedCount)
{
    if (seedCount <= 0)
    {
        std::cerr << "Seed count must be positive\n";
        return 1;
    }

    GeneratorConfig large;
    large.width = 96;
    large.height = 64;

    const TestCase cases[] = {
        {"default 64x48", GeneratorConfig{}},
        {"large 96x64", large},
    };

    int totalFailures = 0;

    for (const TestCase& testCase : cases)
    {
        const DungeonGenerator generator(testCase.config);

        int passed = 0;
        int failed = 0;
        long long roomSum = 0;
        int roomMin = std::numeric_limits<int>::max();
        int roomMax = 0;

        const auto begin = std::chrono::steady_clock::now();

        for (int i = 0; i < seedCount; ++i)
        {
            const std::uint64_t seed = static_cast<std::uint64_t>(i);
            std::string error;

            try
            {
                const Dungeon first = generator.generate(seed);
                const Dungeon second = generator.generate(seed);

                if (!sameDungeon(first, second))
                {
                    error = "not deterministic: two runs with the same seed differ";
                }
                else
                {
                    error = validate(first, testCase.config);
                }

                if (error.empty())
                {
                    const int rooms = static_cast<int>(first.rooms().size());
                    roomSum += rooms;
                    roomMin = std::min(roomMin, rooms);
                    roomMax = std::max(roomMax, rooms);
                }
            }
            catch (const std::exception& exception)
            {
                error = std::string("exception: ") + exception.what();
            }

            if (error.empty())
            {
                ++passed;
            }
            else
            {
                ++failed;
                if (failed <= kMaxPrintedFailures)
                {
                    std::cout << "  FAIL seed " << seed << ": " << error << '\n';
                }
            }
        }

        const auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin);

        std::cout << "[" << testCase.name << "] " << passed << "/" << seedCount << " passed";
        if (passed > 0)
        {
            std::cout << " | rooms min " << roomMin << " avg " << std::fixed << std::setprecision(1)
                      << (static_cast<double>(roomSum) / static_cast<double>(passed)) << " max " << roomMax;
        }
        std::cout << " | " << elapsed.count() << " ms\n";

        if (failed > kMaxPrintedFailures)
        {
            std::cout << "  ... and " << (failed - kMaxPrintedFailures) << " more failures\n";
        }

        totalFailures += failed;
    }

    std::cout << (totalFailures == 0 ? "ALL TESTS PASSED\n" : "TESTS FAILED\n");
    return totalFailures == 0 ? 0 : 1;
}