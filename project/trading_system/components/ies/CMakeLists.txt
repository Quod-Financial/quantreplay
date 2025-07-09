set(COMPONENT_NAME ${PROJECT_NAME}_ies)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS ts::ies
  HEADERS
    ih/phases/phase_scheduler.hpp
    ih/phases/states.hpp
    ih/controller_impl.hpp
    ih/system_tick_controller.hpp
    ih/tick_event_factory.hpp
    ih/trading_phase_controller.hpp
    include/ies/controller.hpp
    include/ies/phase_record.hpp
    include/ies/phase_schedule.hpp
  SOURCES
    src/phases/states.cpp
    src/controller.cpp
    src/phase_schedule.cpp
    src/trading_phase_controller.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_DEPENDENCIES
    ts::common
    ts::runtime
    fmt::fmt
  PRIVATE_DEPENDENCIES
    simulator::core
    simulator::log)

#------------------------------------------------------------------------------#

add_subdirectory(tests)
