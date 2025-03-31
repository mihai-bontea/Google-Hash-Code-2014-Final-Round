cmake_minimum_required(VERSION 3.27)
project(sol3)

set(CMAKE_CXX_STANDARD 23)

add_executable(sol3 main.cpp
        HungarianAlgorithm.h
        ParallelFloydWarshall.h
        Hierholzer.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")
