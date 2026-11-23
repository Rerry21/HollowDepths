#pragma once

#include "world/Entity.hpp"
#include "world/Health.hpp"

#include <optional>

#include "items/Item.hpp"

enum class Facing
{
    Left,
    Right
};

class Player : public Entity
{
public:
    static constexpr float kAttackRange = 52.f;

    explicit Player(sf::Vector2f position);

    void update(sf::Time dt) override;
    void setMoveDirection(sf::Vector2f direction);

    bool takeDamage(int amount);
    void heal(int amount);
    bool tryAttack();
    bool tryShoot();
    void applyItem(const Item& item);
    const std::optional<Item>& weapon() const;
    const std::optional<Item>& armour() const;
    int shotDamage() const;
    void faceTowards(float dx);

    int attackDamage() const;
    const Health& health() const;
    Facing facing() const;
    bool isInvulnerable() const;
    float invulnerabilityRemaining() const;

private:
    sf::Vector2f m_moveDirection{0.f, 0.f};
    float m_speed;
    Health m_health;
    float m_invulnerableTimer = 0.f;
    float m_attackTimer = 0.f;
    float m_shotTimer = 0.f;
    std::optional<Item> m_weapon;
    std::optional<Item> m_armour;
    Facing m_facing = Facing::Right;
};