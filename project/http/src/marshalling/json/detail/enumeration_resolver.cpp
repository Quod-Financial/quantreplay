#include "ih/marshalling/json/detail/enumeration_resolver.hpp"

#include <fmt/format.h>

#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace simulator::http::json {
namespace datasource_format {

constexpr std::string_view Csv{"CSV"};
constexpr std::string_view Postgres{"PSQL"};

}  // namespace datasource_format

namespace datasource_type {

constexpr std::string_view OrderBook{"OrderBook"};

}  // namespace datasource_type

namespace market_phase_type {

constexpr std::string_view Open{"Open"};
constexpr std::string_view Closed{"Closed"};
constexpr std::string_view OpeningAuction{"PreOpen"};
constexpr std::string_view ClosingAuction{"PreClose"};
constexpr std::string_view IntradayAuction{"Auction"};
constexpr std::string_view Halted{"Halted"};

}  // namespace market_phase_type

namespace venue_engine_type {

constexpr std::string_view Matching{"Matching"};
constexpr std::string_view Quoting{"Quoting"};

}  // namespace venue_engine_type

namespace {

template <typename Enumeration>
[[noreturn]]
auto raise_bad_enumeration_error(std::string_view enum_name,
                                 Enumeration bad_value) -> void {
  static_assert(std::is_enum_v<Enumeration>);
  using Underlying = std::underlying_type_t<Enumeration>;

  throw std::logic_error{fmt::format(
      "unable to resolve a JSON value for {} enumeration by an undefined "
      "value `{:#x}'",
      enum_name,
      static_cast<Underlying>(bad_value))};
}

[[noreturn]]
auto raise_bad_value_error(std::string_view enum_name,
                           std::string_view bad_value) -> void {
  throw std::runtime_error{
      fmt::format("unable to decode a `{}' JSON string as a {} enumeration",
                  bad_value,
                  enum_name)};
}

}  // namespace

auto EnumerationResolver::resolve(data_layer::Datasource::Format value)
    -> std::string_view {
  switch (value) {
    case data_layer::Datasource::Format::Csv:
      return datasource_format::Csv;
    case data_layer::Datasource::Format::Postgres:
      return datasource_format::Postgres;
  }

  raise_bad_enumeration_error("Datasource::Format", value);
}

auto EnumerationResolver::resolve(const std::string& value,
                                  data_layer::Datasource::Format& result)
    -> void {
  if (value == datasource_format::Csv) {
    result = data_layer::Datasource::Format::Csv;
  } else if (value == datasource_format::Postgres) {
    result = data_layer::Datasource::Format::Postgres;
  } else {
    raise_bad_value_error("Datasource::Format", value);
  }
}

auto EnumerationResolver::resolve(data_layer::Datasource::Type value)
    -> std::string_view {
  switch (value) {
    case data_layer::Datasource::Type::OrderBook:
      return datasource_type::OrderBook;
      break;
  }

  raise_bad_enumeration_error("Datasource::Type", value);
}

auto EnumerationResolver::resolve(const std::string& value,
                                  data_layer::Datasource::Type& result)
    -> void {
  if (value == datasource_type::OrderBook) {
    result = data_layer::Datasource::Type::OrderBook;
  } else {
    raise_bad_value_error("Datasource::Type", value);
  }
}

auto EnumerationResolver::resolve(data_layer::MarketPhase::Phase value)
    -> std::string_view {
  switch (value) {
    case data_layer::MarketPhase::Phase::Open:
      return market_phase_type::Open;
    case data_layer::MarketPhase::Phase::Closed:
      return market_phase_type::Closed;
    case data_layer::MarketPhase::Phase::OpeningAuction:
      return market_phase_type::OpeningAuction;
    case data_layer::MarketPhase::Phase::ClosingAuction:
      return market_phase_type::ClosingAuction;
    case data_layer::MarketPhase::Phase::IntradayAuction:
      return market_phase_type::IntradayAuction;
    case data_layer::MarketPhase::Phase::Halted:
      return market_phase_type::Halted;
  }

  raise_bad_enumeration_error("MarketPhase::Phase", value);
}

auto EnumerationResolver::resolve(const std::string& value,
                                  data_layer::MarketPhase::Phase& result)
    -> void {
  if (value == market_phase_type::Open) {
    result = data_layer::MarketPhase::Phase::Open;
  } else if (value == market_phase_type::Closed) {
    result = data_layer::MarketPhase::Phase::Closed;
  } else if (value == market_phase_type::OpeningAuction) {
    result = data_layer::MarketPhase::Phase::OpeningAuction;
  } else if (value == market_phase_type::ClosingAuction) {
    result = data_layer::MarketPhase::Phase::ClosingAuction;
  } else if (value == market_phase_type::IntradayAuction) {
    result = data_layer::MarketPhase::Phase::IntradayAuction;
  } else if (value == market_phase_type::Halted) {
    result = data_layer::MarketPhase::Phase::Halted;
  } else {
    raise_bad_value_error("MarketPhase::Phase", value);
  }
}

auto EnumerationResolver::resolve(data_layer::Venue::EngineType value)
    -> std::string_view {
  switch (value) {
    case data_layer::Venue::EngineType::Matching:
      return venue_engine_type::Matching;
    case data_layer::Venue::EngineType::Quoting:
      return venue_engine_type::Quoting;
  }

  raise_bad_enumeration_error("Venue::EngineType", value);
}

auto EnumerationResolver::resolve(const std::string& value,
                                  data_layer::Venue::EngineType& result)
    -> void {
  if (value == venue_engine_type::Matching) {
    result = data_layer::Venue::EngineType::Matching;
  } else if (value == venue_engine_type::Quoting) {
    result = data_layer::Venue::EngineType::Quoting;
  } else {
    raise_bad_value_error("Venue::EngineType", value);
  }
}

}  // namespace simulator::http::json
