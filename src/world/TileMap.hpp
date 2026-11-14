#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

enum class Tile : std::uint8_t
{
    Wall,
    Floor
};

class TileMap
{
public:
    TileMap(int width, int height);

    int width() const;
    int height() const;
    bool inBounds(int x, int y) const;

    Tile at(int x, int y) const;
    void set(int x, int y, Tile tile);
    void fill(Tile tile);
    bool isWalkable(int x, int y) const;

private:
    std::size_t index(int x, int y) const;

    int m_width;
    int m_height;
    std::vector<Tile> m_tiles;
};