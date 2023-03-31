cmake_minimum_required(VERSION 3.22)
project (PrintLib)

#STATIC LIB
add_library(PrintLib-Static STATIC src/printLib.cxx)
target_include_directories(PrintLib-Static
                         PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


add_library(PrintLib-Shared SHARED src/printLib.cxx)
target_include_directories(PrintLib-Shared
                         PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
