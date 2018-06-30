#pragma once
#include <cstdint>
#include <random>
#include <chrono>

struct CHIP8_INFO {
    uint8_t V[15];      // Working registers
    uint8_t VF;         // Flag register

    uint16_t I;         // Memory pointer
    uint16_t PC;        // Program counter

    uint16_t Opcode;    // Opcode
};

class CHIP8 {
public:
    CHIP8();

    void Tick(const std::chrono::duration<double> &delta);

    const CHIP8_INFO GetInfo() const;

    double Elapsed;
    float ClockSpeed;

    uint8_t Delay;
    uint8_t Sound;

    bool Blocked;


    void PrintScreen();
    void ShowDebug();

    void Init();
    bool Cycle();

    uint8_t memory[4096];
    
    bool Keys[16];

    bool screen[64][32];

private:
    uint8_t V[15];
    uint8_t VF;

    uint16_t I;
    uint16_t PC;

    uint16_t Opcode;

    uint16_t stack[24];
    uint8_t SP;

    // Setup for the random number generator
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<unsigned short> dist;
};