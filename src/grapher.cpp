#include "../headers/grapher.hpp"

#include <SFML/Window/Event.hpp>
#include <iostream>

// Constructor

Grapher::Grapher(
    unsigned int width,
    unsigned int height
) :
    window {
        sf::VideoMode(width, height),
        "Graphing Calculator",
        sf::Style::None
    }
{
    resetView();
}

// Main Loop

bool Grapher::isRunning() const
{
    return window.isOpen();
}

void Grapher::execute()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        handleEvent(event);
    }

    update();
    draw();
}

// Event handling

sf::Vector2f Grapher::getMouseCoords() const
{
    return window.mapPixelToCoords(
        sf::Mouse::getPosition(window)
    );
}

void Grapher::handleEvent(const sf::Event& event)
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
            auto tmp = getMouseCoords();
            std::cout << "[ " << tmp.x << ", " << tmp.y << " ]\n";
        }
    }

    else if (event.type == sf::Event::MouseWheelScrolled) {
        zoomAbout(
            event.mouseWheelScroll.delta > 0 ? zoomOut : zoomIn, 
            sf::Mouse::getPosition(window)
        ); 
    }
}

void Grapher::update()
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

// Rendering

void Grapher::draw()
{
    window.clear(sf::Color::Black);

    drawGraph();
    drawAxes();

    drawFunction(function);

    window.display();
}

void Grapher::drawGraph()
{
    auto bounds = getWindowBounds();
    auto min = std::get<0>(bounds), max = std::get<1>(bounds);

    // Draw all the axes in one draw call
    std::vector<sf::Vertex> ticks;

    auto color = sf::Color{ 55, 55, 55 };

    int xDistance = 1024 * (((max.x - min.x) / boxes) / 1024); 
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

void Grapher::drawAxes()
{
    //  make helper function
    auto bounds = getWindowBounds();
    auto min = std::get<0>(bounds), max = std::get<1>(bounds);
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

void Grapher::drawFunction(const Function& f)
{
    auto bounds = getWindowBounds();
    auto min = std::get<0>(bounds), max = std::get<1>(bounds);

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

std::tuple<sf::Vector2f, sf::Vector2f> Grapher::getWindowBounds()
{
    return std::tuple<sf::Vector2f, sf::Vector2f>{
        window.mapPixelToCoords({ 0, 0 }),
        window.mapPixelToCoords(static_cast<sf::Vector2i>(
            window.getSize()
        ))
    };
}

// Window & Zoom

void Grapher::zoomAbout(float zoom, sf::Vector2i around)
{
    sf::View view = window.getView();

    auto before = window.mapPixelToCoords(around);
    view.zoom(zoom);
    window.setView(view);

    view.move(before - window.mapPixelToCoords(around));
    window.setView(view);
}

void Grapher::resetView()
{
    sf::View view = window.getDefaultView();
    view.setCenter({ 0, 0 });
    view.setSize({ 
        static_cast<sf::Vector2f>(initialSize)
    });
    window.setView(view);
}