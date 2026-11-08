cmake_minimum_required(VERSION 3.16)
project(chip8_emulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell CMake where to find SDL3 folder
set(SDL3_DIR "${CMAKE_SOURCE_DIR}/libs/SDL3-3.4.18/cmake")

# Find the SDL3 package
find_package(SDL3 REQUIRED)

# Add executables
add_executable(chip8_emulator sourcecode/main.cpp sourcecode/chip8.cpp)

# Link SDL3 to executable
target_link_libraries(chip8_emulator PRIVATE SDL3::SDL3)