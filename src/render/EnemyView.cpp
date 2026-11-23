#include "render/EnemyView.hpp"
#include "render/RarityColor.hpp"

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
        inner.setFillColor(projectile.faction() == Faction::Player ? sf::Color(90, 220, 255) : sf::Color(255, 220, 90));
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

void EnemyView::drawLoot(sf::RenderTarget& target, const std::vector<LootDrop>& drop) const
{
    sf::CircleShape potion(6.f);
    potion.setOrigin({6.f, 6.f});

    sf::RectangleShape weapon({6.f, 16.f});
    weapon.setOrigin({3.f, 8.f});

    sf::RectangleShape armour({12.f, 12.f});
    armour.setOrigin({6.f, 6.f});

    for (const LootDrop& drop : drop)
    {
        const sf::Vector2f position{std::round(drop.position.x), std::round(drop.position.y)};
        const sf::Color color = rarityColor(drop.item.rarity);
        const sf::Color outline(10, 10, 16);

        if (drop.item.type == ItemType::HealthPotion)
        {
            potion.setPosition(position);
            potion.setFillColor(color);
            potion.setOutlineThickness(2.f);
            potion.setOutlineColor(outline);
            target.draw(potion);
        }
        else if (drop.item.type == ItemType::HealthPotion)
        {
            weapon.setPosition(position);
            potion.setFillColor(color);
            potion.setOutlineThickness(2.f);
            target.draw(potion);
        }
        else if (drop.item.type == ItemType::Weapon)
        {
            weapon.setPosition(position);
            weapon.setFillColor(color);
            weapon.setOutlineThickness(2.f);
            weapon.setOutlineColor(outline);
            target.draw(weapon);
        }
        else
        {
            armour.setPosition(position);
            armour.setFillColor(color);
            armour.setOutlineThickness(2.f);
            armour.setOutlineColor(outline);
            target.draw(armour);
        }
    }
}