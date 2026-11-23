#include "states/PlayingState.hpp"

#include <string>

#include "core/Input.hpp"
#include "core/StateStack.hpp"
#include "generation/Rng.hpp"
#include "states/PauseState.hpp"
#include "render/RarityColor.hpp"

namespace
{
    constexpr int kDebugDamage = 15;
    constexpr int kDebugHeal = 25;
    constexpr int kDebugEnemyDamage = 25;

    constexpr float kBarWidth = 240.f;
    constexpr float kBarHeight = 18.f;
    constexpr float kMargin = 16.f;
    constexpr float kBorder = 2.f;
}

PlayingState::PlayingState(Context& context)
    : State(context)
    , m_generator()
    , m_seed(randomSeed())
    , m_world(m_generator.generate(m_seed))
    , m_camera(sf::Vector2f(context.window.getSize()))
{
    enterWorld();
}

void PlayingState::handleEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        switch (key->code)
        {
        case sf::Keyboard::Key::Escape:
            context().stack.push<PauseState>(context());
            break;
        case sf::Keyboard::Key::R:
            startNewDungeon(randomSeed());
            break;
        case sf::Keyboard::Key::K:
            m_world.player().takeDamage(kDebugDamage);
            break;
        case sf::Keyboard::Key::H:
            m_world.player().heal(kDebugHeal);
            break;
        case sf::Keyboard::Key::X:
            m_world.damageNearestEnemy(kDebugEnemyDamage);
            break;
        case sf::Keyboard::Key::F3:
            m_enemyView.toggleDebug();
            break;
        default:
            break;
        }
    }
    else if (const auto* mouse = event.getIf<sf::Event::MouseButtonPressed>())
    {
        const sf::Vector2f clicked = context().window.mapPixelToCoords(mouse->position, m_camera.view());
        const sf::Vector2f aim = clicked - m_world.player().position();

        if (mouse->button == sf::Mouse::Button::Left)
        {
            m_world.playerMeleeAttack(aim);
        }
        else if (mouse->button == sf::Mouse::Button::Right)
        {
            m_world.playerShoot(aim);
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
    m_playerView.update(m_world.player(), dt);
    m_camera.follow(m_world.player().position(), dt);
}

void PlayingState::render(sf::RenderTarget& target)
{
    target.setView(m_camera.view());

    m_dungeonView.draw(target);
    m_enemyView.drawLoot(target, m_world.drops());
    m_enemyView.draw(target, m_world.enemies());
    m_enemyView.drawProjectiles(target, m_world.projectiles());
    m_playerView.drawSwing(target, m_world.swing());
    m_playerView.draw(target, m_world.player());

    target.setView(target.getDefaultView());

    renderHud(target);

    const auto drawSlot = [&](float x, const std::optional<Item>& item) {
        sf::RectangleShape slot({16.f, 16.f});
        slot.setPosition({x, kMargin + kBarHeight + 2.f * kBorder + 8.f});
        slot.setFillColor(item ? rarityColor(item->rarity) : sf::Color(40, 40, 50));
        slot.setOutlineThickness(2.f);
        slot.setOutlineColor(sf::Color(10, 10, 16, 220));
        target.draw(slot);
    };
    drawSlot(kMargin + 2.f, m_world.player().weapon());
    drawSlot(kMargin + 28.f, m_world.player().armour());
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

    m_camera.setBounds(sf::FloatRect({0.f, 0.f},
                                     {static_cast<float>(tiles.width()) * kTileSize,
                                      static_cast<float>(tiles.height()) * kTileSize}));
    m_camera.snapTo(m_world.player().position());

    updateTitle();
}

void PlayingState::updateTitle()
{
    context().window.setTitle("Hollow Depths | seed " + std::to_string(m_seed) +
                              " | WASD move  LMB melee  RMB shoot  R new  K hurt me  H heal  Esc pause");
}

void PlayingState::renderHud(sf::RenderTarget& target)
{
    sf::RectangleShape frame({kBarWidth + 2.f * kBorder, kBarHeight + 2.f * kBorder});
    frame.setPosition({kMargin, kMargin});
    frame.setFillColor(sf::Color(10, 10, 16, 220));
    target.draw(frame);

    const float fraction = m_world.player().health().fraction();
    sf::RectangleShape fill({kBarWidth * fraction, kBarHeight});
    fill.setPosition({kMargin + kBorder, kMargin + kBorder});
    fill.setFillColor(sf::Color(200, 60, 60));
    target.draw(fill);
}