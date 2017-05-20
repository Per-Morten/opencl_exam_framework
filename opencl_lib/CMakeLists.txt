cmake_minimum_required (VERSION 2.6)
project (opencl_lib)

set (opencl_implementation_VERSION_MAJOR 1)
set (opencl_implementation_VERSION_MINOR 0)

include_directories(opencl_lib
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(opencl_lib
    opencl_api.h
    opencl_api.cpp
    opencl_utility.h
    opencl_utility.cpp
)

target_link_libraries(opencl_lib
    ${CP_STANDARD_LINKS}
    cp_lib
)

add_executable(opencl_lib_test
    main.cpp
)

target_link_libraries(opencl_lib_test
    ${CP_STANDARD_LINKS}
    cp_lib
    opencl_lib
)