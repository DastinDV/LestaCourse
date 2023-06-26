cmake_minimum_required(VERSION 3.16)
project(FinalProject)

add_library(
  FinalProject-shared SHARED
  map.cxx
  mirrorGame.cxx
  player.cxx
  mirror.cxx
  map.hxx
  mirrorGame.hxx
  player.hxx
  mirror.hxx
  common.hxx)

target_compile_options(
    FinalProject-shared
    PRIVATE
        "$<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wall;-Wextra;-Wpedantic;>"
)

target_compile_features(FinalProject-shared PUBLIC cxx_std_20)
# target_include_directories(FinalProject-shared PUBLIC ${IMGUI_DIR}
#                             ${IMGUI_DIR}/backends)



if (WIN32)
  target_compile_definitions(FinalProject-shared PRIVATE "-DIZ_DECLSPEC=__declspec(dllexport)")

  # target_include_directories(FinalProject-shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Task3-Engine/Renderers
  # ${CMAKE_CURRENT_SOURCE_DIR}/../Task3-Engine/OpenGL
  # ${CMAKE_CURRENT_SOURCE_DIR}/../Task3-Engine/
  # ${CMAKE_CURRENT_SOURCE_DIR})
  #target_link_libraries(FinalProject-shared PUBLIC Engine)
endif()

target_link_libraries(FinalProject-shared PUBLIC Engine)