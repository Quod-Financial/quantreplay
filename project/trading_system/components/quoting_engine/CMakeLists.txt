set(COMPONENT_NAME ${PROJECT_NAME}_quoting_engine)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS ts::quoting_engine
  HEADERS
    ih/market_data/quote_streamer.hpp
    ih/orders/order_rejecter.hpp
    ih/quotes/quote.hpp
    ih/quotes/quote_book.hpp
    ih/quotes/quote_validator.hpp
    ih/implementation.hpp
    include/quoting_engine/quote_id_generator.hpp
    include/quoting_engine/quoting_engine.hpp
  SOURCES
    src/market_data/quote_streamer.cpp
    src/orders/order_rejecter.cpp
    src/quotes/quote_book.cpp
    src/quotes/quote_id_generator.cpp
    src/quotes/quote_validator.cpp
    src/implementation.cpp
    src/quoting_engine.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    ts::common
    ts::idgen
    ts::runtime
    simulator::protocol
  PRIVATE_DEPENDENCIES
    fmt::fmt
    Microsoft.GSL::GSL
    simulator::core
    simulator::middleware
    simulator::log)

#------------------------------------------------------------------------------#

add_subdirectory(tests)
