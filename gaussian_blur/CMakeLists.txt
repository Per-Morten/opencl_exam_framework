cmake_minimum_required (VERSION 2.6)
project (gaussian_blur)

set (opencl_implementation_VERSION_MAJOR 1)
set (opencl_implementation_VERSION_MINOR 0)

include_directories(gaussian_blur
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(gaussian_blur
    main.cpp
)


target_link_libraries(gaussian_blur
    ${CP_STANDARD_LINKS}
    cp_lib
    opencl_lib
)
