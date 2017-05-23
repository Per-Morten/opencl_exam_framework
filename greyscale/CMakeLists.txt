cmake_minimum_required (VERSION 2.6)
project (greyscale)

set (opencl_implementation_VERSION_MAJOR 1)
set (opencl_implementation_VERSION_MINOR 0)

include_directories(greyscale
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(greyscale
    main.cpp
)


target_link_libraries(greyscale
    ${CP_STANDARD_LINKS}
    cp_lib
    opencl_lib
)
