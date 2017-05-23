cmake_minimum_required (VERSION 2.6)

add_library(cp_lib
    cp_clock.cpp
    cp_clock.h
    cp_lib.h
    cp_image.cpp
    cp_image.h
    cp_log.c
    cp_params.h
)

target_link_libraries(cp_lib
    ${CP_STANDARD_LINKS}
    third_party
)
