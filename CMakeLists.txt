cmake_minimum_required(VERSION 3.16)
project(chip8 CXX C)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download and build SDL2 automatically
include(FetchContent)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
FetchContent_Declare(SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.30.9)
FetchContent_MakeAvailable(SDL2)

# Emulator core: no SDL dependency
add_library(chip8_core src/core/chip8.cpp)
target_include_directories(chip8_core PUBLIC src/core)

# SDL frontend
add_executable(chip8 src/frontend/main.cpp)
target_link_libraries(chip8 PRIVATE chip8_core SDL2::SDL2-static)
if(TARGET SDL2::SDL2main)
  target_link_libraries(chip8 PRIVATE SDL2::SDL2main)
endif()