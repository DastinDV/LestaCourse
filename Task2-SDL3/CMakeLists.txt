cmake_minimum_required(VERSION 3.0)
project(Task2-SDL3)

# Create an option to switch between a system sdl library and a vendored sdl
# library
set(SDL3_VENDORED FALSE CACHE BOOL "Use vendored libraries" FORCE)
set(SDL_LINK_STATIC TRUE CACHE BOOL "use static linking with SDL lib" FORCE)

if(SDL3_VENDORED)
  set(CMAKE_INSTALL_PREFIX
      ${CMAKE_BINARY_DIR}/../bin/install
      CACHE PATH "Set path for install mode" FORCE)

  if(SDL_LINK_STATIC)
    set(BUILD_SHARED_LIBS OFF)
    set(SDL_SHARED OFF)
    set(SDL_STATIC ON)
  endif()

  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../externals/SDL3
                   ${CMAKE_BINARY_DIR}/lib/SDL3)
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
  # target_link_options(Task2-SDL3-static PRIVATE -static)
endif()
