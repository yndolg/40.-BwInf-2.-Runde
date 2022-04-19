cmake_minimum_required(VERSION 3.21)
project(hexmax)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native -O3 ")

add_executable(hexmax main.cpp Zustand.cpp Zustand.h DPSolver.cpp DPSolver.h TSPSolver.cpp TSPSolver.h)
