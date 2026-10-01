#include <SDL.h>
#include <iostream>
using namespace std;
#include "chip8.h"

int main(int argc, char* argv[]){
    // initialization
    // a. Create a Chip8 object
    Chip8 chip;

    // b. Load the ROM; stop if it fails
    if (!chip.loadRom("roms/2-ibm-logo.ch8")) {
        std::cerr << "Failed to load ROM\n";
        return 1;
    }

    // c. Print the first four bytes of the program in hex
    for (uint16_t addr = 0x200; addr < 0x204; ++addr) {
        std::cout << std::hex << static_cast<int>(chip.readMemory(addr)) << " ";
    }
    std::cout << "\n";
    while(chip.cycle()){
        
    }
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        cerr << "SDL_INIT_FAILED" << SDL_GetError() << '\n';
        return 1;
    }
    //create window
    SDL_Window* win = SDL_CreateWindow("CHIP-8",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       640, 320, 0);
    if(win == nullptr){
        cerr << "SDL_CreateWindow_FAILED: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }
    //render window
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if(rend == nullptr){
        cerr << "SDL_Rendering_FAILED: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(win);
        SDL_Quit(); 
        return 1;
    }
    //while loop for running
    bool running = true;
    SDL_Event event; // event used for handling user input and system message
    while(running){
        while (SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderClear(rend);
        SDL_RenderPresent(rend);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}