set(COMPONENT_NAME ${PROJECT_NAME}_common)

#------------------------------------------------------------------------------#

ensure_project_dependency_exist(quickfix::quickfix)
ensure_project_dependency_exist(simulator::protocol)
ensure_project_dependency_exist(simulator::log)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS simulator::fix_common
  HEADERS
    include/common/mapping/detail/definitions.hpp
    include/common/mapping/detail/error_reporting.hpp
    include/common/mapping/detail/from_fix_conversion.hpp
    include/common/mapping/detail/to_fix_conversion.hpp
    include/common/mapping/checks.hpp
    include/common/mapping/from_fix_mapping.hpp
    include/common/mapping/to_fix_mapping.hpp
    include/common/custom_fields.hpp
    include/common/custom_values.hpp
    include/common/fix_logger.hpp
    include/common/mapping/setting/mapping_settings.hpp
    include/common/mapping/setting/timestamp_precision.hpp
    include/common/message_store.hpp
    include/common/meta.hpp
    include/common/session_conversion.hpp
    include/common/session_settings.hpp
  SOURCES
    src/mapping/detail/from_fix_conversion.cpp
    src/mapping/detail/to_fix_conversion.cpp
    src/fix_logger.cpp
    src/mapping/setting/mapping_settings.cpp
    src/message_store.cpp
    src/session_conversion.cpp
    src/session_settings.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    quickfix::quickfix
    simulator::protocol
  PRIVATE_DEPENDENCIES
    fmt::fmt
    simulator::log)

#------------------------------------------------------------------------------#

add_subdirectory(tests)