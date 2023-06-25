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

target_compile_features(FinalProject-shared PUBLIC cxx_std_20)
# target_include_directories(FinalProject-shared PUBLIC ${IMGUI_DIR}
#                             ${IMGUI_DIR}/backends)



if (WIN32)
  target_compile_definitions(FinalProject-shared PRIVATE "-DIZ_DECLSPEC=__declspec(dllexport)")
endif()

target_link_libraries(FinalProject-shared PUBLIC Engine)