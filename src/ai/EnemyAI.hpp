#pragma once

#include <cstdint>

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include "ai/DistanceField.hpp"
#include "generation/Rng.hpp"
#include "world/EnemyStats.hpp"
#include "world/TileMap.hpp"

enum class AIState
{
    Idle,
    Patrol,
    Chase,
    Attack,
    Retreat
};

struct AIContext
{
    sf::Vector2f selfPosition;
    sf::Vector2f playerPosition;
    bool playerAlive;
    const TileMap& tiles;
    const DistanceField& playerField;
    float tileSize;
};

struct AICommand
{
    sf::Vector2f velocityScale{0.f, 0.f};
    bool strike = false;
};

class EnemyAI
{
public:
    EnemyAI(const EnemyStats& stats, sf::Vector2f spawnPosition, std::uint64_t seed);

    AICommand update(const AIContext& context, sf::Time dt);

    AIState state() const;
    bool isWindingUp() const;

private:
    void enter(AIState next);

    void updateIdle(const AIContext& context, float distance);
    void updatePatrol(const AIContext& context, float distance, AICommand& command);
    void updateChase(const AIContext& context, float distance, AICommand& command);
    void updateAttack(AICommand& command);
    void updateRetreat(const AIContext& context, AICommand& command);

    bool detectsPlayer(const AIContext& context, float distance) const;
    bool pickPatrolTarget(const AIContext& context);
    sf::Vector2f chaseDirection(const AIContext& context) const;

    const EnemyStats* m_stats;
    sf::Vector2f m_spawnPosition;
    Rng m_rng;
    AIState m_state = AIState::Idle;
    float m_stateTime = 0.f;
    float m_stateDuration = 0.f;
    bool m_struck = false;
    sf::Vector2f m_patrolTarget;
};