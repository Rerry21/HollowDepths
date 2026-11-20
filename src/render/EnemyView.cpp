#include "render/EnemyView.hpp"

#include <cmath>

namespace
{
    constexpr float kBarWidth = 28.f;
    constexpr float kBarHeight = 4.f;
    constexpr float kBarGap = 8.f;
    constexpr float kDotSize = 6.f;
    constexpr float kDotGap = 16.f;

    sf::Color baseColor(EnemyType type)
    {
        switch (type)
        {
        case EnemyType::Grunt:
            return sf::Color(150, 70, 190);
        case EnemyType::Runner:
            return sf::Color(240, 90, 220);
        case EnemyType::Heavy:
            return sf::Color(35, 25, 45);
        case EnemyType::Archer:
            return sf::Color(150, 230, 60);
        }
        return sf::Color::White;
    }

    sf::Color stateColor(AIState state)
    {
        switch (state)
        {
        case AIState::Idle:
            return sf::Color(160, 160, 160);
        case AIState::Patrol:
            return sf::Color(80, 140, 255);
        case AIState::Chase:
            return sf::Color(255, 160, 40);
        case AIState::Attack:
            return sf::Color(255, 50, 50);
        case AIState::Retreat:
            return sf::Color(60, 220, 100);
        }
        return sf::Color::White;
    }

    sf::Color bodyColor(const Enemy& enemy)
    {
        if (enemy.isWindingUp() || enemy.isHurtFlashing())
        {
            return sf::Color::White;
        }
        return baseColor(enemy.type());
    }
}

void EnemyView::draw(sf::RenderTarget& target, const std::vector<Enemy>& enemies) const
{
    sf::RectangleShape body;
    sf::RectangleShape bar;
    sf::RectangleShape dot({kDotSize, kDotSize});

    for (const Enemy& enemy : enemies)
    {
        const sf::Vector2f size = enemy.size();
        const sf::Vector2f position{std::round(enemy.position().x), std::round(enemy.position().y)};

        body.setSize(size);
        body.setOrigin(size / 2.f);
        body.setPosition(position);
        body.setFillColor(bodyColor(enemy));
        target.draw(body);

        const float healthFraction = enemy.health().fraction();
        if (healthFraction < 1.f)
        {
            const sf::Vector2f barPosition{position.x - kBarWidth / 2.f, position.y - size.y / 2.f - kBarGap};

            bar.setSize({kBarWidth, kBarHeight});
            bar.setPosition(barPosition);
            bar.setFillColor(sf::Color(20, 20, 20, 220));
            target.draw(bar);

            bar.setSize({kBarWidth * healthFraction, kBarHeight});
            bar.setFillColor(sf::Color(220, 60, 60));
            target.draw(bar);
        }

        if (m_showDebug)
        {
            dot.setPosition({position.x - kDotSize / 2.f, position.y - size.y / 2.f - kDotGap});
            dot.setFillColor(stateColor(enemy.aiState()));
            target.draw(dot);
        }
    }
}

void EnemyView::drawProjectiles(sf::RenderTarget& target, const std::vector<Projectile>& projectiles) const
{
    sf::RectangleShape outer({10.f, 10.f});
    outer.setOrigin({5.f, 5.f});
    outer.setFillColor(sf::Color(20, 20, 30));

    sf::RectangleShape inner({6.f, 6.f});
    inner.setOrigin({3.f, 3.f});
    inner.setFillColor(sf::Color(255, 220, 90));

    for (const Projectile& projectile : projectiles)
    {
        const sf::Vector2f position{std::round(projectile.position().x), std::round(projectile.position().y)};
        outer.setPosition(position);
        inner.setPosition(position);
        target.draw(outer);
        target.draw(inner);
    }
}

void EnemyView::toggleDebug()
{
    m_showDebug = !m_showDebug;
}