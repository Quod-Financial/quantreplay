#include "ih/pqxx/common/enumeration_resolver.hpp"

#include <string>
#include <string_view>

#include "api/models/venue.hpp"
#include "ih/common/exceptions.hpp"
#include "ih/pqxx/common/names/enumerations.hpp"

namespace simulator::data_layer::internal_pqxx {

auto EnumerationResolver::operator()(Datasource::Format format) const
    -> std::string {
  return EnumerationResolver::to_string(format);
}

auto EnumerationResolver::operator()(std::string_view format,
                                     Datasource::Format& value) const -> void {
  EnumerationResolver::from_string(format, value);
}

auto EnumerationResolver::operator()(Datasource::Type type) const
    -> std::string {
  return EnumerationResolver::to_string(type);
}

auto EnumerationResolver::operator()(std::string_view type,
                                     Datasource::Type& value) const -> void {
  EnumerationResolver::from_string(type, value);
}

auto EnumerationResolver::operator()(MarketPhase::Phase phase) const
    -> std::string {
  return to_string(phase);
}

auto EnumerationResolver::operator()(std::string_view phase,
                                     MarketPhase::Phase& value) const -> void {
  EnumerationResolver::from_string(phase, value);
}

auto EnumerationResolver::operator()(Venue::EngineType engine_type) const
    -> std::string {
  return EnumerationResolver::to_string(engine_type);
}

auto EnumerationResolver::operator()(std::string_view engine_type,
                                     data_layer::Venue::EngineType& value) const
    -> void {
  EnumerationResolver::from_string(engine_type, value);
}

auto EnumerationResolver::to_string(Datasource::Format format) -> std::string {
  std::string_view string{};
  switch (format) {
    case Datasource::Format::Csv:
      string = internal_pqxx::datasource_format::Csv;
      break;
    case Datasource::Format::Postgres:
      string = internal_pqxx::datasource_format::Postgres;
      break;
  }

  if (!string.empty()) {
    return std::string{string};
  }
  throw EnumEncodingError("Datasource::Format", format);
}

auto EnumerationResolver::from_string(std::string_view format,
                                      Datasource::Format& value) -> void {
  if (format == internal_pqxx::datasource_format::Csv) {
    value = Datasource::Format::Csv;
  } else if (format == internal_pqxx::datasource_format::Postgres) {
    value = Datasource::Format::Postgres;
  } else {
    throw EnumDecodingError("Datasource::Format", format);
  }
}

auto EnumerationResolver::to_string(Datasource::Type type) -> std::string {
  std::string_view string{};
  switch (type) {
    case Datasource::Type::OrderBook:
      string = internal_pqxx::datasource_type::OrderBook;
      break;
  }

  if (!string.empty()) {
    return std::string{string};
  }
  throw EnumEncodingError("Datasource::Type", type);
}

auto EnumerationResolver::from_string(std::string_view type,
                                      Datasource::Type& value) -> void {
  if (type == internal_pqxx::datasource_type::OrderBook) {
    value = Datasource::Type::OrderBook;
  } else {
    throw EnumDecodingError("Datasource::Type", type);
  }
}

auto EnumerationResolver::to_string(MarketPhase::Phase value) -> std::string {
  std::string_view string{};
  switch (value) {
    case MarketPhase::Phase::Open:
      string = internal_pqxx::market_phase_type::Open;
      break;
    case MarketPhase::Phase::Closed:
      string = internal_pqxx::market_phase_type::Closed;
      break;
    case MarketPhase::Phase::OpeningAuction:
      string = internal_pqxx::market_phase_type::OpeningAuction;
      break;
    case MarketPhase::Phase::ClosingAuction:
      string = internal_pqxx::market_phase_type::ClosingAuction;
      break;
    case MarketPhase::Phase::IntradayAuction:
      string = internal_pqxx::market_phase_type::IntradayAuction;
      break;
    case MarketPhase::Phase::Halted:
      string = internal_pqxx::market_phase_type::Halted;
      break;
  }

  if (!string.empty()) {
    return std::string{string};
  }
  throw EnumEncodingError("MarketPhase::Phase", value);
}

auto EnumerationResolver::from_string(std::string_view string,
                                      MarketPhase::Phase& value) -> void {
  if (string == internal_pqxx::market_phase_type::Open) {
    value = MarketPhase::Phase::Open;
  } else if (string == internal_pqxx::market_phase_type::Closed) {
    value = MarketPhase::Phase::Closed;
  } else if (string == market_phase_type::OpeningAuction) {
    value = MarketPhase::Phase::OpeningAuction;
  } else if (string == market_phase_type::ClosingAuction) {
    value = MarketPhase::Phase::ClosingAuction;
  } else if (string == market_phase_type::IntradayAuction) {
    value = MarketPhase::Phase::IntradayAuction;
  } else if (string == market_phase_type::Halted) {
    value = MarketPhase::Phase::Halted;
  } else {
    throw EnumDecodingError("MarketPhase::Phase", string);
  }
}

auto EnumerationResolver::to_string(Venue::EngineType value) -> std::string {
  std::string_view string{};
  switch (value) {
    case Venue::EngineType::Matching:
      string = internal_pqxx::venue_engine_type::Matching;
      break;
    case Venue::EngineType::Quoting:
      string = internal_pqxx::venue_engine_type::Quoting;
      break;
  }

  if (!string.empty()) {
    return std::string{string};
  }
  throw EnumEncodingError("Venue::EngineType", value);
}

auto EnumerationResolver::from_string(std::string_view string,
                                      data_layer::Venue::EngineType& value)
    -> void {
  if (string == internal_pqxx::venue_engine_type::Matching) {
    value = Venue::EngineType::Matching;
  } else if (string == internal_pqxx::venue_engine_type::Quoting) {
    value = Venue::EngineType::Quoting;
  } else {
    throw EnumDecodingError("Venue::EngineType", string);
  }
}

}  // namespace simulator::data_layer::internal_pqxx
