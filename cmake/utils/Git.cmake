find_package(Git QUIET)


macro(call_git)
    set(single_value_args OUT)
    set(multi_value_args ARGS)
    cmake_parse_arguments(git "" "${single_value_args}" "${multi_value_args}" ${ARGN})

    if (NOT Git_FOUND)
        message(FATAL_ERROR "Can not call git: git executable was not found")
    endif ()

    execute_process(
        COMMAND ${GIT_EXECUTABLE} ${git_ARGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_CALL_CODE
        OUTPUT_VARIABLE GIT_CALL_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    string(REPLACE ";" " " GIT_ARGS "${git_ARGS}")
    if (${GIT_CALL_CODE} EQUAL "0")
        set(${git_OUT} ${GIT_CALL_OUTPUT})
        message(DEBUG "\"${GIT_EXECUTABLE} ${GIT_ARGS}\" exited with code ${GIT_CALL_CODE}")
    else ()
        message(FATAL_ERROR
            "\"${GIT_EXECUTABLE} ${GIT_ARGS}\" exited with code ${GIT_CALL_CODE}")
    endif ()
endmacro()
