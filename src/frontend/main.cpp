// 1. Include SDL
#include <SDL.h>
#include <iostream>
using namespace std;

// int main(int argc, char* argv[]) {
int main(int argc, char* argv[]){
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        cerr << "SDL_INIT_FAILED" << SDL_GetError() << '\n';
        return 1;
    }
    SDL_Window* win = SDL_CreateWindow("CHIP-8",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       640, 320, 0);
    if(win == nullptr){
        cerr << "SDL_CreateWindow_FAILED: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if(rend == nullptr){
        cerr << "SDL_Rendering_FAILED: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(win);
        SDL_Quit(); 
        return 1;
    }
    bool running = true;
    SDL_Event event;
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

    // 2. Initialise SDL's video system. If it fails, print the error and return 1.

    // 3. Create a 640x320 window titled "CHIP-8". If it fails, print the error,
    //    shut down SDL, and return 1.

    // 4. Create a renderer for that window (you'll draw pixels with it later).

    // 5. Main loop: keep running until the user closes the window.
    //    a. Handle all pending events. If one is a quit event, stop running.
    //    b. Fill the screen black and show it.
    //    c. Wait about 16 ms, so the loop runs ~60 times per second
    //       instead of using 100% of your CPU.

    // 6. Clean up: destroy the renderer, destroy the window, shut down SDL.

    // return 0;
// }