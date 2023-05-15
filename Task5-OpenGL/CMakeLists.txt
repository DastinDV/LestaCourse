cmake_minimum_required(VERSION 3.16)
project(Task5-OpenGL)

add_library(glTest-shared SHARED glTest.cxx glTest.hxx)

target_compile_features(glTest-shared PUBLIC cxx_std_20)

target_link_libraries(glTest-shared PRIVATE Engine)
