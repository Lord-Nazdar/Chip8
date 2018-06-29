#include "chip8.h"
#include "program-reader.h"
#include <iostream>

int main(int argc, char**argv) {
    // CHIP8 chip8;

    // chip8.Init();
    // while (chip8.Cycle()){}
    ProgramReader pr;
    pr.Load("MAZE.ch8");

    CHIP8 chp;
    for (uint16_t i = 0; i < pr.Program.size(); ++i) {
        chp.memory[0x200+i] = pr.Program[i];
    }

    chp.Init();

    for (int i = 0; i < 1024; ++i) {
        if(!chp.Cycle())
            break;
        std::cin.get();
    }

    chp.PrintScreen();

    return 0;
}
