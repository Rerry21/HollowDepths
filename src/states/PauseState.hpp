#pragma once

#include <SFML/Graphics.hpp>

#include "core/State.hpp"

class PauseState : public State
{
public:
    explicit PauseState(Context& context);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderTarget& target) override;
};