#include "render/Animator.hpp"

#include <utility>

void Animator::addClip(AnimationClip clip)
{
    m_clips.push_back(std::move(clip));
}

void Animator::play(std::size_t clip)
{
    if (clip == m_current || clip >= m_clips.size())
    {
        return;
    }

    m_current = clip;
    m_frame = 0;
    m_elapsed = 0.f;
}

void Animator::update(sf::Time dt)
{
    if (m_current >= m_clips.size())
    {
        return;
    }

    const AnimationClip& clip = m_clips[m_current];
    if (clip.frames.empty() || clip.frameDuration <= 0.f)
    {
        return;
    }

    m_elapsed += dt.asSeconds();
    while (m_elapsed >= clip.frameDuration)
    {
        m_elapsed -= clip.frameDuration;
        m_frame = (m_frame + 1) % clip.frames.size();
    }
}

sf::IntRect Animator::currentFrame() const
{
    if (m_current >= m_clips.size() || m_clips[m_current].frames.empty())
    {
        return {};
    }
    return m_clips[m_current].frames[m_frame];
}