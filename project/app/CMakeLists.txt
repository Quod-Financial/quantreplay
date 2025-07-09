project(app
  LANGUAGES CXX
  DESCRIPTION
    "Market Simulator application")

#------------------------------------------------------------------------------#

ensure_project_dependency_exist(simulator::cfg)
ensure_project_dependency_exist(simulator::log)
ensure_project_dependency_exist(simulator::data_layer)
ensure_project_dependency_exist(simulator::middleware)
ensure_project_dependency_exist(simulator::trading_system)
ensure_project_dependency_exist(simulator::fix_acceptor)
ensure_project_dependency_exist(simulator::generator)
ensure_project_dependency_exist(simulator::http)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${PROJECT_NAME}
  ALIAS simulator::app
  HEADERS
    ih/components/fix_acceptor.hpp
    ih/components/generator.hpp
    ih/components/http_server.hpp
    ih/components/trading_engine.hpp
    ih/dispatchers/venue_trading_reply_dispatcher.hpp
    ih/platforms/platform.hpp
    ih/platforms/venue_simulation_platform.hpp
    ih/application.hpp
    ih/command_options.hpp
    ih/loading.hpp
    ih/loop.hpp
  SOURCES
    src/platforms/venue_simulation_platform.cpp
    src/application.cpp
    src/command_options.cpp
    src/loading.cpp
    src/loop.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}
  PRIVATE_DEPENDENCIES
    fmt::fmt
    simulator::middleware
    simulator::trading_system
    simulator::fix_acceptor
    simulator::generator
    simulator::http
    simulator::cfg
    simulator::log
    simulator::data_layer)

#------------------------------------------------------------------------------#

add_executable_binary(
  NAME marketsimulator
  SOURCES
    src/main.cpp
  PRIVATE_DEPENDENCIES
    fmt::fmt
    simulator::app
    simulator::cfg
    simulator::core
    simulator::log)

setup_tar_archive_target(create_tar)

#------------------------------------------------------------------------------#

add_subdirectory(tests)