#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_CSV_NO_HEADER_CONFIGURATOR_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_CSV_NO_HEADER_CONFIGURATOR_HPP_

#include <cstdint>
#include <map>
#include <string>
#include <tl/expected.hpp>

#include "data_layer/api/converters/column_mapping.hpp"
#include "ih/historical/mapping/specification.hpp"

namespace simulator::generator::historical::mapping {

class CsvNoHeaderConfigurator {
 public:
  explicit CsvNoHeaderConfigurator(std::uint32_t column_count,
                                   std::uint32_t max_depth_levels);

  auto configure(const std::map<data_layer::converter::ColumnFrom, std::string>&
                     column_configs) const
      -> tl::expected<Specification, std::string>;

 private:
  std::uint32_t column_count_;
  std::uint32_t max_depth_levels_;
};

}  // namespace simulator::generator::historical::mapping

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_CSV_NO_HEADER_CONFIGURATOR_HPP_
