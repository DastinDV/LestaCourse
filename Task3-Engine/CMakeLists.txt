cmake_minimum_required(VERSION 3.16)
project(Task3-Engine)

add_executable(
  Engine
  emulator.cxx
  engine.cxx
  game.cxx
  main.cxx
  emulator.hxx
  engine.hxx
  game.hxx)

set_target_properties(
  Engine PROPERTIES ENABLE_EXPORTS 1 # we need to be able to link to exe
                                     # functions from game
)

target_compile_features(Engine PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)

#target_link_libraries(Engine PRIVATE SDL3::SDL3-shared ConsoleGame)
target_link_libraries(Engine PRIVATE SDL3::SDL3-shared)


add_library(ConsoleGame SHARED consoleGame.cxx consoleGame.hxx)

target_compile_features(ConsoleGame PUBLIC cxx_std_20)

target_link_libraries(ConsoleGame PRIVATE Engine)