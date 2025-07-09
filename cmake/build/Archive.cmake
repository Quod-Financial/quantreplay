include(utils/Git)
include(build/Version)

macro(_strip_version_revision REVISION_VARIABLE)
    # Take first 8 symbols from the commit hash
    string(SUBSTRING "${${REVISION_VARIABLE}}" 0 8 ${REVISION_VARIABLE})
endmacro()

macro(_get_archive_base_name REVISION_NUMBER OUT_VAR)
    set(${OUT_VAR} QuodFinancial-MarketSimulator-${SIM_VERSION}_Rev_${REVISION_NUMBER})
endmacro()


macro(_get_tar_archive_name REVISION_NUMBER OUT_VAR)
    _get_archive_base_name(${REVISION_NUMBER} ${OUT_VAR})
    string(APPEND ${OUT_VAR} ".tar.gz")
endmacro()


function(setup_tar_archive_target ARCHIVE_TARGET_NAME)
    call_git(
        ARGS rev-parse HEAD
        OUT GIT_REVISION)

    _strip_version_revision(GIT_REVISION)
    _get_tar_archive_name(${GIT_REVISION} ARCHIVE_NAME)

    set(EXECUTABLE_PATH ${CMAKE_BINARY_DIR}/bin/marketsimulator)
    set(PACKAGING_PATH ${CMAKE_BINARY_DIR}/quod_package)
    set(ARCHIVE_BASE_PATH ${PACKAGING_PATH}/quod)

    add_custom_target(${ARCHIVE_TARGET_NAME}
        COMMAND mkdir -p ${ARCHIVE_BASE_PATH}/bin
        COMMAND cp "${EXECUTABLE_PATH}" "${ARCHIVE_BASE_PATH}/bin/marketsimulator"
        COMMAND cp -R "${CMAKE_SOURCE_DIR}/cfg" "${ARCHIVE_BASE_PATH}"
        COMMAND cp -R "${CMAKE_SOURCE_DIR}/script" "${ARCHIVE_BASE_PATH}"
        COMMAND cp -R "${CMAKE_SOURCE_DIR}/db" "${ARCHIVE_BASE_PATH}"
        COMMAND cd "${PACKAGING_PATH}" && ${CMAKE_COMMAND} -E tar "cfvz" "${ARCHIVE_NAME}" "${ARCHIVE_BASE_PATH}"
        COMMAND rm -rf "${ARCHIVE_BASE_PATH}"
        DEPENDS marketsimulator)
endfunction()
