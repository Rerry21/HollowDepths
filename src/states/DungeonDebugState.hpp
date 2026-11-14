#pragma once

#include <cstdint>

#include <SFML/Graphics.hpp>

#include "core/State.hpp"
#include "generation/DungeonGenerator.hpp"
#include "render/DungeonView.hpp"
#include "world/Dungeon.hpp"

class DungeonDebugState : public State
{
public:
    explicit DungeonDebugState(Context& context);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderTarget& target) override;

private:
    void loadSeed(std::uint64_t seed);
    void rebuildView();
    void updateTitle();

    DungeonGenerator m_generator;
    std::uint64_t m_seed;
    Dungeon m_dungeon;
    DungeonView m_view;
};