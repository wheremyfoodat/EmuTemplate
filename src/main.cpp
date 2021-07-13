#include "MyEmulator.hpp"
#include "gui.hpp"

int main() {
    auto emulator = MyEmulator(); // Initialize emulator object
    auto gui = GUI (emulator); // Initialize GUI

    while (gui.isOpen()) // Main loop: This is run while the window hasn't been closed
        gui.update(); // GUI update handles rendering the GUI, and also running a frame if the emulator is running

    ImGui::SFML::Shutdown(); // Shut down ImGui SFML if we're down
}