cmake_minimum_required(VERSION 3.16)
project(Task3-Engine)

add_executable(
  Engine
  engine.cxx
  game.cxx
  canvas.cxx
  ./Renderers/lineRenderer.cxx
  ./Renderers/triangleRenderer.cxx
  ./Renderers/glRenderer.cxx
  ./OpenGL/vertexBuffer.cxx
  ./glad/src/glad.c
  main.cxx
  canvas.hxx
  engine.hxx
  game.hxx
  ./Renderers/lineRenderer.hxx
  ./Renderers/triangleRenderer.hxx
  ./Renderers/glRenderer.hxx
  ./OpenGL/vertexBuffer.hxx
  ./glad/include/glad/glad.h)

set_target_properties(
  Engine PROPERTIES ENABLE_EXPORTS 1 # we need to be able to link to exe
                                     # functions from game
)

target_include_directories(Engine PUBLIC 
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Renderers>
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad/include>
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/OpenGL>)

target_compile_features(Engine PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)

#target_link_libraries(Engine PRIVATE SDL3::SDL3-shared ConsoleGame)
target_link_libraries(Engine PRIVATE SDL3::SDL3-shared)

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Task3-Game
#${CMAKE_BINARY_DIR}/lib/MyGame)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Task5-OpenGL
${CMAKE_BINARY_DIR}/lib/glTest)
