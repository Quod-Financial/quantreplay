project(core
  LANGUAGES CXX
  DESCRIPTION
    "Market Simulator common utilities and domain-specific types definition")


#------------------------------------------------------------------------------#

add_static_library(
  NAME ${PROJECT_NAME}
  ALIAS simulator::core
  HEADERS
    include/core/domain/json/instrument_descriptor.hpp
    include/core/domain/json/market_phase.hpp
    include/core/domain/json/party.hpp
    include/core/domain/attributes.hpp
    include/core/domain/enumerators.hpp
    include/core/domain/instrument_descriptor.hpp
    include/core/domain/market_data_entry.hpp
    include/core/domain/market_phase.hpp
    include/core/domain/party.hpp
    include/core/common/json/field.hpp
    include/core/common/json/type.hpp
    include/core/common/json/type_container.hpp
    include/core/common/json/type_enum.hpp
    include/core/common/json/type_struct.hpp
    include/core/common/attribute.hpp
    include/core/common/enum_converter.hpp
    include/core/common/meta.hpp
    include/core/common/name.hpp
    include/core/common/return_code.hpp
    include/core/common/unreachable.hpp
    include/core/common/std_formatter.hpp
    include/core/tools/format.hpp
    include/core/tools/numeric.hpp
    include/core/tools/overload.hpp
    include/core/tools/string_checkers.hpp
    include/core/tools/time.hpp
    include/core/version.hpp
  SOURCES
    src/common/json/type.cpp
    src/common/return_code.cpp
    src/domain/attributes.cpp
    src/domain/composites.cpp
    src/domain/enumerators.cpp
    src/domain/market_phase.cpp
    src/domain/party.cpp
    src/tools/string_checkers.cpp
    src/tools/time.cpp
    src/version.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/include
  PUBLIC_DEPENDENCIES
    RapidJSON::RapidJSON
    fmt::fmt
  PRIVATE_DEPENDENCIES
    date::date
  PRIVATE_COMPILE_DEFINITIONS
    $<$<BOOL:${SIM_VERSION_MAJOR}>:SIM_VERSION_MAJOR="${SIM_VERSION_MAJOR}">
    $<$<BOOL:${SIM_VERSION_REV_HASH}>:SIM_VERSION_REV_HASH="${SIM_VERSION_REV_HASH}">
    $<$<BOOL:${SIM_VERSION_REV_HUMAN_READABLE_TIMESTAMP}>:SIM_VERSION_REV_HUMAN_READABLE_TIMESTAMP="${SIM_VERSION_REV_HUMAN_READABLE_TIMESTAMP}">
    $<$<BOOL:${SIM_VERSION_REV_UNIX_TIMESTAMP}>:SIM_VERSION_REV_UNIX_TIMESTAMP="${SIM_VERSION_REV_UNIX_TIMESTAMP}">
)

#------------------------------------------------------------------------------#

add_subdirectory(tests)