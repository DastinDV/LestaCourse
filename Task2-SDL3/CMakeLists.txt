cmake_minimum_required(VERSION 3.0)
project(Task2-SDL3)

# Create an option to switch between a system sdl library and a vendored sdl
# library
option(MYGAME_VENDORED "Use vendored libraries" OFF)
option(SDL_LINK_STATIC "use static linking with SDL lib" ON)

if(MYGAME_VENDORED)
  add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
  # 1. Look for a SDL3 package, 2. look for the SDL3 component and 3. fail if
  #   none can be found
  find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
endif()

if(NOT SDL_LINK_STATIC)
  # Create your game executable target as usual
  add_executable(Task2-SDL3-shared main.cxx)
  target_link_libraries(Task2-SDL3-shared PRIVATE SDL3::SDL3-shared)
else()
  add_executable(Task2-SDL3-static main.cxx)
  target_link_libraries(Task2-SDL3-static PRIVATE SDL3::SDL3-static)
  #target_link_options(Task2-SDL3-static PRIVATE -static)
endif()
