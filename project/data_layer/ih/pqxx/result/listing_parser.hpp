#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_LISTING_PARSER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_LISTING_PARSER_HPP_

#include <pqxx/row>

#include "api/exceptions/exceptions.hpp"
#include "api/inspectors/listing.hpp"
#include "api/models/listing.hpp"
#include "ih/pqxx/result/detail/basic_row_parser.hpp"

namespace simulator::data_layer::internal_pqxx {

class ListingParser {
 public:
  explicit ListingParser(const pqxx::row& database_row) noexcept
      : row_parser_(database_row) {}

  auto parse_into(Listing::Patch& destination_patch) -> void {
    ListingPatchWriter<decltype(row_parser_)> writer{row_parser_};
    writer.write(destination_patch);
  }

  static auto parse(const pqxx::row& database_row) -> Listing::Patch {
    Listing::Patch parsed{};
    ListingParser parser{database_row};
    parser.parse_into(parsed);
    return parsed;
  }

  static auto parse_identifier(const pqxx::row& database_row) -> std::uint64_t {
    detail::BasicRowParser row_parser{database_row};

    std::uint64_t listing_id{};
    if (row_parser(Listing::Attribute::ListingId, listing_id)) {
      return listing_id;
    }

    throw DataDecodingError(
        "failed to decode a listing record identifier "
        "from the database row");
  }

 private:
  detail::BasicRowParser row_parser_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_LISTING_PARSER_HPP_
