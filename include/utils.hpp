#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <bitset>
#include <utility>     // For std::pair, used by loadROMWithHash
#include "sha1.hpp"    // For calculating SHA hashes, used by loadROMWithHash
#include "fmt/format.h" // Core fmt functions
#include "fmt/color.h"  // Text coloring fmt functions

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

namespace Helpers {
    template <typename... Args>
    [[noreturn]] static void panic (const char* fmt, const Args&... args) {
        fmt::print (fg(fmt::color::red), fmt, args...);
        fmt::print (fg(fmt::color::red), "Aborting...\n");
        exit(1);
    }

    template <typename... Args>
    static void panic (bool condition, const char* fmt, const Args&... args) {
        if (condition) {
            fmt::print (fg(fmt::color::red), fmt, args...);
            fmt::print (fg(fmt::color::red), "Aborting...\n");
            exit(1);
        }
    }

    template <typename... Args>
    static void warn (const char* fmt, const Args&... args) {
        fmt::print (fg(fmt::color::red), fmt, args...);
    }

    // Read a ROM into a vector of uint8_t
    static auto loadROM (std::string directory) {
        std::ifstream file (directory, std::ios::binary);
        if (file.fail())
            panic ("Couldn't read file at {}\n", directory.c_str());

        std::vector<u8> ROM;
        file.unsetf(std::ios::skipws);

        ROM.insert(ROM.begin(),
                  std::istream_iterator<uint8_t>(file),
                   std::istream_iterator<uint8_t>());

        file.close();
        return ROM;
    }

    // Read a ROM into a vector of uint8_t. Return the ROM and its SHA-1 hash
    static auto loadROMWithHash (std::string directory) -> std::pair <std::vector <u8>, std::string> {
        std::ifstream file (directory, std::ios::binary);
        if (file.fail())
            panic ("Couldn't read file at {}\n", directory.c_str());

        std::vector<u8> ROM;
        file.unsetf(std::ios::skipws);

        ROM.insert(ROM.begin(),
                  std::istream_iterator<uint8_t>(file),
                   std::istream_iterator<uint8_t>());

        file.clear(); // Clear fail and EOF flags
        file.seekg(0, std::ios::beg); // Seek back to the start to calculate the SHA-1 checksum
        auto hash = SHA1::from_stream (file); // Calculate the checksum
        file.close(); // Close the file
        return std::make_pair (ROM, hash); // Return the vector and its hash
    }

    static constexpr bool buildingInDebugMode() {
        #ifdef NDEBUG
            return false;
         #endif

         return true;
    }

    template <typename... Args>
    static void log (const char* fmt, const Args&... args) {
        if constexpr (buildingInDebugMode()) {
            fmt::print (fmt, args...);
        }
    }

    static inline constexpr u32 signExtend32 (u32 value, u32 startingSize) {
        auto temp = (s32) value;
        auto bitsToShift = 32 - startingSize;
        return (u32) (temp << bitsToShift >> bitsToShift);
    }

    static inline constexpr u16 signExtend16 (u16 value, u32 startingSize) {
        auto temp = (s16) value;
        auto bitsToShift = 16 - startingSize;
        return (u16) (temp << bitsToShift >> bitsToShift);
    }

    static inline constexpr bool isBitSet (u32 value, int bit) {
        return (value >> bit) & 1;
    }

    template <typename T>
    static inline constexpr T rotr (T value, int bits) {
        if (!bits)
            return value; 

        constexpr auto bitWidth = sizeof(T) * 8;
        bits &= bitWidth - 1;
        return (value >> bits) | (value << (bitWidth - bits));
    }

    template <typename T>
    static inline constexpr T rotl (T value, int bits) {
        if (!bits)
            return value; 

        constexpr auto bitWidth = sizeof(T) * 8;
        bits &= bitWidth - 1;
        return (value << bits) | (value >> (bitWidth - bits));
    }

    static inline void debugAssert (bool condition, std::string message) {
        if constexpr (buildingInDebugMode()) {
            if (!condition) 
                Helpers::panic ("Assertion failed. Message: {}\n", message);
        }
    }

    template <typename T>
    T bswap(T);

    template<>
    inline u8 bswap <u8> (u8 val) {
        return val;
    }

    template<>
    inline u16 bswap <u16> (u16 val) {
        return __builtin_bswap16(val);
    }

    template<>
    inline u32 bswap <u32> (u32 val) {
        return __builtin_bswap32(val);
    }

    template<>
    inline u64 bswap <u64> (u64 val) {
        return __builtin_bswap64(val);
    }

    inline u32 clz (u32 val) {
        return val ? __builtin_clz(val) : 0;
    }

    template <typename T>
    T readBE (const u8* pointer) {
        // TODO: Detect and support big endian systems
        return bswap(*(const T*)(pointer));
    }

    template <typename T>
    void writeBE (u8* pointer, T value) {
        // TODO: Detect and support big endian systems
        *(T*)pointer = bswap(value);
    }

    constexpr u8 get8BitColor (u8 color5) {
        return (color5 << 3) | (color5 >> 2);
    }

    inline int popcnt32 (int number) {
        #if defined (__has_builtin) 
            #if __has_builtin (__builtin_popcount)
                return __builtin_popcount (number);
            #endif
        #else
            return std::bitset<32>{number}.count();
        #endif
    }
}
