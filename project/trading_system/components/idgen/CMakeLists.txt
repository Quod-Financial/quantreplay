set(COMPONENT_NAME ${PROJECT_NAME}_idgen)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS ts::idgen
  HEADERS
    ih/contexts/execution_id_context.hpp
    ih/contexts/instrument_id_context.hpp
    ih/contexts/market_entry_id_context.hpp
    ih/contexts/order_id_context.hpp
    ih/sequences/chrono_sequence.hpp
    ih/sequences/common.hpp
    ih/sequences/numeric_sequence.hpp
    ih/generation.hpp
    include/idgen/errors.hpp
    include/idgen/execution_id.hpp
    include/idgen/instrument_id.hpp
    include/idgen/market_entry_id.hpp
    include/idgen/order_id.hpp
  SOURCES
    src/contexts/execution_id_context.cpp
    src/contexts/instrument_id_context.cpp
    src/contexts/market_entry_id_context.cpp
    src/contexts/order_id_context.cpp
    src/generation.cpp
    src/idgen.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    tl::expected
    ts::common
  PRIVATE_DEPENDENCIES
    fmt::fmt
    simulator::core
    simulator::log)

#------------------------------------------------------------------------------#

add_subdirectory(tests)