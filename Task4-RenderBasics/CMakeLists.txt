cmake_minimum_required(VERSION 3.16)
project(Task4-RenderBasics)

add_library(RenderBasics SHARED renderBasics.cxx renderBasics.hxx)

target_compile_features(RenderBasics PUBLIC cxx_std_20)

target_link_libraries(RenderBasics PRIVATE Engine)
