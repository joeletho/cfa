cmake_minimum_required(VERSION 3.16)
project(cfa)

set(CMAKE_CXX_STANDARD 17)

set(CFA_SOURCES
        "src/cfa.h"
        "src/utils.h"
        "main.cpp"
        )

add_executable(cfa ${CFA_SOURCES})
