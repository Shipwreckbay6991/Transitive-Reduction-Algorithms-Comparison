cmake_minimum_required(VERSION 3.26)
project(AlgorithmProject)

set(CMAKE_CXX_STANDARD 23)

add_executable(AlgorithmProject main.cpp
        IntermediateGraph.cpp
        IntermediateGraph.h
        FinalGraph.h
        GraphParser.cpp
        GraphParser.h
)
