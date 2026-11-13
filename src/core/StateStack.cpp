#include "core/StateStack.hpp"

#include <utility>

void StateStack::pop()
{
    m_pending.push_back({Operation::Pop, nullptr});
}

void StateStack::clear()
{
    m_pending.push_back({Operation::Clear, nullptr});
}

void StateStack::handleEvent(const sf::Event& event)
{
    if (!m_states.empty())
    {
        m_states.back()->handleEvent(event);
    }
    applyPendingChanges();
}

void StateStack::update(sf::Time dt)
{
    if (!m_states.empty())
    {
        m_states.back()->update(dt);
    }
    applyPendingChanges();
}

void StateStack::render(sf::RenderTarget& target)
{
    for (const auto& state : m_states)
    {
        state->render(target);
    }
}

bool StateStack::isEmpty() const
{
    return m_states.empty();
}

void StateStack::applyPendingChanges()
{
    std::vector<PendingChange> changes = std::move(m_pending);
    m_pending.clear();

    for (const PendingChange& change : changes)
    {
        switch (change.operation)
        {
        case Operation::Push:
            m_states.push_back(change.factory());
            break;
        case Operation::Pop:
            if (!m_states.empty())
            {
                m_states.pop_back();
            }
            break;
        case Operation::Clear:
            m_states.clear();
            break;
        }
    }
}