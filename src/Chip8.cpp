// Chip8.cpp

#include "Chip8.hpp"
#include <memory>
#include <chrono>
#include <cstring>
#include <fstream>
#include <random>
#include <map>

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;

uint8_t fontset[FONTSET_SIZE] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc = START_ADDRESS;

    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    // Initialize the opcode map with all opcodes and their corresponding functions
    opcodeMap = {
        {0x00E0, &Chip8::OP_00E0},
        {0x00EE, &Chip8::OP_00EE},
        {0x1000, &Chip8::OP_1nnn},
        {0x2000, &Chip8::OP_2nnn},
        {0x3000, &Chip8::OP_3xkk},
        {0x4000, &Chip8::OP_4xkk},
        {0x5000, &Chip8::OP_5xy0},
        {0x6000, &Chip8::OP_6xkk},
        {0x7000, &Chip8::OP_7xkk},
        {0x8000, &Chip8::OP_8xy0},
        {0x8001, &Chip8::OP_8xy1},
        {0x8002, &Chip8::OP_8xy2},
        {0x8003, &Chip8::OP_8xy3},
        {0x8004, &Chip8::OP_8xy4},
        {0x8005, &Chip8::OP_8xy5},
        {0x8006, &Chip8::OP_8xy6},
        {0x8007, &Chip8::OP_8xy7},
        {0x800E, &Chip8::OP_8xyE},
        {0x9000, &Chip8::OP_9xy0},
        {0xA000, &Chip8::OP_Annn},
        {0xB000, &Chip8::OP_Bnnn},
        {0xC000, &Chip8::OP_Cxkk},
        {0xD000, &Chip8::OP_Dxyn},
        {0xE09E, &Chip8::OP_Ex9E},
        {0xE0A1, &Chip8::OP_ExA1},
        {0xF007, &Chip8::OP_Fx07},
        {0xF00A, &Chip8::OP_Fx0A},
        {0xF015, &Chip8::OP_Fx15},
        {0xF018, &Chip8::OP_Fx18},
        {0xF01E, &Chip8::OP_Fx1E},
        {0xF029, &Chip8::OP_Fx29},
        {0xF033, &Chip8::OP_Fx33},
        {0xF055, &Chip8::OP_Fx55},
        {0xF065, &Chip8::OP_Fx65}
    };
}

void Chip8::LoadROM(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        std::streampos size = file.tellg();
        auto buffer = std::make_unique<char[]>(size);
        file.seekg(0, std::ios::beg);
        file.read(buffer.get(), size);
        file.close();

        for (long i = 0; i < size; ++i) {
            memory[START_ADDRESS + i] = buffer[i];
        }
    }
}

void Chip8::Cycle() {
    opcode = (memory[pc] << 8u) | memory[pc + 1];
    pc += 2;

    uint16_t lookup_key = opcode & 0xF000;

    switch (lookup_key) {
        case 0x0000:
        {
            switch (opcode) {
                case 0x00E0:
                    (this->*opcodeMap[0x00E0])();
                    break;
                case 0x00EE:
                    (this->*opcodeMap[0x00EE])();
                    break;
                default:
                    // Handle a custom 0nnn opcode if needed
                    break;
            }
        } break;
        case 0x1000: (this->*opcodeMap[0x1000])(); break;
        case 0x2000: (this->*opcodeMap[0x2000])(); break;
        case 0x3000: (this->*opcodeMap[0x3000])(); break;
        case 0x4000: (this->*opcodeMap[0x4000])(); break;
        case 0x5000: (this->*opcodeMap[0x5000])(); break;
        case 0x6000: (this->*opcodeMap[0x6000])(); break;
        case 0x7000: (this->*opcodeMap[0x7000])(); break;
        case 0x8000:
        {
            uint16_t sub_key = opcode & 0x000F;
            auto it = opcodeMap.find(0x8000 | sub_key);
            if (it != opcodeMap.end()) {
                (this->*(it->second))();
            }
        } break;
        case 0x9000: (this->*opcodeMap[0x9000])(); break;
        case 0xA000: (this->*opcodeMap[0xA000])(); break;
        case 0xB000: (this->*opcodeMap[0xB000])(); break;
        case 0xC000: (this->*opcodeMap[0xC000])(); break;
        case 0xD000: (this->*opcodeMap[0xD000])(); break;
        case 0xE000:
        {
            uint16_t sub_key = opcode & 0x00FF;
            auto it = opcodeMap.find(0xE000 | sub_key);
            if (it != opcodeMap.end()) {
                (this->*(it->second))();
            }
        } break;
        case 0xF000:
        {
            uint16_t sub_key = opcode & 0x00FF;
            auto it = opcodeMap.find(0xF000 | sub_key);
            if (it != opcodeMap.end()) {
                (this->*(it->second))();
            }
        } break;
        default:
            // Handle unknown opcode or error
            break;
    }

    if (delayTimer > 0) {
        --delayTimer;
    }
    if (soundTimer > 0) {
        --soundTimer;
    }
}

// All opcode function definitions below. These are the same as your original code.
void Chip8::OP_NULL() {}

void Chip8::OP_00E0() {
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE() {
    --sp;
    pc = stack[sp];
}

void Chip8::OP_1nnn() {
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

void Chip8::OP_2nnn() {
    uint16_t address = opcode & 0x0FFFu;
    stack[sp] = pc;
    ++sp;
    pc = address;
}

void Chip8::OP_3xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    if (registers[Vx] == byte) {
        pc += 2;
    }
}

void Chip8::OP_4xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    if (registers[Vx] != byte) {
        pc += 2;
    }
}

void Chip8::OP_5xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if (registers[Vx] == registers[Vy]) {
        pc += 2;
    }
}

void Chip8::OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] = byte;
}

void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] += byte;
}

void Chip8::OP_8xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = registers[Vx] + registers[Vy];
    registers[0xF] = (sum > 255u);
    registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[0xF] = (registers[Vx] > registers[Vy]);
    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[0xF] = registers[Vx] & 0x1u;
    registers[Vx] >>= 1;
}

void Chip8::OP_8xy7() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[0xF] = (registers[Vy] > registers[Vx]);
    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    registers[Vx] <<= 1;
}

void Chip8::OP_9xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if (registers[Vx] != registers[Vy]) {
        pc += 2;
    }
}

void Chip8::OP_Annn() {
    uint16_t address = opcode & 0x0FFFu;
    index = address;
}

void Chip8::OP_Bnnn() {
    uint16_t address = opcode & 0x0FFFu;
    pc = registers[0] + address;
}

void Chip8::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;
    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row) {
        uint8_t spriteByte = memory[index + row];
        for (unsigned int col = 0; col < 8; ++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];
            if (spritePixel) {
                if (*screenPixel == 0xFFFFFFFF) {
                    registers[0xF] = 1;
                }
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::OP_Ex9E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];
    if (keypad[key]) {
        pc += 2;
    }
}

void Chip8::OP_ExA1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];
    if (!keypad[key]) {
        pc += 2;
    }
}

void Chip8::OP_Fx07() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i < 16; ++i) {
        if (keypad[i]) {
            registers[Vx] = i;
            return;
        }
    }
    pc -= 2;
}

void Chip8::OP_Fx15() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    index += registers[Vx];
}

void Chip8::OP_Fx29() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];
    index = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::OP_Fx33() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];
    memory[index + 2] = value % 10;
    value /= 10;
    memory[index + 1] = value % 10;
    value /= 10;
    memory[index] = value % 10;
}

void Chip8::OP_Fx55() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i <= Vx; ++i) {
        memory[index + i] = registers[i];
    }
}

void Chip8::OP_Fx65() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i <= Vx; ++i) {
        registers[i] = memory[index + i];
    }
}