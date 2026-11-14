#include <exception>
#include <iostream>

#include "core/Game.hpp"

int main()
{
    try
    {
        Game game;
        game.run();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}