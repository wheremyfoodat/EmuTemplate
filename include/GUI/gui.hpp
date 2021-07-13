#pragma once
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "MyEmulator.hpp"

class GUI {
    sf::RenderWindow window;
    sf::Clock deltaClock;
    sf::Texture display;
    MyEmulator& emulator;

public:
    GUI (MyEmulator& emulator);

    void update(); // Update the GUI
    bool isOpen() { return window.isOpen(); } // Shows if the GUI window has been closed or not

private:
    void showMenuBar();
};