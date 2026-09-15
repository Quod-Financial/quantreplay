set(COMPONENT_NAME ${PROJECT_NAME}_generator_initiator)

#------------------------------------------------------------------------------#

ensure_project_dependency_exist(quickfix::quickfix)
ensure_project_dependency_exist(fix::common)
ensure_project_dependency_exist(simulator::protocol)
ensure_project_dependency_exist(simulator::log)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS simulator::fix_generator_initiator
  HEADERS
    ih/mapping/detail/from_fix_conversion.hpp
    ih/mapping/detail/to_fix_conversion.hpp
    ih/mapping/from_fix_mapping.hpp
    ih/mapping/from_fix_mapper.hpp
    ih/mapping/mapper_concepts.hpp
    ih/mapping/to_fix_mapper.hpp
    ih/mapping/to_fix_mapping.hpp
    ih/processors/market_data_reply_processor.hpp
    ih/processors/market_data_request_sender.hpp
    ih/processors/session_event_processor.hpp
    ih/implementation.hpp
    include/fix/generator_initiator/generator_initiator.hpp
    include/fix/generator_initiator/lifetime.hpp
    include/fix/generator_initiator/transport.hpp
  SOURCES
    src/mapping/detail/from_fix_conversion.cpp
    src/mapping/detail/to_fix_conversion.cpp
    src/mapping/from_fix_mapper.cpp
    src/mapping/to_fix_mapper.cpp
    src/generator_initiator.cpp
    src/implementation.cpp
    src/lifetime.cpp
    src/transport.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    simulator::protocol
  PRIVATE_DEPENDENCIES
    quickfix::quickfix
    fix::common
    simulator::middleware
    simulator::log)

#------------------------------------------------------------------------------#

add_subdirectory(tests)
