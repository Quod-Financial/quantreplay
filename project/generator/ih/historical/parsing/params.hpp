#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_PARSING_PARAMS_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_PARSING_PARAMS_HPP_

#include <cstdint>
#include <string>

#include "ih/constants.hpp"

namespace simulator::data_layer {

class Datasource;

}  // namespace simulator::data_layer

namespace simulator::generator::historical {

class GeneralDatasourceParams {
 public:
  [[nodiscard]]
  auto name() const noexcept -> const std::string& {
    return name_;
  }

  auto set_name(std::string name) noexcept -> void { name_ = std::move(name); }

  [[nodiscard]]
  auto connection() const noexcept -> const std::string& {
    return connection_;
  }

  auto set_connection(std::string connection) -> void {
    connection_ = std::move(connection);
  }

  [[nodiscard]]
  auto max_depth_levels() const noexcept -> std::uint32_t {
    return max_depth_levels_;
  }

  auto set_max_depth_levels(std::uint32_t levels) noexcept -> void {
    max_depth_levels_ = levels;
  }

 private:
  std::string connection_;
  std::string name_;
  std::uint32_t max_depth_levels_{constant::historical::AllDepthLevels};
};

class CsvParsingParams {
  constexpr static char DefaultDelimiter = ',';
  constexpr static std::uint64_t DefaultHeaderRow = 0;
  constexpr static std::uint64_t DefaultDataRow = 1;

 public:
  explicit CsvParsingParams(GeneralDatasourceParams params) noexcept
      : general_params_{std::move(params)} {}

  [[nodiscard]]
  auto datasource_name() const noexcept -> const std::string& {
    return general_params_.name();
  }

  [[nodiscard]]
  auto datasource_connection() const noexcept -> const std::string& {
    return general_params_.connection();
  }

  [[nodiscard]]
  auto datasource_max_depth_levels() const noexcept -> std::uint32_t {
    return general_params_.max_depth_levels();
  }

  [[nodiscard]]
  auto has_header_row() const noexcept -> bool {
    return header_row_ != 0;
  }

  [[nodiscard]]
  auto header_row() const noexcept -> std::uint64_t {
    return header_row_;
  }

  auto set_header_row(std::uint64_t row_number) noexcept {
    header_row_ = row_number;
  }

  [[nodiscard]]
  auto data_row() const noexcept -> std::uint64_t {
    return data_row_;
  }

  auto set_data_row(std::uint64_t row_number) noexcept -> void {
    data_row_ = row_number;
  }

  [[nodiscard]]
  auto delimiter() const noexcept -> char {
    return delimiter_;
  }

  auto set_delimiter(char delimiter) noexcept -> void {
    delimiter_ = delimiter;
  }

 private:
  GeneralDatasourceParams general_params_;
  std::uint64_t header_row_{DefaultHeaderRow};
  std::uint64_t data_row_{DefaultDataRow};
  char delimiter_{DefaultDelimiter};
};

class DatabaseParsingParams {
 public:
  explicit DatabaseParsingParams(GeneralDatasourceParams params) noexcept
      : general_params_{std::move(params)} {}

  [[nodiscard]]
  auto datasource_name() const noexcept -> const std::string& {
    return general_params_.name();
  }

  [[nodiscard]]
  auto datasource_connection() const noexcept -> const std::string& {
    return general_params_.connection();
  }

  [[nodiscard]]
  auto datasource_max_depth_levels() const noexcept -> std::uint32_t {
    return general_params_.max_depth_levels();
  }

  [[nodiscard]]
  auto table_name() const noexcept -> std::string {
    return table_name_;
  }

  auto set_table_name(std::string name) noexcept -> void {
    table_name_ = std::move(name);
  }

 private:
  GeneralDatasourceParams general_params_;
  std::string table_name_;
};

auto make_csv_parsing_params(const data_layer::Datasource& datasource)
    -> CsvParsingParams;

auto make_database_parsing_params(const data_layer::Datasource& datasource)
    -> DatabaseParsingParams;

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_PARSING_PARAMS_HPP_
