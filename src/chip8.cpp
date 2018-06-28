#include "chip8.h"
#include <cstring>
#include <iostream>

CHIP8::~CHIP8() {
    for(uint8_t y = 0; y < 32; ++y) {
        for(uint8_t x = 0; x < 64; ++x) {
            if (screen[x][y]) {
                std::cout << "O";
            }
            else {
                std::cout << ".";
            }
        }
        std::cout << "\n";
    }
}

void CHIP8::Init() {
    // Program memory starts at 0x200
    PC = 0x200;

    SP = 0;
    memset(stack, 0x0000, sizeof(uint16_t) * 24);


    // Fill the initial part of the memory
    *memory = [0xF0, 0x90, 0x90, 0x90, 0xF0];
}

bool CHIP8::Cycle() {
    // Read the new operation from the program memory
    uint16_t opcode = (memory[PC] << 8) | memory[PC + 1];
    std::cout << "Opcode is " << std::hex << opcode << std::endl;

    // Decode the opcode
    switch (opcode & 0xF000) {
    case 0x0000:
        {
            switch (opcode & 0x00FF) {
            case 0xE0:
                memset(screen, false, sizeof(bool) * 32 * 64);
                std::cout << "Clearing the display" << std::endl;
                break;
            case 0xEE:
                PC = stack[SP--];
                std::cout << "Returning from a subroutine to " << std::hex << PC << std::endl;
                break;
            default:
                std::cout << "Unknown instruction: 0x" << std::hex << opcode << std::endl;
                return false;
            }
            PC += 2;
            break;
        }
    case 0x1000:
        {
            uint16_t dest = opcode & 0x0FFF;
            std::cout << "Jump to " << std::hex << dest << std::endl;

            PC = dest;
            break;
        }
    case 0x2000:
        {
            uint16_t dest = opcode & 0x0FFF;
            std::cout << "Calling subroutine at " << std::hex << dest << std::endl;

            stack[++SP] = PC;

            PC = dest;
            break;
        }
    case 0x3000:
        {
            uint8_t s = (opcode & 0x0F00) >> 8;
            uint16_t v = opcode & 0x00FF;
            std::cout << "Skip if V" << std::hex << s << " equal " << std::hex << v << std::endl;

            if (V[s] == v) {
                PC += 4;
            }
            else {
                PC += 2;
            }
            break;
        }
    case 0x4000:
        {
            uint8_t s = (opcode & 0x0F00) >> 8;
            uint16_t v = opcode & 0x00FF;
            std::cout << "Skip if V" << std::hex << s << " not equal " << std::hex << v << std::endl;

            if (V[s] != v) {
                PC += 4;
            }
            else {
                PC += 2;
            }
            break;
        }
    case 0x5000:
        {
            uint8_t v1 = (opcode & 0x0F00) >> 8;
            uint8_t v2 = (opcode & 0x00F0) >> 16;
            std::cout << "Skip if V" << std::hex << v1 << " equal V" << std::hex << v2 << std::endl;

            if (V[v1] == V[v2]) {
                PC += 4;
            }
            else {
                PC += 2;
            }
            break;
        }
    case 0x6000:
        {
            uint8_t s = (opcode & 0x0F00) >> 8;
            uint16_t v = opcode & 0x00FF;
            std::cout << "Set V" << std::hex << s << " to " << std::hex << v << std::endl;
            V[s] = v;
            PC += 2;
            break;
        }
    case 0x7000:
        {
            uint8_t s = (opcode & 0x0F00) >> 8;
            uint16_t v = opcode & 0x00FF;
            std::cout << "Add to V" << std::hex << s << std::hex << v << std::endl;
            V[s] += v;
            PC += 2;
            break;
        }
    case 0x8000:
        {
            uint8_t X = (opcode & 0x0F00) >> 8;
            uint8_t Y = (opcode & 0x00F0) >> 4;
            switch (opcode & 0x000F) {
            case 0x00:
                V[X] = V[Y];
                break;
            case 0x01:
                V[X] = V[X] | V[Y];
                break;
            case 0x02:
                V[X] = V[X] & V[Y];
                break;
            case 0x03:
                V[X] = V[X] ^ V[Y];
                break;
            case 0x04:
                V[X] = V[X] + V[Y];
                break;
            case 0x05:
                V[X] = V[X] - V[Y];
                break;
            case 0x06:
                V[X] = V[Y] >> 1;
                break;
            case 0x07:
                V[X] = V[Y] - V[X];
                break;
            case 0x0E:
                V[X] = V[Y] = V[Y] << 1;
                break;
            }
            PC += 2;
            break;
        }
    case 0x9000:
        {
            uint8_t X = (opcode & 0x0F00) >> 8;
            uint8_t Y = (opcode & 0x00F0) >> 4;

            if (V[X] != V[Y]) {
                PC += 4;
            }
            else {
                PC += 2;
            }
            break;
        }
    case 0xA000:
        {
            I = opcode & 0x0FFF;

            PC += 2;
            break;
        }
    case 0xB000:
        {
            PC = V[0] + (opcode & 0x0FFF);
            break;
        }
    case 0xC000:
        {
            uint8_t rand = 73;
            uint8_t X = (opcode & 0x0F00) >> 4;
            uint8_t N = opcode & 0x00FF;

            V[X] = rand & N;
            PC += 2;
            break;
        }
    case 0xD000:
        {
            uint8_t X = (opcode & 0x0F00) >> 8;
            uint8_t Y = (opcode & 0x00F0) >> 4;
            uint8_t N = opcode & 0x000F;

            for(uint8_t y = 0; y < N; ++y) {
                for(uint8_t x = 0; x < 8; ++x) {
                    screen[V[X]+x][V[Y]+y] = !screen[V[X]+x][V[Y]+y];
                 }
            }

            PC += 2;
            break;
        }
    default:
        std::cout << "Unknown instruction: 0x" << std::hex << opcode << std::endl;
        return false;
    }

    return true;
}
