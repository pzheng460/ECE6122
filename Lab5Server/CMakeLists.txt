cmake_minimum_required(VERSION 3.26)
project(Lab5Server)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lab5Server Lab5Server.cpp)

find_package(SFML 2.6.1 COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(Lab5Server sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
