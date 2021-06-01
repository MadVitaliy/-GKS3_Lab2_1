cmake_minimum_required(VERSION 3.12)
project(Laba2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(Laba2 main.cpp libs/GantDiagram.cpp libs/GantDiagram.h libs/Matrix.cpp libs/Matrix.h libs/JohnsonProblem.cpp libs/JohnsonProblem.h)