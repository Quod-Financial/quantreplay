set(COMPONENT_NAME ${PROJECT_NAME}_instruments)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS ts::instruments
  HEADERS
    ih/lookup/currency_category.hpp
    ih/lookup/lookup.hpp
    ih/lookup/match_rate.hpp
    ih/lookup/matchers.hpp
    ih/lookup/strategies.hpp
    ih/instruments_cache.hpp
    ih/instruments_container.hpp
    ih/instruments_matcher.hpp
    include/instruments/cache.hpp
    include/instruments/lookup_error.hpp
    include/instruments/matcher.hpp
    include/instruments/sources.hpp
    include/instruments/view.hpp
  SOURCES
    src/lookup/lookup.cpp
    src/lookup/matchers.cpp
    src/lookup/strategies.cpp
    src/instruments.cpp
    src/instruments_cache.cpp
    src/instruments_matcher.cpp
    src/sources.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    ts::common
    simulator::core
    simulator::data_layer
    tl::expected
  PRIVATE_DEPENDENCIES
    ts::idgen
    fmt::fmt
    simulator::log
    simulator::cfg)

#------------------------------------------------------------------------------#

add_subdirectory(tests)