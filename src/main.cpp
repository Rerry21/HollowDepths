#include <exception>
#include <iostream>
#include <string>

#include "core/Game.hpp"
#include "tests/GeneratorSelfTest.hpp"

int main(int argc, char** argv)
{
    try
    {
        if (argc > 1 && std::string(argv[1]) == "--selftest")
        {
            const int seedCount = (argc > 2) ? std::stoi(argv[2]) : 1000;
            return runGeneratorSelfTest(seedCount);
        }

        Game game;
        game.run();
    }
    catch (const std::exception& error)
    {
        std::cerr <<"Fatal error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}