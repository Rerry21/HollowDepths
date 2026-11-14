#include "core/Game.hpp"

#include <optional>

#include "states/DungeonDebugState.hpp"

namespace
{
    constexpr unsigned int kWindowWidth = 1280;
    constexpr unsigned int kWindowHeight = 720;
    const sf::Time kTimeStep = sf::seconds(1.f / 60.f);
    const sf::Time kMaxFrameTime = sf::seconds(0.25f);
}

Game::Game()
    : m_window(sf::VideoMode({kWindowWidth, kWindowHeight}), "Hollow Depths")
    , m_input()
    , m_stateStack()
    , m_context{m_window, m_input, m_stateStack}
{
    m_window.setVerticalSyncEnabled(true);
    m_window.setKeyRepeatEnabled(false);

    m_stateStack.push<DungeonDebugState>(m_context);
}

void Game::run()
{
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;

    while (m_window.isOpen())
    {
        accumulator += clock.restart();
        if (accumulator > kMaxFrameTime)
        {
            accumulator = kMaxFrameTime;
        }

        processEvents();

        while (accumulator >= kTimeStep)
        {
            update(kTimeStep);
            accumulator -= kTimeStep;
        }

        render();
    }
}

void Game::processEvents()
{
    while (const std::optional event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
        }
        else
        {
            m_stateStack.handleEvent(*event);
        }
    }
}

void Game::update(sf::Time dt)
{
    m_stateStack.update(dt);

    if (m_stateStack.isEmpty())
    {
        m_window.close();
    }
}

void Game::render()
{
    m_window.clear(sf::Color(20, 20, 28));
    m_stateStack.render(m_window);
    m_window.display();
}