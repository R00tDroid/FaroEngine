execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/.."
    OUTPUT_VARIABLE FaroEngineVersionCommit
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(DEFINED ENV{GITHUB_HEAD_REF} AND NOT $ENV{GITHUB_HEAD_REF} EQUAL "")
    set(FaroEngineVersionBranch $ENV{GITHUB_HEAD_REF})
elseif(DEFINED ENV{GITHUB_REF_NAME})
    set(FaroEngineVersionBranch $ENV{GITHUB_REF_NAME})
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
