execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/.."
    OUTPUT_VARIABLE FaroEngineVersionCommit
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message("GITHUB_BASE_REF $ENV{GITHUB_BASE_REF}")
message("GITHUB_HEAD_REF $ENV{GITHUB_HEAD_REF}")
message("GITHUB_REF_NAME $ENV{GITHUB_REF_NAME}")

if(DEFINED ENV{GITHUB_HEAD_REF})
    set(FaroEngineVersionBranch $ENV{GITHUB_HEAD_REF})
else()
    execute_process(COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE FaroEngineVersionBranch)
endif()

file(READ "${CMAKE_CURRENT_LIST_DIR}/../.github/ProjectVersion.txt" FileVersion)
string(REPLACE "." ";" FileVersion ${FileVersion})

list(GET FileVersion 0 FaroEngineVersionMajor)
list(GET FileVersion 1 FaroEngineVersionMinor)
list(GET FileVersion 2 FaroEngineVersionPatch)
set(FaroEngineVersionShort "${FaroEngineVersionMajor}.${FaroEngineVersionMinor}.${FaroEngineVersionPatch}")
set(FaroEngineVersionLong "${FaroEngineVersionShort}.${FaroEngineVersionCommit}")
set(FaroEngineVersionFull "${FaroEngineVersionLong} (${FaroEngineVersionBranch})")
