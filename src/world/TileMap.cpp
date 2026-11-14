#include "world/TileMap.hpp"

#include <algorithm>
#include <stdexcept>

TileMap::TileMap(int width, int height)
    : m_width(width)
    , m_height(height)
{
    if (width <= 0 || height <= 0)
    {
        throw std::invalid_argument("TileMap dimensions must be positive");
    }

    m_tiles.assign(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), Tile::Wall);
}

int TileMap::width() const
{
    return m_width;
}

int TileMap::height() const
{
    return m_height;
}

bool TileMap::inBounds(int x, int y) const
{
    return x >= 0 && y >= 0 && x < m_width && y < m_height;
}

Tile TileMap::at(int x, int y) const
{
    if (!inBounds(x, y))
    {
        return Tile::Wall;
    }
    return m_tiles[index(x, y)];
}

void TileMap::set(int x, int y, Tile tile)
{
    if (inBounds(x, y))
    {
        m_tiles[index(x, y)] = tile;
    }
}

void TileMap::fill(Tile tile)
{
    std::fill(m_tiles.begin(), m_tiles.end(), tile);
}

bool TileMap::isWalkable(int x, int y) const
{
    return at(x, y) == Tile::Floor;
}

std::size_t TileMap::index(int x, int y) const
{
    return static_cast<std::size_t>(y) * static_cast<std::size_t>(m_width) + static_cast<std::size_t>(x);
}