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


    int boxes = 12;

    double truncationFactor = 3.0;

    double minSpacing = 1e-2;
    double defaultSpacing = 3.14159 / 3;

    void drawGraph();

public: 
    // TODO: replace this 
    Function function = [](auto x) { return x; };

private:

    double samplesPerBox = 25;

    double getBoxSpacing() const;

    void drawFunction(const Function&);

    std::tuple<sf::Vector2f, sf::Vector2f> getWindowBounds() const;

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
