cmake_minimum_required(VERSION 3.23)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(Compiler main.cpp FA.cpp FA.h LR.cpp LR.h)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
