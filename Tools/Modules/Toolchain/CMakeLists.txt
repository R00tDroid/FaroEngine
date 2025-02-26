file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../../Shared/MSVCInfo/*.*
)

add_library(FaroToolchain SHARED ${SourceFiles})

target_include_directories(FaroToolchain PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)
target_include_directories(FaroToolchain PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../Shared/MSVCInfo)

target_link_libraries(FaroToolchain PUBLIC FaroTools)

ModuleExports(FaroToolchain ${CMAKE_CURRENT_LIST_DIR}/Source)
