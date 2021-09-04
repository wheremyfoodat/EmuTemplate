#include <cstring>
#include "recompiler.hpp"
#define DISPLAY_OFFSET(x) ((uintptr_t) &display[(x)] - (uintptr_t) this)

using namespace Xbyak;
using namespace Xbyak::util;

Chipper::Chipper() {
    reset();
}

void Chipper::reset() {
    pc = 0x200;
    std::memcpy (&memory[0], &font[0], 80); // Copy font to the first 80 bytes of memory
    std::memset (&display[0], 0xFF, 64 * 32 * 4); // Make framebuffer white.
    blocks.fill (nullptr);
}

void Chipper::loadROM (const std::filesystem::path& path) {
    const auto rom = Helpers::loadROM (path.string());
    std::memcpy (&memory[0x200], &rom[0], rom.size()); // Copy the ROM to memory
                                                       // We may want to sanitize inputs...
}

void Chipper::execute() {
    auto recompilerFunc = blocks[pc];
    if (recompilerFunc == nullptr) {
        recompile(pc);
        recompilerFunc = blocks[pc];
        cycles += (*recompilerFunc)();
    }

    else {
        cycles += (*recompilerFunc)();  // Jump to emitted code
    }
}

void Chipper::recompile (uint16_t recompilerPC) {
    pcWrittenBack = false;
    madeStackFrame = false;
    compiling = true;

    if (e.getSize() > codeCacheSize)
        Helpers::panic ("Implement code cache flushing\n");

    if (recompilerPC <= 4096)
        blocks[recompilerPC] = (DynarecCallback) e.getCurr();

    loadContext();

    while (compiling) {
        if (recompilerPC >= 4096) {
            Helpers::panic("Executing from invalid PC: {:04X}\n", recompilerPC);
        }

        const uint16_t instruction = (memory[recompilerPC] << 8) | memory[recompilerPC + 1];
        recompilerPC += 2;
        recompileInstruction (instruction);
    }

    Helpers::panic ("Floosh registers");
}

void Chipper::recompileInstruction (uint16_t instruction) {
    switch (instruction >> 12) {
        case 0:
            if ((instruction & 0xFF) == 0xE0) // CLS
                recCLS();
            else
                Helpers::panic ("Unimplemented instruction {:04X}\n", instruction);
            break;
        
        default:
            Helpers::panic ("Unimplemented instruction {:04X}\n", instruction);
    }
}

void Chipper::recCLS() {
    alignas(32) static constexpr uint8_t val[32] = { // 8 black RGBA32 pixels
        0, 0, 0, 255, 0, 0, 0, 255,
        0, 0, 0, 255, 0, 0, 0, 255,
        0, 0, 0, 255, 0, 0, 0, 255,
        0, 0, 0, 255, 0, 0, 0, 255
    };

    e.vmovdqa32(ymm0, yword [rip + val]); // Load the thing
    for (auto i = 0; i < 64 * 32; i += 32) { // Clear 32 bytes a pop
        e.vmovdqa32(yword [contextPointer + DISPLAY_OFFSET(i)], ymm0);
    }
}