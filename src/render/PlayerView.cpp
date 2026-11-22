#include "render/PlayerView.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace
{
    constexpr unsigned int kFrameSize = 16;
    constexpr unsigned int kFrameCount = 6;
    constexpr float kSpriteScale = 2.f;
    constexpr float kBlinkPeriod = 0.16f;

    enum class Clip : std::size_t
    {
        Idle = 0,
        Walk = 1
    };

    struct Pose
    {
        unsigned int bodyDrop;
        unsigned int leftLeg;
        unsigned int rightLeg;
    };

    constexpr Pose kPoses[kFrameCount] = {
        {0, 3, 3},
        {1, 2, 2},
        {0, 3, 1},
        {1, 2, 2},
        {0, 1, 3},
        {1, 2, 2},
    };

    const sf::Color kBodyColor(80, 200, 255);
    const sf::Color kLegColor(40, 130, 190);
    const sf::Color kEyeColor(15, 25, 50);

    void fillRect(sf::Image& image,
                  unsigned int left,
                  unsigned int top,
                  unsigned int width,
                  unsigned int height,
                  sf::Color color)
    {
        for (unsigned int y = top; y < top + height; ++y)
        {
            for (unsigned int x = left; x < left + width; ++x)
            {
                image.setPixel({x, y}, color);
            }
        }
    }

    sf::Texture createSheetTexture()
    {
        sf::Image image(sf::Vector2u(kFrameSize * kFrameCount, kFrameSize), sf::Color::Transparent);

        for (unsigned int frame = 0; frame < kFrameCount; ++frame)
        {
            const Pose& pose = kPoses[frame];
            const unsigned int originX = frame * kFrameSize;

            fillRect(image, originX + 3, 2 + pose.bodyDrop, 10, 9, kBodyColor);
            fillRect(image, originX + 10, 4 + pose.bodyDrop, 2, 2, kEyeColor);
            fillRect(image, originX + 3, 11 + pose.bodyDrop, 2, pose.leftLeg, kLegColor);
            fillRect(image, originX + 11, 11 + pose.bodyDrop, 2, pose.rightLeg, kLegColor);
        }

        sf::Texture texture;
        if (!texture.loadFromImage(image))
        {
            throw std::runtime_error("Failed to create the player sprite sheet");
        }
        return texture;
    }

    sf::IntRect frameRect(unsigned int index)
    {
        return sf::IntRect({static_cast<int>(index * kFrameSize), 0},
                           {static_cast<int>(kFrameSize), static_cast<int>(kFrameSize)});
    }

    sf::Color tintFor(const Player& player)
    {
        if (player.health().isDead())
        {
            return sf::Color(140, 70, 70);
        }

        if (player.isInvulnerable())
        {
            const bool faded = std::fmod(player.invulnerabilityRemaining(), kBlinkPeriod) > kBlinkPeriod / 2.f;
            return faded ? sf::Color(255, 255, 255, 90) : sf::Color::White;
        }

        return sf::Color::White;
    }
}

PlayerView::PlayerView()
    : m_texture(createSheetTexture())
    , m_sprite(m_texture)
{
    const float half = static_cast<float>(kFrameSize) / 2.f;
    m_sprite.setOrigin({half, half});
    m_sprite.setScale({kSpriteScale, kSpriteScale});

    AnimationClip idle;
    idle.frameDuration = 0.5f;
    idle.frames = {frameRect(0), frameRect(1)};

    AnimationClip walk;
    walk.frameDuration = 0.12f;
    walk.frames = {frameRect(2), frameRect(3), frameRect(4), frameRect(5)};

    m_animator.addClip(idle);
    m_animator.addClip(walk);
}

void PlayerView::update(const Player& player, sf::Time dt)
{
    const sf::Vector2f velocity = player.velocity();
    const bool moving = velocity.x != 0.f || velocity.y != 0.f;

    m_animator.play(static_cast<std::size_t>(moving ? Clip::Walk : Clip::Idle));
    m_animator.update(dt);
}

void PlayerView::draw(sf::RenderTarget& target, const Player& player)
{
    const float direction = (player.facing() == Facing::Left) ? -1.f : 1.f;
    const sf::Vector2f position = player.position();

    m_sprite.setTextureRect(m_animator.currentFrame());
    m_sprite.setScale({direction * kSpriteScale, kSpriteScale});
    m_sprite.setPosition({std::round(position.x), std::round(position.y)});
    m_sprite.setColor(tintFor(player));

    target.draw(m_sprite);
}

void PlayerView::drawSwing(sf::RenderTarget& target, const Swing& swing) const
{
    if (swing.timeLeft <= 0.f)
    {
        return;
    }

    constexpr int kSegments = 12;
    constexpr float kHalfArc = 0.7853982f;

    const float fade = swing.timeLeft / swing.duration;
    const sf::Color color(255, 255, 255, static_cast<std::uint8_t>(160.f * fade));
    const float base = std::atan2(swing.direction.y, swing.direction.x);

    sf::VertexArray fan(sf::PrimitiveType::TriangleFan);
    fan.append(sf::Vertex{swing.origin, color});
    for (int i = 0; i <= kSegments; ++i)
    {
        const float angle = base - kHalfArc + 2.f * kHalfArc * static_cast<float>(i) / static_cast<float>(kSegments);
        fan.append(sf::Vertex{{swing.origin.x + std::cos(angle) * swing.range,
                               swing.origin.y + std::sin(angle) * swing.range},
                              color});
    }
    target.draw(fan);
}