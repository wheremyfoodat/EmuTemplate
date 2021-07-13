#include "MyEmulator.hpp"
#include "fmt/format.h"

void MyEmulator::step() {
    fmt::print ("Step\n");
}

void MyEmulator::runFrame() {
    fmt::print ("Frame {}\n", framesPassed++);
}