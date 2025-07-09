project(middleware
  LANGUAGES CXX
  DESCRIPTION
    "Market Simulator internal middleware")

#------------------------------------------------------------------------------#

ensure_project_dependency_exist(simulator::core)
ensure_project_dependency_exist(simulator::log)
ensure_project_dependency_exist(simulator::protocol)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${PROJECT_NAME}
  ALIAS simulator::middleware
  HEADERS
    ih/channels.hpp
    include/middleware/channels/detail/receiver.hpp
    include/middleware/channels/generator_admin_channel.hpp
    include/middleware/channels/trading_admin_channel.hpp
    include/middleware/channels/trading_reply_channel.hpp
    include/middleware/channels/trading_request_channel.hpp
    include/middleware/channels/trading_session_event_channel.hpp
    include/middleware/routing/errors.hpp
    include/middleware/routing/generator_admin_channel.hpp
    include/middleware/routing/trading_admin_channel.hpp
    include/middleware/routing/trading_reply_channel.hpp
    include/middleware/routing/trading_request_channel.hpp
    include/middleware/routing/trading_session_event_channel.hpp
  SOURCES
    src/middleware.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    simulator::protocol
    simulator::core
  PRIVATE_DEPENDENCIES
    simulator::log
    simulator::cfg)

#------------------------------------------------------------------------------#

add_subdirectory(tests)