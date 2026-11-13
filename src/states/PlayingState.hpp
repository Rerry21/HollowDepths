#pragma once

#include <SFML/Graphics.hpp>

#include "core/State.hpp"
#include "world/Player.hpp"

class PlayingState : public State
{
public:
    explicit PlayingState(Context& context);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderTarget& target) override;

private:
    void clampPlayerToWindow();

    Player m_player;
    sf::RectangleShape m_playerShape;
};