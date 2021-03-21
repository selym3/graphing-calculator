#include "headers/grapher.hpp"

#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>

int main(void)
{
    Grapher grapher {
        600, 600
    };

    grapher.function = [](auto x) { return std::sin(x); };

    std::cout << "Graphing...\n";

    while (grapher.isRunning()) {
        grapher.execute();
    
        sf::sleep(sf::milliseconds(20));
    }

    std::cout << "Closing...\n";
    
}