include(utils/Git)


set(SIM_VERSION "unversioned" CACHE STRING
  "A Market Simulator major version")

function(_generate_revision_hash)
    set(single_value_args OUT)
    cmake_parse_arguments(arg "" "${single_value_args}" "" ${ARGN})

    call_git(
        ARGS rev-parse HEAD
        OUT GIT_COMMIT_REVISION)
    string(SUBSTRING "${GIT_COMMIT_REVISION}" 0 8 GIT_COMMIT_REVISION)

    set(${arg_OUT} ${GIT_COMMIT_REVISION} PARENT_SCOPE)
endfunction()

function(configure_version SIM_VERSION_MAJOR)
    set(SIM_VERSION_MAJOR ${SIM_VERSION_MAJOR} PARENT_SCOPE)
    message(STATUS "Major version: ${SIM_VERSION_MAJOR}")

    call_git(
        ARGS log -n1 --format=%ct
        OUT SIM_VERSION_REV_UNIX_TIMESTAMP)
    set(SIM_VERSION_REV_UNIX_TIMESTAMP ${SIM_VERSION_REV_UNIX_TIMESTAMP} PARENT_SCOPE)
    message(STATUS "Revision UNIX timestamp: ${SIM_VERSION_REV_UNIX_TIMESTAMP}")

    _generate_revision_hash(OUT SIM_VERSION_REV_HASH)
    set(SIM_VERSION_REV_HASH ${SIM_VERSION_REV_HASH} PARENT_SCOPE)
    message(STATUS "Revision hash: ${SIM_VERSION_REV_HASH}")

    call_git(
        ARGS log -n1 --format=%ci
        OUT SIM_VERSION_REV_HUMAN_READABLE_TIMESTAMP)
    set(SIM_VERSION_REV_HUMAN_READABLE_TIMESTAMP ${SIM_VERSION_REV_HUMAN_READABLE_TIMESTAMP} PARENT_SCOPE)
    message(STATUS "Revision human-readable timestamp: ${SIM_VERSION_REV_HUMAN_READABLE_TIMESTAMP}")
endfunction()
