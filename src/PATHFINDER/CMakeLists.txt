cmake_minimum_required(VERSION 3.15)
project(MyBigProject CXX)
include(FetchContent)

# fetch pybind11 from github
FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v3.0.2
)
FetchContent_MakeAvailable(pybind11)

# add modules to pybind
set(SOURCES
        main/pathfinder.cpp
        main/bindings.cpp
)
pybind11_add_module(pathfinder ${SOURCES})