#include "chip8.h"
#include <SDL3/SDL.h>
#include <iostream>

Chip8 myChip8;
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 320;

int main(int argc, char **argv)
{
    std::cout << "Emulator starting..." << std::endl;

    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        std::cerr << "Failed to initialize SDL.\n";
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) 
    {
        std::cerr << "Failed to create SDL window.\n";
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) 
    {
        std::cerr << "Failed to create SDL renderer.\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (!texture) 
    {
        std::cerr << "Failed to create SDL texture.\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    bool running = true;
    SDL_Event event;
    uint32_t pixels[64 * 32];

    myChip8.initialize();

    if (argc < 2) {
    std::cerr << "Usage: ./chip8_emulator <path_to_rom>\n";
    return -1;
    }

    if (!myChip8.loadROM(argv[1])) {
        std::cerr << "Failed to load ROM file: " << argv[1] << "\n";
        return -1;
    }

    while (running) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_EVENT_QUIT) 
            {
                running = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) 
            {
                bool isKeyDown = (event.type == SDL_EVENT_KEY_DOWN);
                switch (event.key.scancode) 
                {
                    case SDL_SCANCODE_1: myChip8.key[0x1] = isKeyDown; break;
                    case SDL_SCANCODE_2: myChip8.key[0x2] = isKeyDown; break;
                    case SDL_SCANCODE_3: myChip8.key[0x3] = isKeyDown; break;
                    case SDL_SCANCODE_4: myChip8.key[0xC] = isKeyDown; break;
                    case SDL_SCANCODE_Q: myChip8.key[0x4] = isKeyDown; break;
                    case SDL_SCANCODE_W: myChip8.key[0x5] = isKeyDown; break;
                    case SDL_SCANCODE_E: myChip8.key[0x6] = isKeyDown; break;
                    case SDL_SCANCODE_R: myChip8.key[0xD] = isKeyDown; break;
                    case SDL_SCANCODE_T: myChip8.key[0x7] = isKeyDown; break;
                    case SDL_SCANCODE_S: myChip8.key[0x8] = isKeyDown; break;
                    case SDL_SCANCODE_D: myChip8.key[0x9] = isKeyDown; break;
                    case SDL_SCANCODE_F: myChip8.key[0xE] = isKeyDown; break;
                    case SDL_SCANCODE_Z: myChip8.key[0xA] = isKeyDown; break;
                    case SDL_SCANCODE_X: myChip8.key[0x0] = isKeyDown; break;
                    case SDL_SCANCODE_C: myChip8.key[0xB] = isKeyDown; break;
                    case SDL_SCANCODE_V: myChip8.key[0xF] = isKeyDown; break;
                }
            }
        }

        for (int i = 0; i < 10; ++i)
            myChip8.emulateCycle();

        myChip8.decrementTimers();

        if (myChip8.drawFlag) 
        {
            for (int i = 0; i < 64 * 32; ++i) 
            {
                pixels[i] = myChip8.gfx[i] ? 0xFFFFFFFF : 0x00000000;
            }

            SDL_RenderClear(renderer);
            SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));
            SDL_RenderTexture(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);

            myChip8.drawFlag = false;
        }

        SDL_Delay(16); // ~60Hz
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}