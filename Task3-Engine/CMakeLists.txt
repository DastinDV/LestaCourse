cmake_minimum_required(VERSION 3.16)
project(Task3-Engine)

add_library(Engine SHARED 
    emulator.cxx
    engine.cxx
    emulator.hxx
    engine.hxx)

target_compile_features(Engine PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)

target_link_libraries(Engine PRIVATE SDL3::SDL3-shared)