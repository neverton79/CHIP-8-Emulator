#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

unsigned char CHIP8_FONTSET[80] = 
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void Chip8()
{
    // default
}

void ~Chip8() 
{
    // default
}

/**
* Resets the registers, memory, and display state to hardware defaults.
*/
void initialize()
{
    m_addressI = 0;
    m_programCounter = 0x200;
    m_opCode = 0;
    m_stackPointer = 0;

    for (int i = 0; i < 64 * 32; ++i)
        gfx[i] = 0;

    for (int i = 0; i < 16; ++i)
        m_stack[i] = 0;

    for (int i = 0; i < 16; ++i)
        m_registers[i] = 0;

    for (int i = 0; i < 4096; ++i)
        m_gameMemory[i] = 0;

    for (int i = 0; i < 80; ++i)
        m_gameMemory[i] = CHIP8_FONTSET[i];

    m_delayTimer = 0;
    m_soundTimer = 0;
}

void emulateCycle()
{
    // TODO
}

/**
* Loads a ROM file into memory.
*
* @param filepath Path to the .ch8 ROM file
* @return true if the ROM was successfully loaded; false otherwise.
*/
bool loadROM(const char *filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cerr << "Failed to open ROM file.\n";
        return false;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize > (4096 - 512)) {
        std::cerr << "ROM file size exceeds memory capacity.\n";
        fclose(file);
        return false;
    }

    size_t bytesRead = fread(&m_gameMemory[0x200], 1, fileSize, file);

    fclose(file);

    if (bytesRead != fileSize) {
        std::cerr << "READING ERROR\n";
        return false;
    }

    return true;
}