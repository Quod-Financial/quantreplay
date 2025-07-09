#ifndef SIMULATOR_PROJECT_DATA_LAYER_IH_PQXX_RESULT_SETTING_PARSER_HPP_
#define SIMULATOR_PROJECT_DATA_LAYER_IH_PQXX_RESULT_SETTING_PARSER_HPP_

#include <pqxx/row>

#include "api/exceptions/exceptions.hpp"
#include "api/inspectors/setting.hpp"
#include "api/models/setting.hpp"
#include "ih/pqxx/result/detail/basic_row_parser.hpp"

namespace simulator::data_layer::internal_pqxx {

class SettingParser {
 public:
  explicit SettingParser(const pqxx::row& database_row) noexcept
      : row_parser_(database_row) {}

  auto parse_into(Setting::Patch& destination_patch) -> void {
    SettingPatchWriter<decltype(row_parser_)> writer{row_parser_};
    writer.write(destination_patch);
  }

  static auto parse(const pqxx::row& database_row) -> Setting::Patch {
    // Warning:
    // GCC reports false-positive warning
    // that parser is used before being initialized
    // in release builds
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

    Setting::Patch parsed{};
    SettingParser parser{database_row};
    parser.parse_into(parsed);
    return parsed;

#pragma GCC diagnostic pop
  }

  static auto parse_key(const pqxx::row& database_row) -> std::string {
    detail::BasicRowParser row_parser{database_row};

    std::string key{};
    if (row_parser(Setting::Attribute::Key, key)) {
      return key;
    }

    throw DataDecodingError(
        "failed to decode a setting record key "
        "from the database row");
  }

 private:
  detail::BasicRowParser row_parser_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_PROJECT_DATA_LAYER_IH_PQXX_RESULT_SETTING_PARSER_HPP_
