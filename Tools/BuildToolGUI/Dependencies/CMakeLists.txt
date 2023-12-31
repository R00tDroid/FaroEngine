cmake_minimum_required(VERSION 3.15)

set( SDL_LIBC ON CACHE BOOL "" FORCE )
set( SDL_TEST OFF CACHE BOOL "" FORCE )
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SDL")


file(GLOB_RECURSE NuklearFiles
    ${CMAKE_CURRENT_LIST_DIR}/Nuklear/src/*.c
)

add_library(Nuklear STATIC ${NuklearFiles})

target_include_directories(Nuklear PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Nuklear/src
)
