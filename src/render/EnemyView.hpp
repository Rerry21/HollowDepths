#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "world/Enemy.hpp"
#include "world/Projectile.hpp"

class EnemyView
{
public:
    void draw(sf::RenderTarget& target, const std::vector<Enemy>& enemies) const;
    void drawProjectiles(sf::RenderTarget& target, const std::vector<Projectile>& projectiles) const;
    void toggleDebug();

private:
    bool m_showDebug = false;
};