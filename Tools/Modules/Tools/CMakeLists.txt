configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Version.hpp.in ${CMAKE_CURRENT_LIST_DIR}/Source/Version.generated.hpp @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_library(FaroTools SHARED ${SourceFiles})

if(MSVC)
    target_link_libraries(FaroTools Rpcrt4.lib)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(UUID REQUIRED uuid)

    target_link_libraries(FaroTools PRIVATE ${UUID_LIBRARIES})
    target_include_directories(FaroTools PRIVATE ${UUID_INCLUDE_DIRS})
endif()

target_include_directories(FaroTools PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)
target_include_directories(FaroTools PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/../../Shared/MSVCInfo
    ${CMAKE_CURRENT_LIST_DIR}/../../Shared/glob/single_include
)

ModuleExports(FaroTools ${CMAKE_CURRENT_LIST_DIR}/Source)
