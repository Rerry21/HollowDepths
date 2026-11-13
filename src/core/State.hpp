#pragma once

#include <SFML/Graphics.hpp>

class Input;
class StateStack;

struct Context
{
    sf::RenderWindow& window;
    Input& input;
    StateStack& stack;
};

class State
{
public:
    explicit State(Context& context)
        : m_context(context)
    {
    }

    virtual ~State() = default;

    State(const State&) = delete;
    State& operator=(const State&) = delete;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void render(sf::RenderTarget& target) = 0;

protected:
    Context& context() { return m_context; }

private:
    Context& m_context;
};