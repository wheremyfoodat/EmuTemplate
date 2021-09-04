#include "MyEmulator.hpp"
#include "fmt/format.h"

void MyEmulator::runFrame() {
    while (chip.cycles < cyclesPerFrame) {
        chip.execute();
    }
}