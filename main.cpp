#include "headers/grapher.hpp"

#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "headers/parser/engine.hpp"

Function getFunction(int argc, char **argv)
{
    
    if (argc == 1) {
        return [](auto x) { return std::sin(x); };
    }

    return Engine(argv[1]);

}

int main(int argc, char ** argv)
{
    Grapher grapher {
        600, 600
    };

    grapher.function = getFunction(argc, argv);

    std::cout << "Graphing...\n";

    while (grapher.isRunning()) {
        grapher.execute();
    
        sf::sleep(sf::milliseconds(20));
    }

    std::cout << "Closing...\n";
    
}