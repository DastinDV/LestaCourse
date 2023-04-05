cmake_minimum_required(VERSION 3.22)

project(Task1.2-HelloWorld)

if(MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  set(BUILD_SHARED_LIBS TRUE)
endif()

add_subdirectory(lib)

add_executable(Task1.2-HelloWorld-static main.cxx)
target_link_libraries(Task1.2-HelloWorld-static PrintLib-Static)
target_link_options(Task1.2-HelloWorld-static PRIVATE -static)

add_executable(Task1.2-HelloWorld-shared main.cxx)
target_link_libraries(Task1.2-HelloWorld-shared PrintLib-Shared)

enable_testing()

add_executable(SimpleTest main.cxx)
target_link_libraries(SimpleTest PrintLib-Static)
add_test(NAME TEST_HELLO_WORLD_GOOD_EXIT COMMAND SimpleTest hello world)

add_test(
  NAME TEST_HELLO_WORLD_BAD_EXIT
  COMMAND
    sh -c
    "${CMAKE_CURRENT_LIST_DIR}/build/./SimpleTest hello world >> /dev/video0")
