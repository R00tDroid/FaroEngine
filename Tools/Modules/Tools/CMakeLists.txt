configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Version.hpp.in ${CMAKE_CURRENT_LIST_DIR}/Source/Version.generated.hpp @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_library(FaroTools SHARED ${SourceFiles})

if(MSVC)
    target_link_libraries(FaroTools Rpcrt4.lib)
else()
    target_link_libraries(FaroTools uuid)
endif()

target_include_directories(FaroTools PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)

ModuleExports(FaroTools ${CMAKE_CURRENT_LIST_DIR}/Source)
