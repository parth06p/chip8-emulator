#include "chip8.h"
#include <fstream>
#include <iostream>
#include <iomanip>
const std::array<uint8_t, 80> font_arr = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8(){
    reset(0);

}

void Chip8::reset(uint32_t seed){
    memory_.fill(0);
    V_.fill(0);
    stack_.fill(0);
    display_.fill(0);
    keypad_.fill(false);
    I_ = 0;
    sp_ = 0;
    delay_timer_ = 0;
    sound_timer_ = 0;
    pc_ = program_start_address;
    rng_.seed(seed);
    for(size_t i = 0; i < font_arr.size(); ++i){
        memory_[font_start_address+ i] = font_arr[i];
    }
}

bool Chip8::loadRom(const std::string& path){
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;
    
    if(!file.is_open()){
        return false;
    }
    std::streamsize size = file.tellg();
    if(size < 0) return false;
    if(size > memory_size - program_start_address) return false;
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&memory_[program_start_address]), size);
    return true;
}

void Chip8::setKey(int index, bool pressed){
    if(index >= 0 && index < 16){
        keypad_[index] = pressed;
    }
}

bool Chip8::cycle(){
    uint8_t high = memory_[pc_];
    uint8_t low = memory_[pc_ + 1];
    static_cast<uint16_t>(high);
    static_cast<uint16_t>(high) << 8;
    (static_cast<uint16_t>(high) << 8) | low;
    uint16_t opcode = (static_cast<uint16_t>(high) << 8) | low;
    pc_ += 2;

    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);
    uint8_t nn = (opcode & 0x00FF);
    uint16_t nnn = (opcode & 0x0FFF);

    switch (opcode & 0xF000){

        default:
            std::cerr << "Unkown opcode: " << std::hex << std::setw(4) << std::setfill('0') << opcode << "\n";
            return false;
    }
    return true;
}