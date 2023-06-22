cmake_minimum_required(VERSION 3.16)
project(FinalProject)

# set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../externals)
# set(IMGUI_DIR ${LIB_DIR}/imgui)

# message(IMGUI_DIR=${IMGUI_DIR})

# list(
#     APPEND
#     IMGUI_SOURCE
#     ${IMGUI_DIR}/imconfig.h
#     ${IMGUI_DIR}/imgui.cpp
#     ${IMGUI_DIR}/imgui.h
#     ${IMGUI_DIR}/imgui_demo.cpp
#     ${IMGUI_DIR}/imgui_draw.cpp
#     ${IMGUI_DIR}/imgui_internal.h
#     ${IMGUI_DIR}/imgui_tables.cpp
#     ${IMGUI_DIR}/imgui_widgets.cpp
#     ${IMGUI_DIR}/imstb_rectpack.h
#     ${IMGUI_DIR}/imstb_textedit.h
#     ${IMGUI_DIR}/imstb_truetype.h
#     ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
#     ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
#     ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
#     ${IMGUI_DIR}/backends/imgui_impl_sdl3.h
# )

# add_library(imgui-lib STATIC ${IMGUI_SOURCE})
# #target_link_libraries(imgui-lib PUBLIC SDL3::SDL3-shared)
# target_include_directories(imgui-lib PUBLIC ${IMGUI_DIR})

#set_property(TARGET imgui-lib PROPERTY POSITION_INDEPENDENT_CODE ON)

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

target_link_libraries(FinalProject-shared PRIVATE Engine)
