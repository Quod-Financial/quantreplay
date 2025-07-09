project(protocol
  LANGUAGES CXX
  DESCRIPTION
    "Market Simulator internal messages definitions")

#------------------------------------------------------------------------------#

ensure_project_dependency_exist(fmt::fmt)
ensure_project_dependency_exist(simulator::core)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${PROJECT_NAME}
  ALIAS simulator::protocol
  HEADERS
    include/protocol/admin/generator.hpp
    include/protocol/admin/market_state.hpp
    include/protocol/admin/trading_phase.hpp
    include/protocol/app/business_message_reject.hpp
    include/protocol/app/execution_report.hpp
    include/protocol/app/instrument_state_request.hpp
    include/protocol/app/market_data_reject.hpp
    include/protocol/app/market_data_request.hpp
    include/protocol/app/market_data_snapshot.hpp
    include/protocol/app/market_data_update.hpp
    include/protocol/app/order_cancellation_confirmation.hpp
    include/protocol/app/order_cancellation_reject.hpp
    include/protocol/app/order_cancellation_request.hpp
    include/protocol/app/order_modification_confirmation.hpp
    include/protocol/app/order_modification_reject.hpp
    include/protocol/app/order_modification_request.hpp
    include/protocol/app/order_placement_confirmation.hpp
    include/protocol/app/order_placement_reject.hpp
    include/protocol/app/order_placement_request.hpp
    include/protocol/app/security_status.hpp
    include/protocol/app/security_status_request.hpp
    include/protocol/app/session_terminated_event.hpp
    include/protocol/types/json/session.hpp
    include/protocol/types/session.hpp
  SOURCES
    src/admin.cpp
    src/app.cpp
    src/types.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/include
  PUBLIC_DEPENDENCIES
    fmt::fmt
    simulator::core)

#------------------------------------------------------------------------------#

add_subdirectory(tests)
