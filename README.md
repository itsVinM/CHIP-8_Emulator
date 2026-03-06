
# CHIP-8 Emulator

![C++](https://img.shields.io/badge/C++17-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=flat&logo=cmake&logoColor=white)
![SDL2](https://img.shields.io/badge/SDL2-121013?style=flat&logo=sdl&logoColor=red)
![Build](https://img.shields.io/badge/build-passing-brightgreen)

Cycle-accurate CHIP-8 virtual machine in C++17. Built as the **first validation target** of a portable hardware-software test framework — the same test architecture later used for Game Boy (Rust), STM32F401RE (embedded Rust), and PX4 SITL fault injection.


<img width="627" height="344" alt="Screenshot 2025-10-05 at 11 39 47" src="https://github.com/user-attachments/assets/94ea7b6f-f52b-4097-ac21-542e92f48505" />



The emulator is the device under test. The goal was not just emulation correctness, but establishing an adapter pattern where the test framework never needs to change when a new target is added.

---

## Architecture

```
ValidationFramework
└── CHIP8Target              ← adapter (ports and adapters pattern)
    └── CHIP8Core
        ├── cpu.cpp          ← fetch / decode / execute, 35 opcodes
        ├── memory.cpp       ← 4KB address space, stack (16 levels)
        ├── display.cpp      ← 64×32 framebuffer, XOR sprite rendering
        └── timers.cpp       ← delay + sound timers, 60Hz decrement
```

`CHIP8Target` implements the same `ValidationTarget` interface as `GameBoyTarget` and `STM32Target`. Adding a new target means writing a new adapter — the test suite and framework stay unchanged.

---

## Technical details

| Property | Value |
|---|---|
| ISA | 8-bit, 35 opcodes |
| RAM | 4KB (0x000–0xFFF) |
| Registers | V0–VF (16 × 8-bit), I, PC, SP |
| Stack | 16 levels, overflow → defined fault state |
| Display | 64×32 monochrome, XOR collision detection |
| Timers | Delay + sound @ 60Hz |
| Renderer | SDL2 |

**VF dual-role** — VF is both a general-purpose register and the carry/borrow/collision flag. Opcode 8XY4 (ADD with carry) must set VF = 1 on overflow without corrupting unrelated register state. This is tested explicitly.

**Stack overflow** — at depth 17 the system transitions to a defined fault state rather than undefined behavior. This mirrors the safe-state FSM pattern in the STM32 self-health project.

**Memory boundary** — program space starts at 0x200. Access into the 0x000–0x1FF interpreter-reserved region is tested for correct fault behavior.

---

## Build

**Requirements:** CMake 3.15+, C++17 compiler, SDL2

```bash
git clone https://github.com/itsVinM/CHIP-8_Emulator.git
cd CHIP-8_Emulator
mkdir build && cd build
cmake ..
make
```

---

## Run

```bash
./chip8 <DELAY_CYCLES> <SCALE_FACTOR> <PATH_TO_ROM>

# Example
./chip8 10 2 ../rom/chip8-logo.ch8
```

| Parameter | Description |
|---|---|
| `DELAY_CYCLES` | CPU cycle delay — 10 ≈ 700 instructions/sec |
| `SCALE_FACTOR` | Display scale multiplier — 2 = 128×64 window |
| `PATH_TO_ROM` | Path to `.ch8` ROM file |

---

## Tests

```bash
cmake .. -DBUILD_TESTS=ON
make
ctest --output-on-failure
```

| Suite | What it covers |
|---|---|
| CPU opcodes | All 35 opcodes, happy path + carry/borrow edge cases |
| Stack | CALL/RET roundtrip, overflow at depth 17 |
| Memory | Boundary access, reserved space behavior |
| Display | XOR collision flag, sprite clipping at screen edges |
| Fault injection | Register corruption mid-execution, bad PC values |

---

## Tested ROMs

| ROM | Status |
|---|---|
| chip8-logo.ch8 | ✅ |
| IBM Logo | ✅ |
| test_opcode.ch8 | ✅ |
| Pong | ✅ |

---

## Framework context

Part of a multi-target validation framework. Each target implements the same interface — fault injection, structured reporting, and test isolation are consistent across all targets.

| Target | Language | Status |
|---|---|---|
| CHIP-8 | C++17 + Google Test | ✅ Complete |
| Game Boy LR35902 | Rust + proptest | 🔄 In progress |
| STM32F401RE | Rust embedded + CLI | 🔄 In progress |
| Digital Analyzer | Rust + SCPI | 🔄 In progress |
| PX4 SITL | Python + MAVLink | 📋 Planned |