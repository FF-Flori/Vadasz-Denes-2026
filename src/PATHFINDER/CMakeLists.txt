cmake_minimum_required(VERSION 3.15)
project(Pathfinder CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(pathfinder PRIVATE -O3 -march=native -Wall -Wextra)
elseif(MSVC)
    target_compile_options(pathfinder PRIVATE /O2 /arch:AVX2 /W4)
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
    set_property(TARGET pathfinder PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(MINGW)
    target_link_options(pathfinder PRIVATE "-static-libgcc" "-static-libstdc++" "-Wl,-Bstatic,--whole-archive" "-lwinpthread" "-Wl,--no-whole-archive")
endif()

set_target_properties(pathfinder PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)
