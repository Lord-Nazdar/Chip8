#include "chip8.h"
#include "program-reader.h"

int main(int argc, char**argv) {
    // CHIP8 chip8;

    // chip8.Init();
    // while (chip8.Cycle()){}
    ProgramReader pr;
    pr.Load("ant.chp");

    return 0;
}
