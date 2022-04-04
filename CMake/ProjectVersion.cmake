execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/.."
    OUTPUT_VARIABLE FaroEngineVersionCommit
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/.."
    OUTPUT_VARIABLE FaroEngineVersionBranch
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

file(READ "${CMAKE_CURRENT_LIST_DIR}/../.github/ProjectVersion.txt" FileVersion)
string(REPLACE "." ";" FileVersion ${FileVersion})

list(GET FileVersion 0 FaroEngineVersionMajor)
list(GET FileVersion 1 FaroEngineVersionMinor)
list(GET FileVersion 2 FaroEngineVersionPatch)
set(FaroEngineVersionShort "${FaroEngineVersionMajor}.${FaroEngineVersionMinor}.${FaroEngineVersionPatch}")
set(FaroEngineVersionLong "${FaroEngineVersionShort}.${FaroEngineVersionCommit}")
set(FaroEngineVersionFull "${FaroEngineVersionLong} (${FaroEngineVersionBranch})")
