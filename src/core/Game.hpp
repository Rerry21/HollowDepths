#pragma once

#include <SFML/Graphics.hpp>

#include "core/Input.hpp"
#include "core/State.hpp"
#include "core/StateStack.hpp"

class Game
{
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time dt);
    void render();

    sf::RenderWindow m_window;
    Input m_input;
    StateStack m_stateStack;
    Context m_context;
};