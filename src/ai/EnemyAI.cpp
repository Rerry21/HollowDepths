#include "ai/EnemyAI.hpp"

#include "ai/Perception.hpp"

namespace
{
    constexpr float kPatrolSpeedScale = 0.5f;
    constexpr float kPatrolArrivalRadius = 6.f;
    constexpr float kPatrolTimeout = 6.f;
    constexpr int kPatrolRangeTiles = 3;
    constexpr float kLeashTiles = 6.f;
    constexpr int kPatrolAttempts = 8;
    constexpr int kMinIdleDeciseconds = 10;
    constexpr int kMaxIdleDeciseconds = 30;

    sf::Vector2f directionTo(sf::Vector2f from, sf::Vector2f to)
    {
        const sf::Vector2f delta = to - from;
        if (delta.lengthSquared() < 0.0001f)
        {
            return {0.f, 0.f};
        }
        return delta.normalized();
    }
}

EnemyAI::EnemyAI(const EnemyStats& stats, sf::Vector2f spawnPosition, std::uint64_t seed)
    : m_stats(&stats)
    , m_spawnPosition(spawnPosition)
    , m_rng(seed)
    , m_patrolTarget(spawnPosition)
{
    enter(AIState::Idle);
}

AICommand EnemyAI::update(const AIContext& context, sf::Time dt)
{
    AICommand command;
    m_stateTime += dt.asSeconds();

    if (!context.playerAlive && m_state != AIState::Idle && m_state != AIState::Patrol)
    {
        enter(AIState::Idle);
    }

    const float distance = (context.playerPosition - context.selfPosition).length();

    switch (m_state)
    {
    case AIState::Idle:
        updateIdle(context, distance);
        break;
    case AIState::Patrol:
        updatePatrol(context, distance, command);
        break;
    case AIState::Chase:
        updateChase(context, distance, command);
        break;
    case AIState::Attack:
        updateAttack(command);
        break;
    case AIState::Retreat:
        updateRetreat(context, command);
        break;
    }

    return command;
}

AIState EnemyAI::state() const
{
    return m_state;
}

bool EnemyAI::isWindingUp() const
{
    return m_state == AIState::Attack && m_stateTime < m_stats->windupTime;
}

void EnemyAI::enter(AIState next)
{
    m_state = next;
    m_stateTime = 0.f;
    m_struck = false;

    switch (next)
    {
    case AIState::Idle:
        m_stateDuration = static_cast<float>(m_rng.range(kMinIdleDeciseconds, kMaxIdleDeciseconds)) * 0.1f;
        break;
    case AIState::Patrol:
        m_stateDuration = kPatrolTimeout;
        break;
    case AIState::Chase:
        m_stateDuration = 0.f;
        break;
    case AIState::Attack:
        m_stateDuration = m_stats->windupTime + m_stats->recoverTime;
        break;
    case AIState::Retreat:
        m_stateDuration = m_stats->retreatTime;
        break;
    }
}

void EnemyAI::updateIdle(const AIContext& context, float distance)
{
    if (detectsPlayer(context, distance))
    {
        enter(AIState::Chase);
        return;
    }

    if (m_stateTime >= m_stateDuration)
    {
        if (pickPatrolTarget(context))
        {
            enter(AIState::Patrol);
        }
        else
        {
            enter(AIState::Idle);
        }
    }
}

void EnemyAI::updatePatrol(const AIContext& context, float distance, AICommand& command)
{
    if (detectsPlayer(context, distance))
    {
        enter(AIState::Chase);
        return;
    }

    const sf::Vector2f toTarget = m_patrolTarget - context.selfPosition;
    if (toTarget.length() <= kPatrolArrivalRadius || m_stateTime >= m_stateDuration)
    {
        enter(AIState::Idle);
        return;
    }

    command.velocityScale = toTarget.normalized() * kPatrolSpeedScale;
}

void EnemyAI::updateChase(const AIContext& context, float distance, AICommand& command)
{
    if (!context.playerAlive || distance > m_stats->loseRadius)
    {
        enter(AIState::Idle);
        return;
    }

    if (distance <= m_stats->attackRange &&
        hasLineOfSight(context.tiles, context.tileSize, context.selfPosition, context.playerPosition))
    {
        enter(AIState::Attack);
        return;
    }

    command.velocityScale = chaseDirection(context);
}

void EnemyAI::updateAttack(AICommand& command)
{
    if (!m_struck && m_stateTime >= m_stats->windupTime)
    {
        m_struck = true;
        command.strike = true;
    }

    if (m_stateTime >= m_stateDuration)
    {
        enter(m_stats->retreatTime > 0.f ? AIState::Retreat : AIState::Chase);
    }
}

void EnemyAI::updateRetreat(const AIContext& context, AICommand& command)
{
    if (m_stateTime >= m_stateDuration)
    {
        enter(AIState::Chase);
        return;
    }

    command.velocityScale = directionTo(context.playerPosition, context.selfPosition);
}

bool EnemyAI::detectsPlayer(const AIContext& context, float distance) const
{
    return context.playerAlive && distance <= m_stats->detectRadius &&
           hasLineOfSight(context.tiles, context.tileSize, context.selfPosition, context.playerPosition);
}

bool EnemyAI::pickPatrolTarget(const AIContext& context)
{
    const sf::Vector2i origin = tileOf(context.selfPosition, context.tileSize);
    const float halfSize = m_stats->size / 2.f;

    for (int attempt = 0; attempt < kPatrolAttempts; ++attempt)
    {
        const int dx = m_rng.range(-kPatrolRangeTiles, kPatrolRangeTiles);
        const int dy = m_rng.range(-kPatrolRangeTiles, kPatrolRangeTiles);
        const sf::Vector2i tile{origin.x + dx, origin.y + dy};

        if (!context.tiles.isWalkable(tile.x, tile.y))
        {
            continue;
        }

        const sf::Vector2f target = tileCentre(tile, context.tileSize);
        if ((target - m_spawnPosition).length() > kLeashTiles * context.tileSize)
        {
            continue;
        }

        if (!hasLineOfSight(context.tiles, context.tileSize, context.selfPosition, target, halfSize))
        {
            continue;
        }

        m_patrolTarget = target;
        return true;
    }

    return false;
}

sf::Vector2f EnemyAI::chaseDirection(const AIContext& context) const
{
    const float halfSize = m_stats->size / 2.f;
    if (hasLineOfSight(context.tiles, context.tileSize, context.selfPosition, context.playerPosition, halfSize))
    {
        return directionTo(context.selfPosition, context.playerPosition);
    }

    const sf::Vector2i tile = tileOf(context.selfPosition, context.tileSize);
    const sf::Vector2i next = context.playerField.nextStep(tile);
    if (next == tile)
    {
        return directionTo(context.selfPosition, context.playerPosition);
    }

    return directionTo(context.selfPosition, tileCentre(next, context.tileSize));
}