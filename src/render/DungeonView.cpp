#include "render/DungeonView.hpp"

namespace
{
    const sf::Color kWallColor(30, 32, 44);
    const sf::Color kCorridorColor(78, 82, 98);

    sf::Color roomColor(RoomType type)
    {
        switch (type)
        {
        case RoomType::Start:
            return sf::Color(70, 170, 100);
        case RoomType::Exit:
            return sf::Color(200, 80, 70);
        case RoomType::Treasure:
            return sf::Color(210, 180, 60);
        case RoomType::Normal:
            break;
        }
        return sf::Color(112, 118, 142);
    }

    void addQuad(sf::VertexArray& vertices, sf::Vector2f position, sf::Vector2f size, sf::Color color)
    {
        const sf::Vector2f topLeft = position;
        const sf::Vector2f topRight{position.x + size.x, position.y};
        const sf::Vector2f bottomLeft{position.x, position.y + size.y};
        const sf::Vector2f bottomRight{position.x + size.x, position.y + size.y};
        const sf::Vector2f none{0.f, 0.f};

        vertices.append(sf::Vertex{topLeft, color, none});
        vertices.append(sf::Vertex{topRight, color, none});
        vertices.append(sf::Vertex{bottomLeft, color, none});
        vertices.append(sf::Vertex{bottomLeft, color, none});
        vertices.append(sf::Vertex{topRight, color, none});
        vertices.append(sf::Vertex{bottomRight, color, none});
    }
}

DungeonView::DungeonView()
    : m_vertices(sf::PrimitiveType::Triangles)
{
}

void DungeonView::rebuild(const Dungeon& dungeon, float tileSize, sf::Vector2f origin)
{
    m_vertices.clear();

    const TileMap& tiles = dungeon.tiles();
    for (int y = 0; y < tiles.height(); ++y)
    {
        for (int x = 0; x < tiles.width(); ++x)
        {
            const sf::Color color = tiles.isWalkable(x, y) ? kCorridorColor : kWallColor;
            const sf::Vector2f position{origin.x + static_cast<float>(x) * tileSize,
                                        origin.y + static_cast<float>(y) * tileSize};
            addQuad(m_vertices, position, {tileSize, tileSize}, color);
        }
    }

    for (const Room& room : dungeon.rooms())
    {
        const sf::Vector2f position{origin.x + static_cast<float>(room.bounds.position.x) * tileSize,
                                    origin.y + static_cast<float>(room.bounds.position.y) * tileSize};
        const sf::Vector2f size{static_cast<float>(room.bounds.size.x) * tileSize,
                                static_cast<float>(room.bounds.size.y) * tileSize};
        addQuad(m_vertices, position, size, roomColor(room.type));
    }
}

void DungeonView::draw(sf::RenderTarget& target) const
{
    target.draw(m_vertices);
}