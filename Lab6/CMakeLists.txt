cmake_minimum_required(VERSION 3.26)
project(Lab6)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)

add_executable(Lab6 Lab6.cpp)

target_link_libraries(Lab6 PRIVATE MPI::MPI_CXX)