project(cfg
  LANGUAGES CXX
  DESCRIPTION
    "Global configuration component")

#------------------------------------------------------------------------------#

ensure_project_dependency_exist(date::date)
ensure_project_dependency_exist(tinyxml2::tinyxml2)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${PROJECT_NAME}
  ALIAS simulator::cfg
  HEADERS
    include/cfg/api/cfg.hpp
    src/cfg_impl.hpp
  SOURCES
    src/cfg_impl.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/include/cfg
    ${PROJECT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    simulator::core
  PRIVATE_DEPENDENCIES
    date::date
    tinyxml2::tinyxml2
    fmt::fmt)