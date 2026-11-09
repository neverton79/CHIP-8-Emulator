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

Chip8::Chip8()
{
    // default
}

Chip8::~Chip8() 
{
    // default
}

/**
* Resets the registers, memory, and display state to hardware defaults.
*/
void Chip8::initialize()
{
    m_addressI = 0;
    m_programCounter = 0x200;
    m_opCode = 0;
    m_stackPointer = 0;

    for (int i = 0; i < 64 * 32; i++)
        gfx[i] = 0;

    for (int i = 0; i < 16; i++)
        m_stack[i] = 0;

    for (int i = 0; i < 16; i++)
        m_registers[i] = 0;

    for (int i = 0; i < 4096; i++)
        m_gameMemory[i] = 0;

    for (int i = 0; i < 80; i++)
        m_gameMemory[i] = CHIP8_FONTSET[i];

    srand(time(NULL));

    m_delayTimer = 0;
    m_soundTimer = 0;
}

/**
* Draws a sprite at coordinate (x, y) that has a width of 8 pixels and height of "height" pixels
* @param x The x position of the coordinate
* @param y The y position of the coordinate
* @param height The height of the sprite being drawn
*/
void Chip8::drawAt(unsigned char x, unsigned char y, unsigned char height)
{
    unsigned char pixel;
    m_registers[0xF] = 0;

    for (int ypos = 0; ypos < height; ypos++)
    {
        int currentY = (y + ypos);

        if (currentY >= 32)
            break;

        pixel = m_gameMemory[m_addressI + ypos];

        for (int xpos = 0; xpos < 8; xpos++)
        {
            int currentX = (x + xpos);

            if (currentX >= 64)
                break;

            if ((pixel & (0x80 >> xpos)) != 0)
            {
                if (gfx[(currentY * 64) + currentX] == 1)
                {
                    m_registers[0xF] = 1;
                }
                gfx[(currentY * 64) + currentX] ^= 1;
            }
        }
    }
    drawFlag = true;
}

/**
* Fetches, decodes, and executes the opcode. Updates timers.
*/
void Chip8::emulateCycle()
{
    m_opCode = m_gameMemory[m_programCounter] << 8 | m_gameMemory[m_programCounter + 1];

    m_programCounter += 2;

    switch (m_opCode & 0xF000) 
    {
        case 0x0000: 
        {
            switch (m_opCode & 0x000F)
            {
                case 0x0000: 
                {
                    for (int i = 0; i < 64 * 32; ++i)
                        gfx[i] = 0;

                    drawFlag = true;
                    break;
                }
                
                case 0x000E: 
                {
                    m_stackPointer--;
                    m_programCounter = m_stack[m_stackPointer];
                    break;
                }

                default: 
                {
                    printf("Unknown 0x0000 opcode: 0x%X\n", m_opCode);
                }
            }
            break;
        }
        
        case 0x1000:
        {
            m_programCounter = m_opCode & 0x0FFF;
            break;
        }

        case 0x2000:
        {
            m_stack[m_stackPointer] = m_programCounter;
            m_stackPointer++;
            m_programCounter = m_opCode & 0x0FFF;
            break;  
        }

        case 0x3000:
        {
            unsigned char vx = (m_opCode & 0x0F00) >> 8;
            unsigned char nn = m_opCode & 0x00FF;

            if (m_registers[vx] == nn)
                m_programCounter += 2;

            break;
        }

        case 0x4000:
        {
            unsigned char vx = (m_opCode & 0x0F00) >> 8;
            unsigned char nn = m_opCode & 0x00FF;

            if (m_registers[vx] != nn)
                m_programCounter += 2;

            break;
        }

        case 0x5000:
        {
            unsigned char vx = (m_opCode & 0x0F00) >> 8;
            unsigned char vy = (m_opCode & 0x00F0) >> 4;

            if (m_registers[vx] == m_registers[vy])
                m_programCounter += 2;

            break;
        }

        case 0x6000:
        {
            unsigned char vx = (m_opCode & 0x0F00) >> 8;
            unsigned char nn = m_opCode & 0x00FF;

            m_registers[vx] = nn;
            break;
        }

        case 0x7000:
        {
            unsigned char vx = (m_opCode & 0x0F00) >> 8;
            unsigned char nn = m_opCode & 0x00FF;

            m_registers[vx] += nn;
            break;
        }

        case 0x8000:
        {
            switch (m_opCode & 0x000F)
            {
                case 0x0000:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    unsigned char vy = (m_opCode & 0x00F0) >> 4;

                    m_registers[vx] = m_registers[vy];
                    break;
                }

                case 0x0001:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    unsigned char vy = (m_opCode & 0x00F0) >> 4;

                    m_registers[vx] |= m_registers[vy];
                    break;
                }

                case 0x0002:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    unsigned char vy = (m_opCode & 0x00F0) >> 4;

                    m_registers[vx] &= m_registers[vy];
                    break;
                }

                case 0x0003:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    unsigned char vy = (m_opCode & 0x00F0) >> 4;

                    m_registers[vx] ^= m_registers[vy];
                    break;
                }

                case 0x0004:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    unsigned char vy = (m_opCode & 0x00F0) >> 4;
                    unsigned char carry = (m_registers[vx] > (0xFF - m_registers[vy])) ? 1 : 0;
                    
                    m_registers[vx] += m_registers[vy];
                    m_registers[0xF] = carry;
                    break;
                }

                case 0x0005:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    unsigned char vy = (m_opCode & 0x00F0) >> 4;
                    unsigned char carry = (m_registers[vx] >= m_registers[vy]) ? 1 : 0;
                    
                    m_registers[vx] -= m_registers[vy];
                    m_registers[0xF] = carry;
                    break;
                }

                case 0x0006:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    unsigned char val = m_registers[vx] & 0x1;
                    m_registers[vx] >>= 1;
                    m_registers[0xF] = val;
                    break;
                }

                case 0x0007:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    unsigned char vy = (m_opCode & 0x00F0) >> 4;
                    unsigned char carry = (m_registers[vy] >= m_registers[vx]) ? 1 : 0;

                    m_registers[vx] = m_registers[vy] - m_registers[vx];
                    m_registers[0xF] = carry;
                    break;
                }

                case 0x000E:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    unsigned char val = (m_registers[vx] & 0x80) >> 7;
                    m_registers[vx] <<= 1;
                    m_registers[0xF] = val;
                    break;
                }

                default: 
                {
                    printf("Unknown 0x8000 opcode: 0x%X\n", m_opCode);
                }
            }
            break;
        }

        case 0x9000:
        {
            unsigned char vx = (m_opCode & 0x0F00) >> 8;
            unsigned char vy = (m_opCode & 0x00F0) >> 4;

            if (m_registers[vx] != m_registers[vy])
                m_programCounter += 2;

            break;
        }

        case 0xA000:
        {
            unsigned short nnn = m_opCode & 0x0FFF;
            m_addressI = nnn;
            break;
        }

        case 0xB000:
        {
            unsigned short nnn = m_opCode & 0x0FFF;
            m_programCounter = m_registers[0x0] + nnn;
            break;
        }

        case 0xC000:
        {
            unsigned char vx = (m_opCode & 0x0F00) >> 8;
            unsigned char nn = m_opCode & 0x00FF;

            m_registers[vx] = (rand() & 0xFF) & nn;
            break;
        }

        case 0xD000:
        {
            unsigned char vx = (m_opCode & 0x0F00) >> 8;
            unsigned char vy = (m_opCode & 0x00F0) >> 4;
            unsigned char height = m_opCode & 0x000F;

            drawAt(m_registers[vx], m_registers[vy], height);
            break;
        }

        case 0xE000:
        {
            unsigned char vx = (m_opCode & 0x0F00) >> 8;
            unsigned char keyOp = m_opCode & 0x00FF;

            if (keyOp == 0x9E)
            {
                if (key[m_registers[vx]] != 0)
                    m_programCounter += 2;
            }
            else if (keyOp == 0xA1) 
            {
                if (key[m_registers[vx]] == 0)
                    m_programCounter += 2;
            }
            else
            {
                printf("Unknown 0xE000 opcode: 0x%X\n", m_opCode);
            }
            break;
        }

        case 0xF000:
        {
            switch (m_opCode & 0x00FF)
            {
                case 0x0007:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    m_registers[vx] = m_delayTimer;
                    break;
                }

                case 0x000A:
                {
                    bool keyPress = false;
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;

                    for (int i = 0; i < 16; i++)
                    {
                        if (key[i] != 0)
                        {
                            m_registers[vx] = i;
                            keyPress = true;
                            break;
                        }
                    }

                    if (!keyPress)
                        m_programCounter -= 2;

                    break;
                }

                case 0x0015:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    m_delayTimer = m_registers[vx];
                    break;
                }

                case 0x0018:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    m_soundTimer = m_registers[vx];
                    break;
                }

                case 0x001E:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    m_addressI += m_registers[vx];
                    break;
                }

                case 0x0029:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;
                    unsigned char character = m_registers[vx] & 0x0F;

                    m_addressI = character * 5;
                    break;
                }

                case 0x0033:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;

                    unsigned char value = m_registers[vx];

                    unsigned char hundreds = value / 100;
                    unsigned char tens = (value / 10) % 10;
                    unsigned char ones = value % 10;

                    m_gameMemory[m_addressI] = hundreds;
                    m_gameMemory[m_addressI + 1] = tens;
                    m_gameMemory[m_addressI + 2] = ones;

                    break;
                }

                case 0x0055:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;

                    for (unsigned char i = 0; i <= vx; i++) 
                    {
                        m_gameMemory[m_addressI + i] = m_registers[i];
                    }
                    break;
                }

                case 0x0065:
                {
                    unsigned char vx = (m_opCode & 0x0F00) >> 8;

                    for (unsigned char i = 0; i <= vx; i++) 
                    {
                        m_registers[i] = m_gameMemory[m_addressI + i];
                    }
                    break;
                }

                default:
                {
                    printf("Unknown 0xF000 opcode: 0x%X\n", m_opCode);
                }
            }
            break;
        }

        default:
        {
            printf("Unknown opcode: 0x%X\n", m_opCode);
        }
    }
}

/**
* Decrements the delay and sound timers.
*/
void Chip8::decrementTimers()
{
    if (m_delayTimer > 0)
        m_delayTimer--;
    if (m_soundTimer > 0)
        m_soundTimer--;
}

/**
* Loads a ROM file into memory.
*
* @param filename Path to the .ch8 ROM file
* @return true if the ROM was successfully loaded; false otherwise.
*/
bool Chip8::loadROM(const char *filename)
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