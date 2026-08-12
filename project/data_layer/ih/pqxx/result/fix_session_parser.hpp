#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_FIX_SESSION_PARSER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_FIX_SESSION_PARSER_HPP_

#include <pqxx/row>

#include "api/inspectors/fix_session.hpp"
#include "api/models/fix_session.hpp"
#include "ih/pqxx/result/detail/basic_row_parser.hpp"

namespace simulator::data_layer::internal_pqxx {

class FixSessionParser {
 public:
  explicit FixSessionParser(const pqxx::row& database_row) noexcept
      : row_parser_{database_row} {}

  auto parse_into(FixSession::Patch& destination_patch) -> void {
    FixSessionPatchWriter<decltype(row_parser_)> writer{row_parser_};
    writer.write(destination_patch);
  }

  static auto parse(const pqxx::row& database_row) -> FixSession::Patch {
    FixSession::Patch parsed;
    FixSessionParser parser{database_row};
    parser.parse_into(parsed);
    return parsed;
  }

 private:
  detail::BasicRowParser row_parser_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_FIX_SESSION_PARSER_HPP_
