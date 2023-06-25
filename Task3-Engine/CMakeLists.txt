cmake_minimum_required(VERSION 3.16)
project(Task3-Engine)

set(CMAKE_CXX_STANDARD 17)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../externals)
set(IMGUI_DIR ${LIB_DIR}/imgui)

message(IMGUI_DIR=${IMGUI_DIR})

list(
    APPEND
    IMGUI_SOURCE
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imstb_rectpack.h
    ${IMGUI_DIR}/imstb_textedit.h
    ${IMGUI_DIR}/imstb_truetype.h
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.h
)

if(WIN32)
  set(CMAKE_PREFIX_PATH "C:\\libs\\SDL3")
endif()

message(CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH})

find_package(SDL3 REQUIRED)

add_library(imgui-lib STATIC ${IMGUI_SOURCE})
#target_link_libraries(imgui-lib PUBLIC SDL3::SDL3-shared)
target_include_directories(imgui-lib PUBLIC ${IMGUI_DIR})
if(WIN32)
target_include_directories(imgui-lib PUBLIC ${CMAKE_PREFIX_PATH}/include)
endif()
set_property(TARGET imgui-lib PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(
  Engine
  engine.cxx
  game.cxx
  canvas.cxx
  ./Renderers/lineRenderer.cxx
  ./Renderers/triangleRenderer.cxx
  ./Renderers/glRenderer.cxx
  ./OpenGL/vertexBuffer.cxx
  ./OpenGL/elementBuffer.cxx
  ./OpenGL/vao.cxx
  ./OpenGL/shader.cxx
  ./OpenGL/texture.cxx
  ./OpenGL/glDebug.cxx
  ./glad/src/glad.c
  main.cxx
  canvas.hxx
  engine.hxx
  game.hxx
  stb_image.h
  ./Renderers/lineRenderer.hxx
  ./Renderers/triangleRenderer.hxx
  ./Renderers/glRenderer.hxx
  ./OpenGL/vertexBuffer.hxx
  ./OpenGL/elementBuffer.hxx
  ./OpenGL/vao.hxx
  ./OpenGL/shader.hxx
  ./OpenGL/texture.hxx
  ./OpenGL/glDebug.hxx
  ./glad/include/glad/glad.h)

if(WIN32)
    target_compile_definitions(Engine
                               PRIVATE "-DIZ_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

set_target_properties(
  Engine PROPERTIES ENABLE_EXPORTS ON # we need to be able to link to exe
                                     # functions from game
)

target_include_directories(Engine PUBLIC 
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Renderers>
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad/include>
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/OpenGL>)

target_include_directories(Engine PUBLIC ${IMGUI_DIR}
                           ${IMGUI_DIR}/backends)

target_compile_features(Engine PUBLIC cxx_std_20)

#target_link_libraries(Engine PRIVATE SDL3::SDL3-shared ConsoleGame)
if (LINUX)
target_link_libraries(Engine PRIVATE SDL3::SDL3-shared imgui-lib)
endif()
if (WIN32)
target_link_libraries(Engine PRIVATE SDL3::SDL3 imgui-lib)
target_include_directories(Engine PUBLIC ${CMAKE_PREFIX_PATH}/include)
endif()

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Task3-Game
#${CMAKE_BINARY_DIR}/lib/MyGame)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../FinalProject
${CMAKE_BINARY_DIR}/lib/MirrorGame)
