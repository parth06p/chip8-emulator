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
    waitingKey_ = -1;
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
void Chip8::tickTimers() {
    if (delay_timer_ > 0) {
        --delay_timer_;
    }
    if (sound_timer_ > 0) {
        --sound_timer_;
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
        case 0x0000:
            if(opcode == 0x00E0){
                display_.fill(0);
            }
            else if (opcode == 0x00EE){
                if(sp_ == 0) return false;
                sp_--;
                pc_ = stack_[sp_];
            }
            else{
                std::cerr << "unknown opcode: " << std::hex << std::setw(4) << std::setfill('0') << opcode << "\n";
                return false;
            }
            break;
        case 0xA000:
            I_ = nnn;
            break;
        case 0x6000:
            V_[x] = nn;
            break;
        case 0x7000:
            V_[x] += nn;
            break;
        case 0x1000:
            pc_ = nnn;
            break;
        case 0xD000:{
            uint16_t startX = V_[x] % display_width;
            uint16_t startY = V_[y] % display_height;
            V_[0xF] = 0;
            for(int row = 0; row < n; ++row){
                uint16_t py = startY     + row;
                if(py >= display_height){
                    break;
                }
                uint16_t spriteByte = memory_[I_ + row];
                for(int col = 0; col < 8; ++col){
                    uint16_t px = startX + col;
                    if(px >= display_width){
                        break;
                    }
                    if(spriteByte & (0x80 >> col)){
                        uint16_t index = py * display_width + px;
                        if(display_[index] == 1){
                            V_[0xF] = 1;
                        }
                        display_[index]^= 1;
                    }
                }
            }
            break;
        }
        case 0x2000:
            if(sp_ >= 16) return false;
            stack_[sp_] = pc_;
            sp_++;
            pc_ = nnn;
            break;
        case 0x3000:
            if(V_[x] == nn){
                pc_+= 2;
            }
            break;
        case 0x4000:
            if(V_[x] != nn){
                pc_+= 2;
            }
            break;
        case 0x5000:
            if(V_[x] == V_[y] && n == 0){
                pc_+= 2;
            }
            break;
        case 0x9000:
            if(V_[x] != V_[y] && n == 0){
                pc_+= 2;
            }
            break;
        case 0x8000:
            switch(n){
                case 0:
                    V_[x] = V_[y];
                    break;
                case 1:
                    V_[x] = V_[x] | V_[y];
                    V_[0xF] = 0;
                    break;
                case 2:
                    V_[x] = V_[x] & V_[y];
                    V_[0xF] = 0;
                    break;
                case 3:
                    V_[x] = V_[x] ^ V_[y];
                    V_[0xF] = 0;
                    break;
                case 4:{
                    uint16_t sum = V_[x] + V_[y];
                    uint8_t flag = (sum > 255) ? 1 : 0;
                    V_[x] = sum; 
                    V_[0xF] = flag; 
                    break;
                }
                case 5: {
                    uint8_t flag = (V_[x] >= V_[y]) ? 1 : 0;
                    V_[x] = V_[x] - V_[y];
                    V_[0xF] = flag;
                    break;
                }
                case 6: {
                    V_[x] = V_[y];
                    uint8_t flag = V_[x] & 1;
                    V_[x] = V_[x] >> 1;
                    V_[0xF] = flag;
                    break;
                }
                case 7: {
                    uint8_t flag = (V_[y] >= V_[x]) ? 1 : 0;
                    V_[x] = V_[y] - V_[x];
                    V_[0xF] = flag;
                    break;
                }
                case 0xE: {
                    V_[x] = V_[y];
                    uint8_t flag = (V_[x] >> 7) & 1;
                    V_[x] = V_[x] << 1;
                    V_[0xF] = flag;
                    break;
                }
                default:
                    std::cerr << "unknown opcode: " << std::hex << std::setw(4) << std::setfill('0') << opcode << "\n";
                    return false;
            }
            break;
        case 0xB000:
            pc_ = nnn + V_[0];
            break;
        case 0xC000:
            V_[x] = (rng_() & 0xFF) & nn;
            break;  
        case 0xE000:
            switch (nn) {
                case 0x9E:
                    if (keypad_[V_[x] & 0xF]) {
                        pc_ += 2;
                    }
                    break;
                case 0xA1:
                    if (!keypad_[V_[x] & 0xF]) {
                        pc_ += 2;
                    }
                    break;
                default:
                    std::cerr << "Unknown opcode: " << std::hex << std::setw(4)
                            << std::setfill('0') << opcode << "\n";
                    return false;
            }
            break;

        case 0xF000:
            switch (nn) {
                case 0x07:
                    V_[x] = delay_timer_;
                    break;

                case 0x0A: {
                    if (waitingKey_ == -1) {
                        for (int k = 0; k < 16; ++k) {
                            if (keypad_[k]) {
                                waitingKey_ = k;
                                break;
                            }
                        }
                        pc_ -= 2;
                    } else if (keypad_[waitingKey_]) {
                        pc_ -= 2;
                    } else {
                        V_[x] = waitingKey_;
                        waitingKey_ = -1;
                    }
                    break;
                }

                case 0x15:
                    delay_timer_ = V_[x];
                    break;

                case 0x18:
                    sound_timer_ = V_[x];
                    break;

                case 0x1E:
                    I_ = I_ + V_[x];
                    break;

                case 0x29:
                    I_ = font_start_address + (V_[x] & 0xF) * 5;
                    break;

                case 0x33: {
                    uint8_t value = V_[x];
                    memory_[I_]     = value / 100;
                    memory_[I_ + 1] = (value / 10) % 10;
                    memory_[I_ + 2] = value % 10;
                    break;
                }

                case 0x55:
                    for (int i = 0; i <= x; ++i) {
                        memory_[I_ + i] = V_[i];
                    }
                    I_ = I_ + x + 1; //try
                    break;

                case 0x65:
                    for (int i = 0; i <= x; ++i) {
                        V_[i] = memory_[I_ + i];
                    }
                    I_ = I_ + x + 1; //try
                    break;

                default:
                    std::cerr << "Unknown opcode: " << std::hex << std::setw(4)
                            << std::setfill('0') << opcode << "\n";
                    return false;
            }
            break;
        default:
            std::cerr << "unknown opcode: " << std::hex << std::setw(4) << std::setfill('0') << opcode << "\n";
            return false;
    }
    return true;
}