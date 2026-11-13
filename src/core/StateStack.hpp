#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "core/State.hpp"

class StateStack
{
    public:
        template <typename T>
        void push(Context& context);

        void pop();
        void clear();

        void handleEvent(const sf::Event& event);
        void update(sf::Time dt);
        void render(sf::RenderTarget& target);

        bool isEmpty() const;

    private:
        enum class Operation
        {
            Push,
            Pop,
            Clear
        };

        struct PendingChange
        {
            Operation operation;
            std::function<std::unique_ptr<State>()> factory;
        };

        void applyPendingChanges();

        std::vector<std::unique_ptr<State>> m_states;
        std::vector<PendingChange> m_pending;
};

template <typename T>
void StateStack::push(Context& context)
{
    m_pending.push_back({Operation::Push, [&context]() {return std::make_unique<T>(context); }});
}