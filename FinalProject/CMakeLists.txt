cmake_minimum_required(VERSION 3.16)
project(FinalProject)

add_library(FinalProject-shared SHARED map.cxx mirrorGame.cxx map.hxx mirrorGame.hxx)

target_compile_features(FinalProject-shared PUBLIC cxx_std_20)

target_link_libraries(FinalProject-shared PRIVATE Engine)
