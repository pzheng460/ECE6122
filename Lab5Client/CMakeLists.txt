cmake_minimum_required(VERSION 3.26)
project(Lab5Client)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lab5Client Lab5Client.cpp)

find_package(SFML 2.6.1 COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(Lab5Client sfml-system sfml-window sfml-graphics sfml-audio sfml-network)