#include <iostream>
#include "headers/function.hpp"

#include <SFML/Graphics.hpp>

/////////////
// GLOBALS //
/////////////

sf::RenderWindow window {
    sf::VideoMode(600, 600),
    "Graphing Calculator",
    sf::Style::None
};

bool lastMousePress;
sf::Vector2i pan;
// sf::Vector2f pan;

//////////////////////
// FUNCTION HEADERS //
//////////////////////

sf::Vector2f getMouse();
void zoomRelative(float zoom);

void handleEvent(const sf::Event& event);
void update();
void draw();

int main(int argc, char **argv)
{
    sf::View view = window.getView();
    view.setCenter({ 0, 0 });

    window.setView(view);

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

sf::Vector2f getMouse()
{
    return window.mapPixelToCoords(
        sf::Mouse::getPosition(window)
    );
}

void zoomRelative(float zoom)
{
    sf::View view = window.getView();

    auto mouse = getMouse();
    view.zoom(zoom);
    window.setView(view);

    view.move(mouse - getMouse());

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
            zoomRelative(zoomIn);
        }
        else if (event.key.code == sf::Keyboard::E) {
            zoomRelative(zoomOut);
        }
        else if (event.key.code == sf::Keyboard::Space) {
            auto tmp = getMouse();
            std::cout << "[ " << tmp.x << ", " << tmp.y << " ]\n";
        }
    }

    else if (event.type == sf::Event::MouseWheelScrolled) {
        zoomRelative(event.mouseWheelScroll.delta > 0 ? zoomOut : zoomIn); 
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

    window.setView(view);
    lastMousePress = mousePress;
}

void draw() 
{
    window.clear(sf::Color::Black);


    // Draw rectangle

    sf::RectangleShape rectangle;
    rectangle.setSize({ 100, 50 });
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        rectangle.setPosition(getMouse());

    window.draw(rectangle);

    // Draw grid

    int x = window.getSize().x;
    int y = window.getSize().y;

    auto topLeft = window.mapPixelToCoords({ 0, 0 });
    auto bottomRight = window.mapPixelToCoords({ x, y });

    sf::Vertex xAxis[] = {
        sf::Vertex({ topLeft.x, 0 }),
        sf::Vertex({ bottomRight.x, 0 })
    };

    window.draw(xAxis, 2, sf::Lines);

    sf::Vertex yAxis[] = {
        sf::Vertex({ 0, topLeft.y }),
        sf::Vertex({ 0, bottomRight.y })
    };

    window.draw(yAxis, 2, sf::Lines);

    window.display();
}