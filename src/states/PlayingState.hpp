#pragma once

#include <cstdint>

#include <SFML/Graphics.hpp>

#include "core/State.hpp"
#include "generation/DungeonGenerator.hpp"
#include "render/Camera.hpp"
#include "render/DungeonView.hpp"
#include "world/World.hpp"

class PlayingState : public State
{
public:
    explicit PlayingState(Context& context);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderTarget& target) override;

private:
    void startNewDungeon(std::uint64_t seed);
    void enterWorld();
    void updateTitle();

    DungeonGenerator m_generator;
    std::uint64_t m_seed;
    World m_world;
    DungeonView m_dungeonView;
    Camera m_camera;
    sf::RectangleShape m_playerShape;
};