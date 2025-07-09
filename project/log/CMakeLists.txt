project(log
  LANGUAGES CXX
  DESCRIPTION
    "Logger for the whole project")

#------------------------------------------------------------------------------#

ensure_project_dependency_exist(spdlog::spdlog)
ensure_project_dependency_exist(simulator::cfg)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${PROJECT_NAME}
  ALIAS simulator::log
  HEADERS
    ih/factories/logger_factory.hpp
    ih/factories/sink_factory.hpp
    ih/custom_spd_flags.hpp
    ih/util.hpp
    include/log/logging.hpp
    include/log/logger.hpp
  SOURCES
    src/factories/logger_factory.cpp
    src/custom_spd_flags.cpp
    src/logger.cpp
    src/util.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/include/log
    ${PROJECT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    fmt::fmt
    spdlog::spdlog)

#------------------------------------------------------------------------------#

add_subdirectory(tests)