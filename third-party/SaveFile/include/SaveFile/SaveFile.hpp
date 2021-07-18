// Wrapper for memory-mapped files used for save files
// Via memory-mapped files, we can use our SRAM like a normal array, and have the OS periodically flush it into a file for us, automatically!
#pragma once
#include <fstream>
#include <iostream>
#include <filesystem>
#include <system_error>
#include "mio/mio.hpp"

class SaveFile {
    std::filesystem::path path = std::filesystem::path ("");
    std::error_code error; // Just an error code
    mio::mmap_sink map; // mmap sink for our file
    uint8_t* pointer; // Pointer to the contents of the memory mapped file
    size_t beeg = 0; // Size of memory mapped file

    // Make file at directory "path" with a size of "size"
    static void makeFile (const std::filesystem::path path, const int size) {
        std::ofstream file (path);
        std::string s (size, '0');
        file << s;
    }

    static void panic (std::string message) {
        std::cout << "[SaveFile][Fatal] " << message;
        exit (1);
    }
    
public:
    bool exists() { return !path.empty(); }
    size_t size() { return beeg; }
    void unmap()  { map.unmap(); }
    auto begin()  { return map.begin(); }
    auto end()    { return map.end(); }

    std::filesystem::path extension() { return path.extension(); }
    std::filesystem::path filename() { return path.filename(); }
    std::filesystem::path stem() { return path.stem(); }

    uint8_t* data() { return pointer; }
    
    std::error_code flush() {
        map.sync (error);
        return error;
    }

    // path: the path of the save file
    // size: Size of the save file
    // autoCreate: Should the file be created if it doesn't exist?
    SaveFile (std::filesystem::path path, size_t size, bool autoCreate = true) : path(path), beeg(size) {
        const bool found = std::filesystem::exists (path);
        
        if (!found && autoCreate)
            makeFile (path, size);

        else if (!found && !autoCreate)
            panic ("Save file not found, and won't be created automatically\n");

        else if (found && std::filesystem::file_size(path) != size)
            panic ("Save file found, but it's not the expected size\n");

        map = mio::make_mmap_sink(path.string(), 0, mio::map_entire_file, error);
        pointer = (uint8_t*) &map[0];
    }

    SaveFile (std::string& path, size_t size, bool autoCreate = true) {
        SaveFile (std::filesystem::current_path() / path, size, autoCreate);
    }

    SaveFile() {} // Empty file. exists() will return false

    uint8_t at (size_t index) {
        if (index >= beeg) panic ("Out of bounds save file access\n");
        return pointer[index];
    }

    uint8_t& operator[](size_t index) {
        return pointer[index];
    }
};