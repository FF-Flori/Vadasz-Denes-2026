cmake_minimum_required(VERSION 3.15)
project(Pathfinder CXX)

set(PYBIND11_FINDPYTHON NEW)

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

if(MINGW)
    target_link_options(pathfinder PRIVATE "-static-libgcc" "-static-libstdc++" "-Wl,-Bstatic,--whole-archive" "-lwinpthread" "-Wl,--no-whole-archive" "-fsanitize=address")
endif()

set_target_properties(pathfinder PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)
