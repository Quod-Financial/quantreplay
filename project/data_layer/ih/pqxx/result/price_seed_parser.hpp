#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_PRICE_SEED_PARSER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_PRICE_SEED_PARSER_HPP_

#include <pqxx/row>

#include "api/exceptions/exceptions.hpp"
#include "api/inspectors/price_seed.hpp"
#include "api/models/price_seed.hpp"
#include "ih/pqxx/result/detail/basic_row_parser.hpp"

namespace simulator::data_layer::internal_pqxx {

class PriceSeedParser {
 public:
  explicit PriceSeedParser(const pqxx::row& database_row) noexcept
      : row_parser_(database_row) {}

  auto parse_into(PriceSeed::Patch& destination_patch) -> void {
    PriceSeedPatchWriter<decltype(row_parser_)> writer{row_parser_};
    writer.write(destination_patch);
  }

  static auto parse(const pqxx::row& database_row) -> PriceSeed::Patch {
    PriceSeed::Patch parsed{};
    PriceSeedParser parser{database_row};
    parser.parse_into(parsed);
    return parsed;
  }

  static auto parse_identifier(const pqxx::row& database_row) -> std::uint64_t {
    // Warning:
    // GCC reports false-positive warning
    // that row_parser is used before being initialized
    // in release builds
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

    detail::BasicRowParser row_parser{database_row};

    std::uint64_t price_seed_id{};
    if (row_parser(PriceSeed::Attribute::PriceSeedId, price_seed_id)) {
      return price_seed_id;
    }

    throw DataDecodingError(
        "failed to decode a price seed record identifier "
        " from the database row");

#pragma GCC diagnostic pop
  }

 private:
  detail::BasicRowParser row_parser_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_PRICE_SEED_PARSER_HPP_
