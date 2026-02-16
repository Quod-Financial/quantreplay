set(COMPONENT_NAME ${PROJECT_NAME}_common)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS ts::common
  HEADERS
    include/common/attributes.hpp
    include/common/events.hpp
    include/common/instrument.hpp
    include/common/instrument_state.hpp
    include/common/phase.hpp
    include/common/trade.hpp
    include/common/trading_engine.hpp
  SOURCES
    src/attributes.cpp
    src/instrument.cpp
    src/instrument_state.cpp
    src/phase.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    simulator::core
    simulator::protocol
  PRIVATE_DEPENDENCIES
    date::date
    tl::expected
)

#------------------------------------------------------------------------------#

add_subdirectory(tests)
