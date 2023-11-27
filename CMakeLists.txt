cmake_minimum_required(VERSION 3.23)
project(IDE)

set(CMAKE_CXX_STANDARD 11)

add_executable(IDE main.cpp SLR.cpp CFG.cpp)
