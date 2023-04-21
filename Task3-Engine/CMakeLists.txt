cmake_minimum_required(VERSION 3.16)
project(Task3-Engine)

add_executable(
  Engine
  engine.cxx
  game.cxx
  main.cxx
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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Task3-Game
${CMAKE_BINARY_DIR}/lib/MyGame)