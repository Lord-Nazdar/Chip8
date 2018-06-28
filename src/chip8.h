#pragma once
#include <cstdint>

class CHIP8 {
public:
    void Init();
    bool Cycle();
    uint8_t memory[4096];

private:
    uint8_t V[15];
    uint8_t VF;

    uint16_t I;
    uint16_t PC;

    bool screen[64][32];

    uint16_t stack[24];
    uint8_t SP;
};
