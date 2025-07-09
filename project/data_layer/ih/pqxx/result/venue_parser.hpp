#ifndef SIMULATOR_PROJECT_DATA_LAYER_IH_PQXX_RESULT_VENUE_PARSER_HPP_
#define SIMULATOR_PROJECT_DATA_LAYER_IH_PQXX_RESULT_VENUE_PARSER_HPP_

#include <pqxx/row>

#include "api/inspectors/market_phase.hpp"
#include "api/inspectors/venue.hpp"
#include "api/models/market_phase.hpp"
#include "api/models/venue.hpp"
#include "ih/pqxx/result/detail/basic_row_parser.hpp"

namespace simulator::data_layer::internal_pqxx {

class MarketPhaseParser {
 public:
  explicit MarketPhaseParser(const pqxx::row& database_row) noexcept
      : row_parser_(database_row) {}

  auto parse_into(MarketPhase::Patch& destination_patch) -> void {
    MarketPhasePatchWriter<decltype(row_parser_)> writer{row_parser_};
    writer.write(destination_patch);
  }

  static auto parse(const pqxx::row& database_row) -> MarketPhase::Patch {
    MarketPhase::Patch parsed{};
    MarketPhaseParser parser{database_row};
    parser.parse_into(parsed);
    return parsed;
  }

 private:
  detail::BasicRowParser row_parser_;
};

class VenueParser {
 public:
  explicit VenueParser(const pqxx::row& database_row) noexcept
      : row_parser_(database_row) {}

  auto parse_into(Venue::Patch& destination_patch) -> void {
    VenuePatchWriter<decltype(row_parser_)> writer{row_parser_};
    writer.write(destination_patch);
  }

  static auto parse(const pqxx::row& database_row) -> Venue::Patch {
    Venue::Patch parsed{};
    VenueParser parser{database_row};
    parser.parse_into(parsed);
    return parsed;
  }

 private:
  detail::BasicRowParser row_parser_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_PROJECT_DATA_LAYER_IH_PQXX_RESULT_VENUE_PARSER_HPP_
