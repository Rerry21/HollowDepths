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

void EnemyView::toggleDebug()
{
    m_showDebug = !m_showDebug;
}