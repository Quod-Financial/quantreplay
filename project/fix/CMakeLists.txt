project(fix
  LANGUAGES CXX
  DESCRIPTION
  "Market Simulator FIX protocol module")

#------------------------------------------------------------------------------#
# Internal components                                                          #
#------------------------------------------------------------------------------#

add_subdirectory(components/common)
add_subdirectory(components/generator_initiator)
add_subdirectory(components/trading_system_acceptor)

#------------------------------------------------------------------------------#

ensure_project_dependency_exist(simulator::core)
ensure_project_dependency_exist(fix::common)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${PROJECT_NAME}
  ALIAS simulator::fix
  HEADERS
    include/fix/session_settings.hpp
  SOURCES
    src/session_settings.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    simulator::core
  PRIVATE_DEPENDENCIES
    fix::common)

#------------------------------------------------------------------------------#

add_subdirectory(tests)
