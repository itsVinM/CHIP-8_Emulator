Hello there 👋

This is my first emulator, the CHIP-8 writen in C++.

There is still work to do though, like refactoring and making more rom to work.


<img width="627" height="344" alt="Screenshot 2025-10-05 at 11 39 47" src="https://github.com/user-attachments/assets/94ea7b6f-f52b-4097-ac21-542e92f48505" />


## Get the project files
```bash
git clone https://github.com/itsVinM/CHIP-8_Emulator.git
```
## Navigate into the project directory
```bash
cd CHIP-8_Emulator
```
## Create and enter the build directory 
```bash
mkdir build
cd build
```

## Configure the build files
```bahs
cmake ..
```

## Compile the executable (creates the 'chip8' binary)
```bahs
make

./chip8 <DELAY_CYCLES> <SCALE_FACTOR> <PATH_TO_ROM>
```
## Example

To launch the emulator with a delay cycle of 10, a display scale of 2, and loading the included chip8-logo.ch8 ROM:

``` bash
cd build 
./chip8 10 2 ../rom/chip8-logo.ch8
```
