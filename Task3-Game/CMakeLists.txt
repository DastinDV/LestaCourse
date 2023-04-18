cmake_minimum_required(VERSION 3.16)
project(Task3-Game)

add_library(ConsoleGame SHARED consoleGame.cxx consoleGame.hxx)

target_compile_definitions(ConsoleGame PRIVATE $<$<PLATFORM_ID:Windows>: OM_DECLSPEC=__declspec(dllexport) >)
target_compile_features(ConsoleGame PUBLIC cxx_std_20)

target_link_libraries(ConsoleGame PRIVATE Engine)
