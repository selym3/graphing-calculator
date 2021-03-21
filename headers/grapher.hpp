#ifndef __GRAPHER_HPP__
#define __GRAPHER_HPP__

#include <SFML/Graphics/RenderWindow.hpp>
#include "function.hpp"

#include <tuple>

class Grapher
{
public:
    // Main loop

    Grapher(unsigned int width, unsigned int height);

    bool isRunning() const;
    void execute();

private:
    // Event handling

    void handleEvent(const sf::Event&);
    void update();

    sf::Vector2f getMouseCoords() const;

private:
    // Rendering
    void draw();

    void drawAxes();
    void drawGraph();

public: 
    // TODO: replace this 
    Function function = [](auto x) { return x; };

private:
    double samples = 1e-3;
    double spacing = 3.14159 / 3;

    int boxes = 12;

    void drawFunction(const Function&);

    std::tuple<sf::Vector2f, sf::Vector2f> getWindowBounds();

private:
    // Window & View
    void resetView();
    void zoomAbout(float zoom, sf::Vector2i around);

    sf::Vector2i pan;
    bool lastMousePress;

    sf::Vector2i initialSize {
        10,
        10
    };

    sf::RenderWindow window;

};


#endif
