set(COMPONENT_NAME ${PROJECT_NAME}_common)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS ts::common
  HEADERS
    include/common/json/instrument.hpp
    include/common/json/trade.hpp
    include/common/market_state/json/instrument_info.hpp
    include/common/market_state/json/instrument_state.hpp
    include/common/market_state/json/limit_order.hpp
    include/common/market_state/json/order_book.hpp
    include/common/market_state/json/session.hpp
    include/common/market_state/json/snapshot.hpp
    include/common/market_state/snapshot.hpp
    include/common/attributes.hpp
    include/common/events.hpp
    include/common/instrument.hpp
    include/common/phase.hpp
    include/common/trade.hpp
    include/common/trading_engine.hpp
  SOURCES
    src/attributes.cpp
    src/instrument.cpp
    src/phase.cpp
    src/snapshot.cpp
    src/trade.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    simulator::core
    simulator::protocol)

#------------------------------------------------------------------------------#

add_subdirectory(tests)
