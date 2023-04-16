cmake_minimum_required(VERSION 3.0)
project(Task3-Game)

# Create an option to switch between a system sdl library and a vendored sdl
# library
set(SDL3_VENDORED
    FALSE
    CACHE BOOL "Use vendored libraries" FORCE)
set(SDL_LINK_STATIC
    FALSE
    CACHE BOOL "use static linking with SDL lib" FORCE)

if(SDL3_VENDORED)
  set(CMAKE_INSTALL_PREFIX
      ${CMAKE_BINARY_DIR}
      CACHE PATH "Path for install" FORCE)

  # add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../externals/SDL3
  # ${CMAKE_BINARY_DIR}/lib/SDL3)

  if(SDL_LINK_STATIC)
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

# Use my engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Task3-Engine
                 ${CMAKE_BINARY_DIR}/lib/MyEngine)

if(NOT SDL_LINK_STATIC)
  # Create your game executable target as usual
  add_executable(Task3-Game-shared main.cxx)
  target_link_libraries(Task3-Game-shared PRIVATE Engine)
else()
  add_executable(Task3-Game-static main.cxx)
  target_link_libraries(Task3-Game-static PRIVATE SDL3::SDL3-static)
  # target_link_options(Task2-SDL3-static PRIVATE -static)
endif()
