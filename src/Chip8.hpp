#pragma once

#include <cstdint>
#include <random>
#include <map>

const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8
{
public:
    Chip8();
    void LoadROM(char const* filename);
    void Cycle();
    using OpcodeFunc = void (Chip8::*)();
    std::map<uint16_t, OpcodeFunc> opcodeMap;

    uint8_t keypad[KEY_COUNT]{};
    uint32_t video[VIDEO_WIDTH*VIDEO_HEIGHT]{};

private:
    // Opcode instruction tables
    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];

    // Helper functions for opcode decoding
    void Table0();
    void Table8();
    void TableE();
    void TableF();

    // Individual opcode functions
    void OP_NULL();      // Do nothing
    void OP_00E0();      // CLS
    void OP_00EE();      // RET
    void OP_1nnn();      // JP address
    void OP_2nnn();      // CALL address
    void OP_3xkk();      // SE Vx, byte
    void OP_4xkk();      // SNE Vx, byte
    void OP_5xy0();      // SE Vx, Vy
    void OP_6xkk();      // LD Vx, byte
    void OP_7xkk();      // ADD Vx, byte
    void OP_8xy0();      // LD Vx, Vy
    void OP_8xy1();      // OR Vx, Vy
    void OP_8xy2();      // AND Vx, Vy
    void OP_8xy3();      // XOR Vx, Vy
    void OP_8xy4();      // ADD Vx, Vy
    void OP_8xy5();      // SUB Vx, Vy
    void OP_8xy6();      // SHR Vx
    void OP_8xy7();      // SUBN Vx, Vy
    void OP_8xyE();      // SHL Vx
    void OP_9xy0();      // SNE Vx, Vy
    void OP_Annn();      // LD I, address
    void OP_Bnnn();      // JP V0, address
    void OP_Cxkk();      // RND Vx, byte
    void OP_Dxyn();      // DRW Vx, Vy, height
    void OP_Ex9E();      // SKP Vx
    void OP_ExA1();      // SKNP Vx
    void OP_Fx07();      // LD Vx, DT
    void OP_Fx0A();      // LD Vx, K
    void OP_Fx15();      // LD DT, Vx
    void OP_Fx18();      // LD ST, Vx
    void OP_Fx1E();      // ADD I, Vx
    void OP_Fx29();      // LD F, Vx
    void OP_Fx33();      // LD B, Vx
    void OP_Fx55();      // LD [I], Vx
    void OP_Fx65();      // LD Vx, [I]

    // CPU state and memory
    uint8_t memory[MEMORY_SIZE]{};
    uint8_t registers[REGISTER_COUNT]{};
    uint16_t index{};
    uint16_t pc{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint16_t stack[STACK_LEVELS]{};
    uint8_t sp{};
    uint16_t opcode{};
    
    // Random number generation
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
};
