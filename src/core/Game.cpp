#include "core/Game.hpp"

#include <optional>

namespace
{
    constexpr unsigned int kWindowWidth = 1280;
    constexpr unsigned int kWindowHeight = 720;
    const sf::Time kTimeStep = sf::seconds(1.f / 60.f);
    const sf::Time kMaxFrameTime = sf::seconds(0.25f);
}

Game::Game()
    : m_window(sf::VideoMode({kWindowWidth, kWindowHeight}), "Hollow Depths")
    , m_testSquare({100.f, 100.f})
{
    m_window.setVerticalSyncEnabled(true);

    m_testSquare.setFillColor(sf::Color(200, 80, 60));
    m_testSquare.setOrigin(m_testSquare.getSize() / 2.f);
    m_testSquare.setPosition({kWindowWidth / 2.f, kWindowHeight / 2.f});
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
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Escape)
            {
                m_window.close();
            }
        }
    }
}

void Game::update(sf::Time dt)
{
    m_testSquare.rotate(sf::degrees(90.f * dt.asSeconds()));
}

void Game::render()
{
    m_window.clear(sf::Color(20, 20, 28));
    m_window.draw(m_testSquare);
    m_window.display();
}