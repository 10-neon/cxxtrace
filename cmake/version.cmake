find_package(Git REQUIRED)
if(GIT_EXECUTABLE)
    get_filename_component(SRC_DIR ${SRC} DIRECTORY)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --match "v*"
            WORKING_DIRECTORY ${SRC_DIR}
            OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
            RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT GIT_DESCRIBE_ERROR_CODE)
        set(GIT_VERSION ${GIT_DESCRIBE_VERSION})
    endif()
endif()

if(NOT DEFINED GIT_VERSION)
    set(GIT_VERSION v0.0.0-unknown)
    message(WARNING "Failed to determine GIT_DESCRIBE_VERSION from Git tags. Using default version \"${GIT_VERSION}\".")
endif()
message(STATUS "GIT_VERSION: ${GIT_VERSION}")
configure_file(${SRC} ${DST} @ONLY)
