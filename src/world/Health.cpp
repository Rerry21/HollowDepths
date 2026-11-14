#include "world/Health.hpp"

#include <algorithm>

Health::Health(int maximum)
    : m_current(std::max(1, maximum))
    , m_maximum(std::max(1, maximum))
{
}

int Health::current() const
{
    return m_current;
}

int Health::maximum() const
{
    return m_maximum;
}

float Health::fraction() const
{
    return static_cast<float>(m_current) / static_cast<float>(m_maximum);
}

bool Health::isDead() const
{
    return m_current <= 0;
}

void Health::damage(int amount)
{
    if (amount <= 0)
    {
        return;
    }
    m_current = std::max(0, m_current - amount);
}

void Health::heal(int amount)
{
    if (amount <= 0 || isDead())
    {
        return;
    }
    m_current = std::min(m_maximum, m_current + amount);
}