#include "world/Dungeon.hpp"

#include <algorithm>
#include <utility>

Dungeon::Dungeon(TileMap tiles, std::vector<Room> rooms, std::uint64_t seed)
    : m_tiles(std::move(tiles))
    , m_rooms(std::move(rooms))
    , m_seed(seed)
{
}

const TileMap& Dungeon::tiles() const
{
    return m_tiles;
}

const std::vector<Room>& Dungeon::rooms() const
{
    return m_rooms;
}

std::uint64_t Dungeon::seed() const
{
    return m_seed;
}

const Room* Dungeon::findRoom(RoomType type) const
{
    const auto it = std::find_if(m_rooms.begin(), m_rooms.end(), [type](const Room& room) { return room.type == type; });
    return it == m_rooms.end() ? nullptr : &(*it);
}