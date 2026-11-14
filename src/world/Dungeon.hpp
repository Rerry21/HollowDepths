#pragma once

#include <cstdint>
#include <vector>

#include "world/Room.hpp"
#include "world/TileMap.hpp"

class Dungeon
{
public:
    Dungeon(TileMap tiles, std::vector<Room> rooms, std::uint64_t seed);

    const TileMap& tiles() const;
    const std::vector<Room>& rooms() const;
    std::uint64_t seed() const;
    const Room* findRoom(RoomType type) const;

private:
    TileMap m_tiles;
    std::vector<Room> m_rooms;
    std::uint64_t m_seed;
};