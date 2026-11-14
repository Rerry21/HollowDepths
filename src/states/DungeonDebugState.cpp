#include "states/DungeonDebugState.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <string>

#include "core/StateStack.hpp"

namespace
{
    std::uint64_t randomSeed()
    {
        std::random_device device;
        return (static_cast<std::uint64_t>(device()) << 32) | static_cast<std::uint64_t>(device());
    }
}

DungeonDebugState::DungeonDebugState(Context& context)
    : State(context)
    , m_seed(randomSeed())
    , m_dungeon(m_generator.generate(m_seed))
{
    rebuildView();
    updateTitle();
}

void DungeonDebugState::handleEvent(const sf::Event& event)
{
    const auto* key = event.getIf<sf::Event::KeyPressed>();
    if (key == nullptr)
    {
        return;
    }

    switch (key->code)
    {
    case sf::Keyboard::Key::Escape:
        context().stack.pop();
        break;
    case sf::Keyboard::Key::R:
        loadSeed(randomSeed());
        break;
    case sf::Keyboard::Key::Right:
        loadSeed(m_seed + 1);
        break;
    case sf::Keyboard::Key::Left:
        loadSeed(m_seed - 1);
        break;
    default:
        break;
    }
}

void DungeonDebugState::update(sf::Time)
{
}

void DungeonDebugState::render(sf::RenderTarget& target)
{
    m_view.draw(target);
}

void DungeonDebugState::loadSeed(std::uint64_t seed)
{
    m_seed = seed;
    m_dungeon = m_generator.generate(seed);
    rebuildView();
    updateTitle();
}

void DungeonDebugState::rebuildView()
{
    const sf::Vector2f windowSize(context().window.getSize());
    const TileMap& tiles = m_dungeon.tiles();

    const float tileSize = std::floor(std::min(windowSize.x / static_cast<float>(tiles.width()),
                                               windowSize.y / static_cast<float>(tiles.height())));
    const sf::Vector2f origin{std::floor((windowSize.x - tileSize * static_cast<float>(tiles.width())) / 2.f),
                              std::floor((windowSize.y - tileSize * static_cast<float>(tiles.height())) / 2.f)};

    m_view.rebuild(m_dungeon, tileSize, origin);
}

void DungeonDebugState::updateTitle()
{
    const std::string title = "Hollow Depths | seed " + std::to_string(m_seed) + " | rooms " +
                              std::to_string(m_dungeon.rooms().size()) +
                              " | R: random  Left/Right: seed -/+  Esc: quit";
    context().window.setTitle(title);
}