#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_DATASOURCE_PARAMS_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_DATASOURCE_PARAMS_HPP_

#include <optional>

#include "data_layer/api/models/datasource.hpp"

namespace simulator::generator::historical {

class DatasourceParams {
 public:
  static const DatasourceParams Postgres;
  static const DatasourceParams CsvNoHeader;
  static const DatasourceParams CsvHasHeader;

  [[nodiscard]]
  auto operator==(const DatasourceParams& other) const noexcept
      -> bool = default;

 private:
  constexpr DatasourceParams(data_layer::Datasource::Format format,
                             std::optional<bool> has_header_row) noexcept
      : format_(format), has_header_row_(has_header_row) {}

  data_layer::Datasource::Format format_;
  std::optional<bool> has_header_row_;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_DATASOURCE_PARAMS_HPP_
