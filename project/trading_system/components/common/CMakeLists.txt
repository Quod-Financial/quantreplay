set(COMPONENT_NAME ${PROJECT_NAME}_common)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS ts::common
  HEADERS
    include/common/attributes.hpp
    include/common/checker_utils.hpp
    include/common/events.hpp
    include/common/instrument.hpp
    include/common/instrument_state.hpp
    include/common/market_data_rejects.hpp
    include/common/order_modifying_engine.hpp
    include/common/order_rejects.hpp
    include/common/phase.hpp
    include/common/phase_aware_engine.hpp
    include/common/quote_publishing_engine.hpp
    include/common/security_status_publisher.hpp
    include/common/state_persisting_engine.hpp
    include/common/trade.hpp
    include/common/trading_engine.hpp
    include/common/validation.hpp
  SOURCES
    src/attributes.cpp
    src/instrument.cpp
    src/instrument_state.cpp
    src/market_data_rejects.cpp
    src/order_rejects.cpp
    src/phase.cpp
    src/security_status_publisher.cpp
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
