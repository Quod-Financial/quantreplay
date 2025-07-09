set(COMPONENT_NAME ${PROJECT_NAME}_acceptor)

#------------------------------------------------------------------------------#

ensure_project_dependency_exist(quickfix::quickfix)
ensure_project_dependency_exist(simulator::fix_common)
ensure_project_dependency_exist(simulator::protocol)
ensure_project_dependency_exist(simulator::log)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS simulator::fix_acceptor
  HEADERS
    ih/communicators/application.hpp
    ih/communicators/fix_reply_sender.hpp
    ih/communicators/reply_sender.hpp
    ih/mapping/from_fix_mapper.hpp
    ih/mapping/to_fix_mapper.hpp
    ih/processors/app_event_processor.hpp
    ih/processors/app_reply_processor.hpp
    ih/processors/app_request_processor.hpp
    ih/processors/event_processor.hpp
    ih/processors/request_processor.hpp
    ih/acceptor.hpp
    include/acceptor/acceptor.hpp
    include/acceptor/lifetime.hpp
    include/acceptor/transport.hpp
  SOURCES
    src/communicators/application.cpp
    src/communicators/fix_reply_sender.cpp
    src/mapping/from_fix_mapper.cpp
    src/mapping/to_fix_mapper.cpp
    src/acceptor.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    simulator::protocol
  PRIVATE_DEPENDENCIES
    quickfix::quickfix
    simulator::fix_common
    simulator::middleware
    simulator::log)

#------------------------------------------------------------------------------#

add_subdirectory(tests)