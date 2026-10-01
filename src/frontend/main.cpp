#include <SDL.h>
#include <iostream>
using namespace std;
#include "chip8.h"

const int scale = 10;
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

    

    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        cerr << "SDL_INIT_FAILED" << SDL_GetError() << '\n';
        return 1;
    }
    //create window
    SDL_Window* win = SDL_CreateWindow("CHIP-8",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       display_width * scale, display_height * scale, 0);
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
    bool emulating = true;
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
        
        
        if(emulating){
            for (int i = 0; i < 11; ++i) {
                if (!chip.cycle()) {
                    emulating = false;
                    break;
                }
            }
        }
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderClear(rend);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        const Display& screen = chip.getDisplay();
        for(uint16_t y = 0; y < display_height; ++y){
            for(uint16_t x = 0; x < display_width; ++x){
                if(screen[y*display_width + x]){
                    SDL_Rect rect{x * scale, y * scale, scale, scale};
                    SDL_RenderFillRect(rend, &rect);

                }
            }
        }
        SDL_RenderPresent(rend);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}