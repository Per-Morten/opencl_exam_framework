cmake_minimum_required (VERSION 2.6)
project (third_party)

set (opencl_implementation_VERSION_MAJOR 1)
set (opencl_implementation_VERSION_MINOR 0)

add_library(third_party
    lodepng.cpp
    lodepng.h
)
