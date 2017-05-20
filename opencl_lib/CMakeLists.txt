cmake_minimum_required (VERSION 2.6)
project (opencl_lib)

set (opencl_implementation_VERSION_MAJOR 1)
set (opencl_implementation_VERSION_MINOR 0)

include_directories(opencl_lib
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(opencl_lib
)

target_link_libraries(opencl_lib
    ${CP_STANDARD_LINKS}
    cp_lib
)
