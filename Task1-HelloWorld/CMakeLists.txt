cmake_minimum_required(VERSION 3.22)
project(HelloWorld  CXX)

add_executable(HelloWorld HelloWorld.cxx)
target_compile_features(HelloWorld PUBLIC cxx_std_17) 
