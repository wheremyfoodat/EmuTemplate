#include <filesystem>          // For std::filesystem::path
#include "fmt/format.h"        // For fmt::print
#include "tinyfiledialogs.h"   // For file explorer
#include "gui.hpp"
#include "MyEmulator.hpp"

GUI::GUI (MyEmulator& emulator) : window(sf::VideoMode(800, 600), "SFML window"), emulator(emulator) {
    window.setFramerateLimit(60); // cap FPS to 60
    ImGui::SFML::Init(window);    // Init Imgui-SFML
    display.create (720, 480);

    auto& io = ImGui::GetIO();  // Set some ImGui options
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable navigation with keyboard
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Configure viewport feature
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;

    io.Fonts -> Clear();  // Use Deja Vu Sans Mono as the font if found
    auto fontPath = std::filesystem::current_path() / "DejaVuSansMono.ttf";
    auto font = io.Fonts -> AddFontFromFileTTF (fontPath.string().c_str(), 18.f);
    if (!font) // Fall back to default font if not found
        io.Fonts -> AddFontDefault();
    ImGui::SFML::UpdateFontTexture(); // Updates font texture
}

void GUI::update() {
    sf::Event event;
    
    while (window.pollEvent(event)) { // Poll events
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed)
            window.close();
    }
    ImGui::SFML::Update(window, deltaClock.restart()); // Update imgui-sfml

    showMenuBar(); // Render GUI stuff

    window.clear(); // Render
    ImGui::SFML::Render(window);
    window.display();
}

void GUI::showMenuBar() {
    static const char* romTypes[] = { "*.bin", "*.rom" }; // Some generic filetypes for ROMs, configure as you want
    
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem ("Open ROM")) { // Show file selection dialog if open ROM button is pressed
            auto file = tinyfd_openFileDialog(
                "Choose a ROM", // File explorer window title
                "",             // Default directory
                2,              // Amount of file types
                romTypes,       // Array of file types
                "ROMs",         // File type description in file explorer window
                0);

            if (file != nullptr) {  // Check if file dialog was canceled
                const auto path = std::filesystem::path (file);
                fmt::print ("Opened file {}\n", path.string());
            }
        }

        ImGui::EndMainMenuBar();
    }
}
