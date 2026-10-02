#include <SDL.h>
#include <iostream>
#include "chip8.h"
#include <string>
using namespace std;
const int scale = 10;
int mapKey(SDL_Keycode key) {
    switch (key) {
        case SDLK_1: return 0x1;
        case SDLK_2: return 0x2;
        case SDLK_3: return 0x3;
        case SDLK_4: return 0xC;

        case SDLK_q: return 0x4;
        case SDLK_w: return 0x5;
        case SDLK_e: return 0x6;
        case SDLK_r: return 0xD;

        case SDLK_a: return 0x7;
        case SDLK_s: return 0x8;
        case SDLK_d: return 0x9;
        case SDLK_f: return 0xE;

        case SDLK_z: return 0xA;
        case SDLK_x: return 0x0;
        case SDLK_c: return 0xB;
        case SDLK_v: return 0xF;

        default: return -1;
    }
}
void audioCallback(void* userdata, Uint8* stream, int len) {
    int* phase = static_cast<int*>(userdata);
    int16_t* samples = reinterpret_cast<int16_t*>(stream);
    int count = len / 2;

    for (int i = 0; i < count; ++i) {
        samples[i] = ((*phase / 50) % 2 == 0) ? 3000 : -3000;
        ++(*phase);
    }
}
int main(int argc, char* argv[]){
    if (argc < 2) {
        std::cerr << "Usage: chip8 <rom file>\n";
        return 1;
    }
    

    Chip8 chip;
    if (!chip.loadRom(argv[1])) {
        std::cerr << "Failed to load ROM: " << argv[1] << "\n";
        return 1;
    }
    if (argc >= 3 && std::string(argv[2]) == "--classic") {
        chip.setQuirks(Quirks::classic());
    }
    
    // initialization
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) != 0){
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
    int phase = 0;
    SDL_AudioSpec want{};
    want.freq = 44100;
    want.format = AUDIO_S16SYS; 
    want.channels = 1;
    want.samples = 512;
    want.callback = audioCallback;
    want.userdata = &phase;

    SDL_AudioDeviceID audio = SDL_OpenAudioDevice(nullptr, 0, &want, nullptr, 0);
    if (audio == 0) {
        std::cerr << "Audio failed: " << SDL_GetError() << " (continuing without sound)\n";
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
                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                        break;
                    }
                    int key = mapKey(event.key.keysym.sym);
                    if (key != -1) {
                        chip.setKey(key, true);
                    }
                    break;
                }
                case SDL_KEYUP: {
                    int key = mapKey(event.key.keysym.sym);
                    if (key != -1) {
                        chip.setKey(key, false);
                    }
                    break;
                }
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
            chip.tickTimers();
            if (audio != 0) {
                SDL_PauseAudioDevice(audio, chip.isSoundPlaying() ? 0 : 1);
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
    if (audio != 0) {
        SDL_CloseAudioDevice(audio);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}