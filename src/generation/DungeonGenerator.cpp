#include "generation/DungeonGenerator.hpp"

#include <algorithm>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "generation/Rng.hpp"

namespace
{
    struct BspNode
    {
        sf::IntRect area;
        int left = -1;
        int right = -1;
        int depth = 0;
        int roomIndex = -1;
    };

    struct Edge
    {
        int a;
        int b;
    };

    std::vector<int> distanceField(const TileMap& tiles, sf::Vector2i start)
    {
        const int width = tiles.width();
        const int height = tiles.height();
        std::vector<int> distance(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), -1);

        if (!tiles.isWalkable(start.x, start.y))
        {
            return distance;
        }

        constexpr int kDx[4] = {1, -1, 0, 0};
        constexpr int kDy[4] = {0, 0, 1, -1};

        std::queue<sf::Vector2i> frontier;
        distance[static_cast<std::size_t>(start.y * width + start.x)] = 0;
        frontier.push(start);

        while (!frontier.empty())
        {
            const sf::Vector2i current = frontier.front();
            frontier.pop();
            const int currentDistance = distance[static_cast<std::size_t>(current.y * width + current.x)];

            for (int i = 0; i < 4; ++i)
            {
                const int nx = current.x + kDx[i];
                const int ny = current.y + kDy[i];
                if (!tiles.isWalkable(nx, ny))
                {
                    continue;
                }

                const std::size_t index = static_cast<std::size_t>(ny * width + nx);
                if (distance[index] != -1)
                {
                    continue;
                }

                distance[index] = currentDistance + 1;
                frontier.push({nx, ny});
            }
        }

        return distance;
    }

    class Builder
    {
    public:
        Builder(const GeneratorConfig& config, std::uint64_t attemptSeed)
            : m_config(config)
            , m_rng(attemptSeed)
            , m_tiles(config.width, config.height)
        {
        }

        std::optional<Dungeon> run(std::uint64_t reportedSeed)
        {
            m_tiles.fill(Tile::Wall);

            addNode(sf::IntRect({0, 0}, {m_config.width, m_config.height}), 0);
            splitNode(0);
            createRooms();
            connectNode(0);
            addLoops();

            if (static_cast<int>(m_rooms.size()) < m_config.minRooms)
            {
                return std::nullopt;
            }

            if (!assignRoomTypes())
            {
                return std::nullopt;
            }

            return Dungeon(std::move(m_tiles), std::move(m_rooms), reportedSeed);
        }

    private:
        int addNode(const sf::IntRect& area, int depth)
        {
            BspNode node;
            node.area = area;
            node.depth = depth;
            m_nodes.push_back(node);
            return static_cast<int>(m_nodes.size()) - 1;
        }

        void splitNode(int index)
        {
            const sf::IntRect area = m_nodes[static_cast<std::size_t>(index)].area;
            const int depth = m_nodes[static_cast<std::size_t>(index)].depth;

            const bool canSplitX = area.size.x >= 2 * m_config.minLeafSize;
            const bool canSplitY = area.size.y >= 2 * m_config.minLeafSize;
            if (!canSplitX && !canSplitY)
            {
                return;
            }

            if (depth >= 2 && m_rng.chance(20))
            {
                return;
            }

            bool splitAlongX;
            if (!canSplitY)
            {
                splitAlongX = true;
            }
            else if (!canSplitX)
            {
                splitAlongX = false;
            }
            else if (area.size.x * 4 > area.size.y * 5)
            {
                splitAlongX = true;
            }
            else if (area.size.y * 4 > area.size.x * 5)
            {
                splitAlongX = false;
            }
            else
            {
                splitAlongX = m_rng.chance(50);
            }

            sf::IntRect first;
            sf::IntRect second;

            if (splitAlongX)
            {
                const int cut = m_rng.range(m_config.minLeafSize, area.size.x - m_config.minLeafSize);
                first = sf::IntRect(area.position, {cut, area.size.y});
                second = sf::IntRect({area.position.x + cut, area.position.y}, {area.size.x - cut, area.size.y});
            }
            else
            {
                const int cut = m_rng.range(m_config.minLeafSize, area.size.y - m_config.minLeafSize);
                first = sf::IntRect(area.position, {area.size.x, cut});
                second = sf::IntRect({area.position.x, area.position.y + cut}, {area.size.x, area.size.y - cut});
            }

            const int leftIndex = addNode(first, depth + 1);
            const int rightIndex = addNode(second, depth + 1);
            m_nodes[static_cast<std::size_t>(index)].left = leftIndex;
            m_nodes[static_cast<std::size_t>(index)].right = rightIndex;

            splitNode(leftIndex);
            splitNode(rightIndex);
        }

        void createRooms()
        {
            for (std::size_t i = 0; i < m_nodes.size(); ++i)
            {
                if (m_nodes[i].left != -1)
                {
                    continue;
                }

                const sf::IntRect area = m_nodes[i].area;
                const int margin = m_config.roomMargin;
                const int availableWidth = area.size.x - 2 * margin;
                const int availableHeight = area.size.y - 2 * margin;

                if (availableWidth < m_config.minRoomSize || availableHeight < m_config.minRoomSize)
                {
                    continue;
                }

                const int width = m_rng.range(m_config.minRoomSize, std::min(m_config.maxRoomSize, availableWidth));
                const int height = m_rng.range(m_config.minRoomSize, std::min(m_config.maxRoomSize, availableHeight));
                const int x = area.position.x + margin + m_rng.range(0, availableWidth - width);
                const int y = area.position.y + margin + m_rng.range(0, availableHeight - height);

                for (int ty = y; ty < y + height; ++ty)
                {
                    for (int tx = x; tx < x + width; ++tx)
                    {
                        m_tiles.set(tx, ty, Tile::Floor);
                    }
                }

                m_nodes[i].roomIndex = static_cast<int>(m_rooms.size());
                m_rooms.push_back(Room{sf::IntRect({x, y}, {width, height}), RoomType::Normal});
            }
        }

        int pickRoom(int nodeIndex)
        {
            const int left = m_nodes[static_cast<std::size_t>(nodeIndex)].left;
            const int right = m_nodes[static_cast<std::size_t>(nodeIndex)].right;

            if (left == -1)
            {
                return m_nodes[static_cast<std::size_t>(nodeIndex)].roomIndex;
            }

            return pickRoom(m_rng.chance(50) ? left : right);
        }

        void connectNode(int nodeIndex)
        {
            const int left = m_nodes[static_cast<std::size_t>(nodeIndex)].left;
            const int right = m_nodes[static_cast<std::size_t>(nodeIndex)].right;

            if (left == -1)
            {
                return;
            }

            connectNode(left);
            connectNode(right);

            const int roomA = pickRoom(left);
            const int roomB = pickRoom(right);
            if (roomA >= 0 && roomB >= 0)
            {
                connectRooms(roomA, roomB);
            }
        }

        void connectRooms(int roomA, int roomB)
        {
            const sf::Vector2i from = m_rooms[static_cast<std::size_t>(roomA)].center();
            const sf::Vector2i to = m_rooms[static_cast<std::size_t>(roomB)].center();

            if (m_rng.chance(50))
            {
                carveHorizontal(from.x, to.x, from.y);
                carveVertical(from.y, to.y, to.x);
            }
            else
            {
                carveVertical(from.y, to.y, from.x);
                carveHorizontal(from.x, to.x, to.y);
            }

            m_edges.push_back({roomA, roomB});
        }

        void carveHorizontal(int x0, int x1, int y)
        {
            for (int x = std::min(x0, x1); x <= std::max(x0, x1); ++x)
            {
                m_tiles.set(x, y, Tile::Floor);
            }
        }

        void carveVertical(int y0, int y1, int x)
        {
            for (int y = std::min(y0, y1); y <= std::max(y0, y1); ++y)
            {
                m_tiles.set(x, y, Tile::Floor);
            }
        }

        bool areConnected(int roomA, int roomB) const
        {
            return std::any_of(m_edges.begin(), m_edges.end(), [&](const Edge& edge) {
                return (edge.a == roomA && edge.b == roomB) || (edge.a == roomB && edge.b == roomA);
            });
        }

        int degree(int room) const
        {
            return static_cast<int>(std::count_if(m_edges.begin(), m_edges.end(), [&](const Edge& edge) {
                return edge.a == room || edge.b == room;
            }));
        }

        int squaredDistance(int roomA, int roomB) const
        {
            const sf::Vector2i a = m_rooms[static_cast<std::size_t>(roomA)].center();
            const sf::Vector2i b = m_rooms[static_cast<std::size_t>(roomB)].center();
            const int dx = a.x - b.x;
            const int dy = a.y - b.y;
            return dx * dx + dy * dy;
        }

        void addLoops()
        {
            const int count = static_cast<int>(m_rooms.size());
            if (count < 3)
            {
                return;
            }

            for (int i = 0; i < m_config.extraLoops; ++i)
            {
                const int roomA = m_rng.range(0, count - 1);

                std::vector<int> others;
                for (int j = 0; j < count; ++j)
                {
                    if (j != roomA)
                    {
                        others.push_back(j);
                    }
                }

                std::sort(others.begin(), others.end(), [&](int lhs, int rhs) {
                    const int distanceL = squaredDistance(roomA, lhs);
                    const int distanceR = squaredDistance(roomA, rhs);
                    if (distanceL != distanceR)
                    {
                        return distanceL < distanceR;
                    }
                    return lhs < rhs;
                });

                const int maxRank = std::min(2, static_cast<int>(others.size()) - 1);
                const int rank = maxRank >= 1 ? m_rng.range(1, maxRank) : 0;
                const int roomB = others[static_cast<std::size_t>(rank)];

                if (!areConnected(roomA, roomB))
                {
                    connectRooms(roomA, roomB);
                }
            }
        }

        bool assignRoomTypes()
        {
            const int count = static_cast<int>(m_rooms.size());
            const int startIndex = m_rng.range(0, count - 1);
            m_rooms[static_cast<std::size_t>(startIndex)].type = RoomType::Start;

            const sf::Vector2i startCentre = m_rooms[static_cast<std::size_t>(startIndex)].center();
            const std::vector<int> distance = distanceField(m_tiles, startCentre);

            if (!isFullyConnected(distance))
            {
                return false;
            }

            int exitIndex = -1;
            int bestDistance = -1;
            for (int i = 0; i < count; ++i)
            {
                if (i == startIndex)
                {
                    continue;
                }

                const sf::Vector2i centre = m_rooms[static_cast<std::size_t>(i)].center();
                const int roomDistance = distance[static_cast<std::size_t>(centre.y * m_config.width + centre.x)];
                if (roomDistance > bestDistance)
                {
                    bestDistance = roomDistance;
                    exitIndex = i;
                }
            }

            if (exitIndex < 0)
            {
                return false;
            }

            m_rooms[static_cast<std::size_t>(exitIndex)].type = RoomType::Exit;

            int treasureCount = 0;
            for (int i = 0; i < count && treasureCount < m_config.maxTreasureRooms; ++i)
            {
                Room& room = m_rooms[static_cast<std::size_t>(i)];
                if (room.type == RoomType::Normal && degree(i) == 1)
                {
                    room.type = RoomType::Treasure;
                    ++treasureCount;
                }
            }

            return true;
        }

        bool isFullyConnected(const std::vector<int>& distance) const
        {
            for (int y = 0; y < m_config.height; ++y)
            {
                for (int x = 0; x < m_config.width; ++x)
                {
                    if (m_tiles.isWalkable(x, y) && distance[static_cast<std::size_t>(y * m_config.width + x)] < 0)
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        GeneratorConfig m_config;
        Rng m_rng;
        TileMap m_tiles;
        std::vector<BspNode> m_nodes;
        std::vector<Room> m_rooms;
        std::vector<Edge> m_edges;
    };
}

DungeonGenerator::DungeonGenerator(GeneratorConfig config)
    : m_config(config)
{
}

Dungeon DungeonGenerator::generate(std::uint64_t seed) const
{
    for (int attempt = 0; attempt < m_config.maxAttempts; ++attempt)
    {
        const std::uint64_t attemptSeed = (attempt == 0) ? seed : Rng::deriveSeed(seed, attempt);

        Builder builder(m_config, attemptSeed);
        std::optional<Dungeon> dungeon = builder.run(seed);
        if (dungeon)
        {
            return std::move(*dungeon);
        }
    }

    throw std::runtime_error("Dungeon generation failed for seed " + std::to_string(seed));
}