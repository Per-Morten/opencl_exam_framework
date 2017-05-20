cmake_minimum_required (VERSION 2.6)

add_library(cp_lib
    cp_log.c
    cp_clock.cpp
)

target_link_libraries(cp_lib
    ${CP_STANDARD_LINKS}
)
