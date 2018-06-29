#pragma once
#include <cstdint>
#include <random>

class CHIP8 {
public:
    CHIP8();


    void PrintScreen();
    void ShowDebug();

    void Init();
    bool Cycle();
    uint8_t memory[4096];

private:
    uint8_t V[15];
    uint8_t VF;

    uint16_t I;
    uint16_t PC;

    uint16_t Opcode;

    bool screen[64][32];

    uint16_t stack[24];
    uint8_t SP;

    // Setup for the random number generator
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<unsigned short> dist;
};
