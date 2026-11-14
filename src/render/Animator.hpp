#pragma once

#include <cstddef>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

struct AnimationClip
{
    std::vector<sf::IntRect> frames;
    float frameDuration = 0.1f;
};

class Animator
{
public:
    void addClip(AnimationClip clip);
    void play(std::size_t clip);
    void update(sf::Time dt);

    sf::IntRect currentFrame() const;

private:
    std::vector<AnimationClip> m_clips;
    std::size_t m_current = 0;
    std::size_t m_frame = 0;
    float m_elapsed = 0.f;
};