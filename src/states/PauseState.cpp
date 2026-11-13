#include "states/PauseState.hpp"

#include "core/StateStack.hpp"

PauseState::PauseState(Context& context)
    : State(context)
{
}

void PauseState::handleEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            context().stack.pop();
        }
        else if (key->code == sf::Keyboard::Key::Q)
        {
            context().window.close();
        }
    }
}

void PauseState::update(sf::Time)
{
}

void PauseState::render(sf::RenderTarget& target)
{
    const sf::Vector2f size(target.getSize());

    sf::RectangleShape overlay(size);
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    target.draw(overlay);

    const sf::Vector2f centre = size / 2.f;

    sf::RectangleShape bar({18.f, 64.f});
    bar.setFillColor(sf::Color(230, 230, 230));

    bar.setPosition({centre.x - 30.f, centre.y - 32.f});
    target.draw(bar);

    bar.setPosition({centre.x + 12.f, centre.y - 32.f});
    target.draw(bar);
}