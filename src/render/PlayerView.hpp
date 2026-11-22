#pragma once

#include <SFML/Graphics.hpp>

#include "render/Animator.hpp"
#include "world/Player.hpp"
#include "world/Swing.hpp"

class PlayerView
{
public:
    PlayerView();

    PlayerView(const PlayerView&) = delete;
    PlayerView& operator=(const PlayerView&) = delete;

    void update(const Player& player, sf::Time dt);
    void draw(sf::RenderTarget& target, const Player& player);
    void drawSwing(sf::RenderTarget& target, const Swing& swing) const;

private:
    sf::Texture m_texture;
    sf::Sprite m_sprite;
    Animator m_animator;
};