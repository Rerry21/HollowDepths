#include "states/PlayingState.hpp"

#include <algorithm>

#include "core/Input.hpp"
#include "core/StateStack.hpp"
#include "states/PauseState.hpp"

PlayingState::PlayingState(Context& context)
    : State(context)
    , m_player(sf::Vector2f(context.window.getSize()) / 2.f)
    , m_playerShape(m_player.size())
{
    m_playerShape.setOrigin(m_player.size() / 2.f);
    m_playerShape.setFillColor(sf::Color(90, 190, 120));
}

void PlayingState::handleEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            context().stack.push<PauseState>(context());
        }
    }
    else if (event.is<sf::Event::FocusLost>())
    {
        context().stack.push<PauseState>(context());
    }
}

void PlayingState::update(sf::Time dt)
{
    Context& ctx = context();

    const sf::Vector2f direction = ctx.window.hasFocus() ? ctx.input.movementAxis() : sf::Vector2f{};
    m_player.setMoveDirection(direction);
    m_player.update(dt);

    clampPlayerToWindow();
}

void PlayingState::render(sf::RenderTarget& target)
{
    m_playerShape.setPosition(m_player.position());
    target.draw(m_playerShape);
}

void PlayingState::clampPlayerToWindow()
{
    const sf::Vector2f half = m_player.size() / 2.f;
    const sf::Vector2f limit = sf::Vector2f(context().window.getSize()) - half;

    sf::Vector2f position = m_player.position();
    position.x = std::clamp(position.x, half.x, limit.x);
    position.y = std::clamp(position.y, half.y, limit.y);
    m_player.setPosition(position);
}