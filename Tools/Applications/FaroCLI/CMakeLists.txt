configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Version.hpp.in ${CMAKE_CURRENT_LIST_DIR}/Source/Version.generated.hpp @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_executable(FaroCLI ${SourceFiles})

target_include_directories(FaroCLI PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)

target_link_libraries(FaroCLI PUBLIC FaroTools FaroProjects FaroGeneration)

set_target_properties(FaroCLI PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-configure -project -clean -build -debug -platform windowsx64 -path \"${CMAKE_CURRENT_LIST_DIR}\\..\\Tests\\TestProject\\TestProject.faroproject.json\"")

ModuleExports(FaroCLI ${CMAKE_CURRENT_LIST_DIR}/Source)
