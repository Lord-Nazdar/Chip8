#include "chip8.h"
#include <cstring>
#include <iostream>
#include "mem.h"

CHIP8::CHIP8() {
    // Init the random device
    mt = std::mt19937(rd());
    dist = std::uniform_int_distribution<unsigned short>(0, 255);
}

void CHIP8::PrintScreen() {
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

void CHIP8::ShowDebug() {
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Current location: 0x0" << std::hex << PC << std::endl;
    std::cout << "Current opcode:   0x" << std::hex << Opcode << std::endl;
    std::cout << "Registers V:" << std::endl;
    std::cout << "0    1    2    3    4    5    6    7    8    9    10    11    12    13    14    F" << std::endl;
    printf("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", V[0], V[1], V[2], V[3], V[4]);
    printf("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ", V[5], V[6], V[7], V[8], V[9]);
    printf("0x%02x  0x%02x  0x%02x  0x%02x  0x%02x  0x%02x\n", V[10], V[11], V[12], V[13], V[14], VF);
    std::cout << "--------------------------------" << std::endl;
}

void CHIP8::Init() {

    // Program memory starts at 0x200
    PC = 0x200;

    SP = 0;
    memset(stack, 0x0000, sizeof(uint16_t) * 24);


    // Fill the initial part of the memory
    uint16_t ptr = 0x00;
    memcpy(&memory[ptr], &font_0, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_1, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_2, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_3, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_4, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_5, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_6, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_7, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_8, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_9, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_A, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_B, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_C, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_D, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_E, FONT_SIZE);
    memcpy(&memory[ptr += 0x05], &font_F, FONT_SIZE);
}

bool CHIP8::Cycle() {
    // Read the new operation from the program memory
   Opcode = (memory[PC] << 8) | memory[PC + 1];


    ShowDebug();

    // Decode the opcode
    switch (Opcode & 0xF000) {
    case 0x0000:
        {
            switch (Opcode & 0x00FF) {
            case 0xE0:
                memset(screen, false, sizeof(bool) * 32 * 64);
                std::cout << "Clearing the display" << std::endl;
                break;
            case 0xEE:
                PC = stack[SP--];
                printf("Returning from a subroutine to 0x%02x\n", PC);
                break;
            default:
                printf("Unknown instruction: 0x%02x\n", Opcode);
                return false;
            }
            PC += 2;
            break;
        }
    case 0x1000:
        {
            uint16_t dest = Opcode & 0x0FFF;
            printf("Jump to 0x%02x\n", dest);

            PC = dest;
            break;
        }
    case 0x2000:
        {
            uint16_t dest = Opcode & 0x0FFF;
            printf("Calling subroutine at 0x%02x\n", dest);

            stack[++SP] = PC;

            PC = dest;
            break;
        }
    case 0x3000:
        {
            uint8_t s = (Opcode & 0x0F00) >> 8;
            uint16_t v = Opcode & 0x00FF;

            printf("Skip if V[%u](0x%02x)=0x%02x\n", s, V[s], v);

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
            uint8_t s = (Opcode & 0x0F00) >> 8;
            uint16_t v = Opcode & 0x00FF;

            printf("Skip if V[%u](0x%02x)!=0x%02x\n", s, V[s], v);

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
            uint8_t v1 = (Opcode & 0x0F00) >> 8;
            uint8_t v2 = (Opcode & 0x00F0) >> 4;

            printf("Skip if V[%u](0x%02x)=V[%u](0x%02x)\n", v1, V[v1], v2, V[v2]);

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
            uint8_t s = (Opcode & 0x0F00) >> 8;
            uint16_t v = Opcode & 0x00FF;

            printf("Set V[%u] to 0x%02x\n", s, v);

            V[s] = v;
            PC += 2;
            break;
        }
    case 0x7000:
        {
            uint8_t s = (Opcode & 0x0F00) >> 8;
            uint16_t v = Opcode & 0x00FF;

            printf("V[%u](0x%02x) += 0x%02x\n", s, V[s], v);

            V[s] += v;
            PC += 2;
            break;
        }
    case 0x8000:
        {
            uint8_t X = (Opcode & 0x0F00) >> 8;
            uint8_t Y = (Opcode & 0x00F0) >> 4;
            switch (Opcode & 0x000F) {
            case 0x00:
                V[X] = V[Y];
                printf("V[%u](0x%02x) = V[%u](0x%02x)\n", X, V[X], Y, V[Y]);
                break;
            case 0x01:
                V[X] = V[X] | V[Y];
                printf("V[%u](0x%02x) | V[%u](0x%02x)\n", X, V[X], Y, V[Y]);
                break;
            case 0x02:
                V[X] = V[X] & V[Y];
                printf("V[%u](0x%02x) & V[%u](0x%02x)\n", X, V[X], Y, V[Y]);
                break;
            case 0x03:
                V[X] = V[X] ^ V[Y];
                printf("V[%u](0x%02x) ^ V[%u](0x%02x)\n", X, V[X], Y, V[Y]);
                break;
            case 0x04:
                V[X] = V[X] + V[Y];
                printf("V[%u](0x%02x) += V[%u](0x%02x)\n", X, V[X], Y, V[Y]);
                break;
            case 0x05:
                V[X] = V[X] - V[Y];
                printf("V[%u](0x%02x) -= V[%u](0x%02x)\n", X, V[X], Y, V[Y]);
                break;
            case 0x06:
                V[X] = V[Y] >> 1;
                printf("V[%u](0x%02x) = V[%u](0x%02x) >> 1\n", X, V[X], Y, V[Y]);
                break;
            case 0x07:
                V[X] = V[Y] - V[X];
                printf("V[%u](0x%02x) = V[%u](0x%02x) - V[%u](0x%02x) \n", X, V[X], Y, V[Y], X, V[X]);
                break;
            case 0x0E:
                V[X] = V[Y] = V[Y] << 1;
                printf("V[%u](0x%02x) = V[%u](0x%02x) = V[%u](0x%02x) << 1\n", X, V[X], Y, V[Y], Y, V[Y]);
                break;
            }
            PC += 2;
            break;
        }
    case 0x9000:
        {
            uint8_t X = (Opcode & 0x0F00) >> 8;
            uint8_t Y = (Opcode & 0x00F0) >> 4;


            printf("Skip if V[%u](0x%02x)!=V[%u](0x%02x)\n", X, V[X], Y, V[Y]);

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
            I = Opcode & 0x0FFF;

            printf("Set I to 0x%02x\n", I);

            PC += 2;
            break;
        }
    case 0xB000:
        {
            PC = V[0] + (Opcode & 0x0FFF);

            printf("Set I to 0x%02x\n", I);

            break;
        }
    case 0xC000:
        {
            uint8_t X = (Opcode & 0x0F00) >> 8;
            uint8_t N = Opcode & 0x00FF;

            V[X] = dist(mt) & N;
            PC += 2;
            break;
        }
    case 0xD000:
        {
            uint8_t X = (Opcode & 0x0F00) >> 8;
            uint8_t Y = (Opcode & 0x00F0) >> 4;
            uint8_t N = Opcode & 0x000F;

            for(uint8_t y = 0; y < N; ++y) {
                for(uint8_t x = 0; x < 8; ++x) {
                    screen[V[X]+x][V[Y]+y] = !screen[V[X]+x][V[Y]+y];
                 }
            }

            PC += 2;
            break;
        }
    default:
        std::cout << "Unknown instruction: 0x" << std::hex << Opcode << std::endl;
        return false;
    }

    return true;
}
