#include "states/PlayingState.hpp"

#include <string>

#include "core/Input.hpp"
#include "core/StateStack.hpp"
#include "generation/Rng.hpp"
#include "states/PauseState.hpp"

PlayingState::PlayingState(Context& context)
    : State(context)
    , m_generator()
    , m_seed(randomSeed())
    , m_world(m_generator.generate(m_seed))
    , m_camera(sf::Vector2f(context.window.getSize()))
    , m_playerShape(m_world.player().size())
{
    m_playerShape.setFillColor(sf::Color(80, 200, 255));
    enterWorld();
}

void PlayingState::handleEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            context().stack.push<PauseState>(context());
        }
        else if (key->code == sf::Keyboard::Key::R)
        {
            startNewDungeon(randomSeed());
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
    m_world.update(dt, direction);
    m_camera.follow(m_world.player().position(), dt);
}

void PlayingState::render(sf::RenderTarget& target)
{
    target.setView(m_camera.view());

    m_dungeonView.draw(target);

    m_playerShape.setPosition(m_world.player().position());
    target.draw(m_playerShape);

    target.setView(target.getDefaultView());
}

void PlayingState::startNewDungeon(std::uint64_t seed)
{
    m_seed = seed;
    m_world = World(m_generator.generate(seed));
    enterWorld();
}

void PlayingState::enterWorld()
{
    const TileMap& tiles = m_world.dungeon().tiles();

    m_dungeonView.rebuild(m_world.dungeon(), kTileSize, {0.f, 0.f});

    m_playerShape.setSize(m_world.player().size());
    m_playerShape.setOrigin(m_world.player().size() / 2.f);

    m_camera.setBounds(sf::FloatRect({0.f, 0.f},
                                     {static_cast<float>(tiles.width()) * kTileSize,
                                      static_cast<float>(tiles.height()) * kTileSize}));
    m_camera.snapTo(m_world.player().position());

    updateTitle();
}

void PlayingState::updateTitle()
{
    context().window.setTitle("Hollow Depths | seed " + std::to_string(m_seed) +
                              " | WASD/arrows: move  R: new dungeon  Esc: pause");
}