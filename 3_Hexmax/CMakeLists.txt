cmake_minimum_required(VERSION 3.21)
project(3_Hexmax)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native -O3 ")

add_executable(3_Hexmax main.cpp Solver.cpp Solver.h State.cpp State.h DPSolver.cpp DPSolver.h)
