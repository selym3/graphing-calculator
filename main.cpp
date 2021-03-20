#include <iostream>
#include "headers/function.hpp"
#include "headers/font_load.hpp"

#include <SFML/Graphics.hpp>

/*

    Equation e = [](auto x) { return x[0]; };

    window.draw(equation);

*/

/////////////
// GLOBALS //
/////////////

sf::RenderWindow window {
    sf::VideoMode(600, 600),
    "Graphing Calculator",
    sf::Style::None
};

#include <cmath>

Function f = [](auto x) { return std::sin(x); };

double samples = 1e-3;
double spacing = M_PI / 3;
sf::Vector2i initialSize {
    10,
    10
};

bool lastMousePress;
sf::Vector2i pan;

sf::Vector2f rectPos; // <-- no real use except for testing

//////////////////////
// FUNCTION HEADERS //
//////////////////////

bool mouseInScreen();
sf::Vector2f getMouse();
void zoomAbout(float zoom, sf::Vector2i around);
void resetView();

void handleEvent(const sf::Event& event);
void update();
void draw();

void drawFunction(const Function&);
void drawAxes();
void drawGraph();

int main(int argc, char **argv)
{
    resetView();

    while (window.isOpen()) {    
        
        sf::Event event;
        while (window.pollEvent(event)) {
            handleEvent(event);
        }

        update();
        draw();

        sf::sleep(sf::milliseconds(20));
    }

    return 0;
}

///////////////////
// FUNCTION DEFS //
///////////////////

bool mouseInScreen()
{
    auto mouse = sf::Mouse::getPosition(window);
    auto size = window.getSize();
    return mouse.x >= 0 && mouse.y >= 0 && mouse.x < size.x && mouse.y < size.y;
}

void resetView()
{
    sf::View view = window.getDefaultView();
    view.setCenter({ 0, 0 });
    view.setSize({ 
        static_cast<sf::Vector2f>(initialSize)
    });
    window.setView(view);
}


sf::Vector2f getMouse()
{
    return window.mapPixelToCoords(
        sf::Mouse::getPosition(window)
    );
}

void zoomAbout(float zoom, sf::Vector2i around)
{
    sf::View view = window.getView();

    auto before = window.mapPixelToCoords(around);
    view.zoom(zoom);
    window.setView(view);

    view.move(before - window.mapPixelToCoords(around));
    window.setView(view);
}

void handleEvent(const sf::Event& event)
{
    const auto zoomIn = 1.1, zoomOut = 0.9;

    if (event.type == sf::Event::Closed)  {
        window.close();
    } 
    else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            window.close();
        }
        else if (event.key.code == sf::Keyboard::Q) {
            zoomAbout(zoomIn, sf::Mouse::getPosition(window));
        }
        else if (event.key.code == sf::Keyboard::E) {
            zoomAbout(zoomOut, sf::Mouse::getPosition(window));
        }
        else if (event.key.code == sf::Keyboard::Space) {
            resetView();
        }
        else if (event.key.code == sf::Keyboard::M) {
            auto tmp = getMouse();
            std::cout << "[ " << tmp.x << ", " << tmp.y << " ]\n";
        }
    }

    else if (event.type == sf::Event::MouseWheelScrolled) {
        zoomAbout(event.mouseWheelScroll.delta > 0 ? zoomOut : zoomIn, sf::Mouse::getPosition(window)); 
    }
}

void update()
{
    sf::View view = window.getView();
    view.move(
        (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) - (sf::Keyboard::isKeyPressed(sf::Keyboard::D)),
        (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) - (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    );

    bool mousePress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    
    if (mousePress && !lastMousePress) {
        pan = sf::Mouse::getPosition();
    }

    if (mousePress && lastMousePress) {
        auto newPan = sf::Mouse::getPosition();
        view.move((window.mapPixelToCoords(pan) - window.mapPixelToCoords(newPan)));
        pan = newPan;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        rectPos = getMouse();

    window.setView(view);
    lastMousePress = mousePress;
}

void draw() 
{
    window.clear(sf::Color::Black);

    drawGraph();
    drawAxes();

    drawFunction(f);

    // Draw rectangle
    // sf::RectangleShape rectangle( {50, 50} );
    // rectangle.setPosition(rectPos);
    // rectangle.setFillColor(sf::Color::Blue);
    // window.draw(rectangle);
    ///

    window.display();
}

void drawFunction(const Function& function)
{
    auto min = window.mapPixelToCoords({ 0, 0 });
    auto max = window.mapPixelToCoords(static_cast<sf::Vector2i>(
        window.getSize()
    ));

    std::vector<sf::Vertex> pointsA;
    std::vector<sf::Vertex> pointsB;

    auto color = sf::Color::White;

    // TODO: this needs to change dynamically with zoom
    auto xDistance = samples;
    for (float i = 0; i < max.x; i += xDistance) {
        pointsA.push_back(sf::Vertex({ i, -float(function(i)) }, color));
    }
    for (float i = 0; i > min.x; i -= xDistance) {
        pointsB.push_back(sf::Vertex({ i, -float(function(i)) }, color));
    }

    window.draw(&pointsA[0], pointsA.size(), sf::LinesStrip);
    window.draw(&pointsB[0], pointsB.size(), sf::LinesStrip);

}

void drawGraph() 
{
    // Calculate screen space
    auto min = window.mapPixelToCoords({ 0, 0 });
    auto max = window.mapPixelToCoords(static_cast<sf::Vector2i>(
        window.getSize()
    ));

    // Draw all the axes in one draw call
    std::vector<sf::Vertex> ticks;

    auto color = sf::Color{ 55, 55, 55};

    int xDistance = 1024 * (((max.x - min.x) / 12) / 1024); // spacing;
    xDistance = xDistance > 1e-2 ? xDistance : spacing;
    for (float i = 0; i < max.x; i += xDistance) {
        ticks.push_back(sf::Vertex({ i, min.y }, color));
        ticks.push_back(sf::Vertex({ i, max.y }, color));
    }
    for (float i = 0; i > min.x; i -= xDistance) {
        ticks.push_back(sf::Vertex({ i, min.y }, color));
        ticks.push_back(sf::Vertex({ i, max.y }, color));
    }

    auto yDistance =xDistance;// spacing;
    for (float i = 0; i < max.y; i += yDistance) {
        ticks.push_back(sf::Vertex({ min.x, i }, color));
        ticks.push_back(sf::Vertex({ max.x, i }, color));
    }
    for (float i = 0; i > min.y; i -= yDistance) {
        ticks.push_back(sf::Vertex({ min.x, i }, color));
        ticks.push_back(sf::Vertex({ max.x, i }, color));
    }

    window.draw(&ticks[0], ticks.size(), sf::Lines);
}

void drawAxes()
{
    //  make helper function
    auto min = window.mapPixelToCoords({ 0, 0 });
    auto max = window.mapPixelToCoords(static_cast<sf::Vector2i>(
        window.getSize()
    ));
    /////

    sf::Vertex axes[] = {
        // x-axis
        sf::Vertex({ min.x, 0 }),
        sf::Vertex({ max.x, 0 }),

        // y-axis
        sf::Vertex({ 0, min.y }),
        sf::Vertex({ 0, max.y })
    };

    window.draw(axes, 4, sf::Lines);
}
