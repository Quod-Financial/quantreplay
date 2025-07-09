project(trading_system
  LANGUAGES CXX
  DESCRIPTION
    "Market Simulator venue trading system implementation")

#------------------------------------------------------------------------------#

add_subdirectory(components/common)
add_subdirectory(components/idgen)
add_subdirectory(components/instruments)
add_subdirectory(components/runtime)
add_subdirectory(components/ies)
add_subdirectory(components/matching_engine)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${PROJECT_NAME}
  ALIAS simulator::trading_system
  HEADERS
    ih/config/config.hpp
    ih/config/phase_entry_reader.hpp
    ih/config/venue_entry_reader.hpp
    ih/execution/execution_system.hpp
    ih/execution/reject_notifier.hpp
    ih/repository/repository_accessor.hpp
    ih/repository/trading_engines_repository.hpp
    ih/state_persistence/market_state_persistence_controller.hpp
    ih/state_persistence/serializer.hpp
    ih/tools/instrument_resolver.hpp
    ih/tools/loaders.hpp
    ih/tools/trading_engine_factory.hpp
    ih/trading_system.hpp
    ih/trading_system_facade.hpp
    include/trading_system/trading_system.hpp
  SOURCES
    src/config.cpp
    src/execution/execution_system.cpp
    src/execution/reject_notifier.cpp
    src/repository/repository_accessor.cpp
    src/repository/trading_engines_repository.cpp
    src/state_persistence/market_state_persistence_controller.cpp
    src/state_persistence/serializer.cpp
    src/tools/instrument_resolver.cpp
    src/tools/loaders.cpp
    src/tools/trading_engine_factory.cpp
    src/trading_system.cpp
    src/trading_system_facade.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    simulator::protocol
  PRIVATE_DEPENDENCIES
    ts::idgen
    ts::ies
    ts::runtime
    ts::instruments
    ts::matching_engine
    simulator::middleware
    simulator::log
    simulator::data_layer
    date::date
    fmt::fmt
    tl::function-ref
    Microsoft.GSL::GSL)

#------------------------------------------------------------------------------#

add_subdirectory(tests)