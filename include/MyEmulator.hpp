#pragma once
#include <array>
#include <filesystem>
#include "utils.hpp"
#include "recompiler.hpp"

class MyEmulator {
    Chipper chip;

public:
    static constexpr int width = 64;
    static constexpr int height = 32;
    static constexpr int cyclesPerFrame = 60;

    bool isRunning = false;
    bool loadedROM = false;
    
    void loadROM (const std::filesystem::path& path) {
        chip.reset();
        chip.loadROM (path);
        loadedROM = true;
    }

    void step() {
        chip.execute();
    }

    uint8_t* getFramebuffer() {
        return chip.getFramebuffer();
    }

    void runFrame();
};