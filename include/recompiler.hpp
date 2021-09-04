#pragma once
#include <array>
#include <filesystem>
#include <optional>
#include "emitter.hpp"
#include "platforms.hpp"
#include "utils.hpp"

class Chipper {
    using DynarecCallback = uint32_t(*)(); // A function pointer to JIT-emitted code

    uint8_t regs[16];
    uint16_t pc = 0x200;
    uint16_t index = 0;
    int stackPointer = 0;

    Emitter e;
    bool compiling = false;
    bool pcWrittenBack = false;
    bool madeStackFrame = false;

    enum class RegState { Unknown, Constant };

    struct Register {
        uint8_t val = 0; // The register's cached value used for constant propagation
        RegState state = RegState::Unknown; // Is this register's value a constant, or an unknown value?

        bool allocated = false; // Has this guest register been allocated to a host reg?
        bool writeback = false; // Does this register need to be written back to memory at the end of the block?
        Reg32 allocatedReg; // If a host reg has been allocated to this register, which reg is it?
        int allocatedRegIndex = 0;

        inline bool isConst() { return state == RegState::Constant; }
        inline bool isAllocated() { return allocated; }
        inline void markConst(uint8_t value) {
            val = value;
            state = RegState::Constant;
            writeback = false; // Disable writeback in case the reg was previously allocated with writeback
            allocated = false; // Unallocate register
        }

        // Note: It's important that markUnknown does not modify the val field as that would mess up codegen
        inline void markUnknown() {
            state = RegState::Unknown;
        }

        inline void setWriteback() {
            writeback = true;
        }
    };

    struct HostRegister {
        std::optional<int> mappedReg = std::nullopt; // The register this is allocated to, if any
        bool restore = false; // Did this register need to get restored after this block?
    };

    std::array <Register, 16> regState;
    std::array <HostRegister, ALLOCATEABLE_REG_COUNT> hostRegs;
    std::array <DynarecCallback, 4096> blocks;

    alignas(32) uint32_t display[64 * 32]; // Must be 32-byte aligned to use AVX2 on it
    uint16_t stack[16];
    uint8_t memory[4096];

    inline void markConst (int index, uint8_t value) {
        regState[index].markConst(value);
        if (hostRegs[regState[index].allocatedRegIndex].mappedReg == index) {
            hostRegs[regState[index].allocatedRegIndex].mappedReg = std::nullopt;  // Unmap the register on the host reg side too
        }
    }

    void recompile (uint16_t recompilerPC);
    void recompileInstruction (uint16_t instruction);
    void flushCache();
    void loadContext() {
        e.push(contextPointer);
        e.mov(contextPointer, (uintptr_t) this);
    }

    static constexpr uint8_t font [80] = { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    void recCLS();

public:
    Chipper();
    void execute();
    void reset();
    void loadROM (const std::filesystem::path& path);

    int cycles = 0;
    static constexpr int MAX_BLOCK_SIZE = 50;

    uint8_t* getFramebuffer() {
        return (uint8_t*) &display[0];
    }

    void dumpBuffer() {
        std::ofstream file ("DynarecOutput.dump", std::ios::binary); // Make a file for our dump
        file.write ((const char*) e.getCode(), e.getSize()); // Write the code buffer to the dump
    }
};