cmake_minimum_required(VERSION 3.15)
project(FaroDocs)

include(${CMAKE_CURRENT_LIST_DIR}/../CMake/ProjectVersion.cmake)

message("Engine version: ${FaroEngineVersionFull}")

set(DocumentRoot ${CMAKE_CURRENT_LIST_DIR})

configure_file(${CMAKE_CURRENT_LIST_DIR}/Doxygen.in ${CMAKE_CURRENT_LIST_DIR}/Doxygen @ONLY)
