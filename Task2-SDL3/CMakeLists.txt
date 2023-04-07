cmake_minimum_required(VERSION 3.0)
project(Task2-SDL3)

# Create an option to switch between a system sdl library and a vendored sdl library
option(MYGAME_VENDORED "Use vendored libraries" OFF)

if(MYGAME_VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL3 package, 2. look for the SDL3 component and 3. fail if none can be found
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
endif()

# Create your game executable target as usual
add_executable(Task2-SDL3 main.cxx)

# Link to the actual SDL3 library. SDL3::SDL3 is the shared SDL library, SDL3::SDL3-static is the static SDL libarary.
target_link_libraries(Task2-SDL3 PRIVATE SDL3::SDL3)
