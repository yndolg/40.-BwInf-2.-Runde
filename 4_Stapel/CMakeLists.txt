cmake_minimum_required(VERSION 3.13)
project(stapel)
set(CMAKE_CXX_STANDARD 14)



set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=sandybridge -mtune=sandybridge")
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

add_executable(stapel main.cpp BruteforceSolver.cpp BruteforceSolver.h ISDSolver.cpp ISDSolver.h Utils.cpp Utils.h)

FIND_PACKAGE( Boost 1.40 REQUIRED)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(stapel PUBLIC OpenMP::OpenMP_CXX)
endif()

TARGET_LINK_LIBRARIES(stapel LINK_PUBLIC ${Boost_LIBRARIES} )
