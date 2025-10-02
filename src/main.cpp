#include "Chip8.hpp"
#include "Platform.hpp"
#include <chrono>
#include <iostream>

#include "Chip8.hpp"
#include "Platform.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <stdexcept>

// Number of required command-line arguments
const int REQUIRED_ARGS = 4;

void runEmulator(const char* romFilename, int videoScale, int cycleDelay)
{
    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);
    Chip8 chip8;
    chip8.LoadROM(romFilename);

    const int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit)
    {
        quit = platform.ProcessInput(chip8.keypad);

        const auto currentTime = std::chrono::high_resolution_clock::now();
        const auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastCycleTime);

        if (timeElapsed.count() > cycleDelay)
        {
            lastCycleTime = currentTime;
            
            // Execute multiple cycles per frame
            for (int i = 0; i < cyclesPerFrame; ++i) {
                chip8.Cycle();
            }

            platform.Update(chip8.video, videoPitch);
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != REQUIRED_ARGS)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        return EXIT_FAILURE;
    }

    int videoScale{};
    int cycleDelay{};

    try
    {
        videoScale = std::stoi(argv[1]);
        cycleDelay = std::stoi(argv[2]);
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Invalid arguments: Scale and Delay must be integers.\n";
        return EXIT_FAILURE;
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "Arguments out of range.\n";
        return EXIT_FAILURE;
    }

    const char* romFilename = argv[3];
    runEmulator(romFilename, videoScale, cycleDelay);

    return 0;
}