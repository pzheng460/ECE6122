cmake_minimum_required(VERSION 3.26)
project(Lab1)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lab1 main.cpp
        ECE_PointCharge.cpp
        ECE_PointCharge.h
        ECE_ElectricField.cpp
        ECE_ElectricField.h
)
