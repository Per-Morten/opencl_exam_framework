cmake_minimum_required (VERSION 2.6)
project (hough_transform)

set (opencl_implementation_VERSION_MAJOR 1)
set (opencl_implementation_VERSION_MINOR 0)

include_directories(hough_transform
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(hough_transform
    main.cpp
)


target_link_libraries(hough_transform
    ${CP_STANDARD_LINKS}
    cp_lib
)
