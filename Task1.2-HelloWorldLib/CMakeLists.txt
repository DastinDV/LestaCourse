cmake_minimum_required (VERSION 3.22)

project(Task1.2-HelloWorld)

add_subdirectory(lib)

add_executable(Task1.2-HelloWorld-static main.cxx)
target_link_libraries(Task1.2-HelloWorld-static PrintLib-Static)
target_link_options(Task1.2-HelloWorld-static PRIVATE -static)

add_executable(Task1.2-HelloWorld-shared main.cxx)
target_link_libraries(Task1.2-HelloWorld-shared PrintLib-Shared)
