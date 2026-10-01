#pragma once
#include <array> 
#include <random>
#include <string> 
#include <cstdint>

//Constants
const int display_width = 64;
const int display_height = 32;
const uint16_t memory_size = 4096;
const uint16_t program_start_address = 0x200;
const uint16_t font_start_address = 0x050;
using Display = std::array<uint8_t, display_width * display_height>;

class Chip8{
    public:
        Chip8();
        void reset(uint32_t seed);
        bool loadRom(const std::string& path);
        const Display& getDisplay() const{
            return display_;
        }
        void setKey(int index, bool pressed);
        uint8_t readMemory(uint16_t address) const { return memory_[address]; }
        bool cycle();
        void tickTimers();
        bool isSoundPlaying() const { return sound_timer_ > 0; }
    private:
        std::array<uint8_t, memory_size> memory_; //memory
        std::array<uint8_t, 16> V_; //V register
        uint16_t I_; //I register
        uint16_t pc_; //program counter
        std::array<uint16_t, 16> stack_; //stack of retrun address
        uint8_t sp_; //stack pointer
        uint8_t delay_timer_; //delay
        uint8_t sound_timer_; //sound
        Display display_; // screen pixels
        std::array<bool, 16> keypad_; //keys
        std::mt19937 rng_; //random number
};