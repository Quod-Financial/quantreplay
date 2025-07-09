#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_ADAPTERS_CSV_READER_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_ADAPTERS_CSV_READER_HPP_

#include <cstddef>
#include <cstdint>
#include <csv.hpp>
#include <memory>
#include <vector>

#include "ih/historical/adapters/data_access_adapter.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/mapping/params.hpp"
#include "ih/historical/parsing/params.hpp"

namespace simulator::generator::historical {

class CsvReader final : public DataAccessAdapter {
 public:
  CsvReader() = delete;

  CsvReader(historical::CsvParsingParams params,
            MappingParams mapping_params,
            csv::CSVReader& reader);

  static auto create(historical::CsvParsingParams parsing_params,
                     historical::MappingParams mapping_params)
      -> std::unique_ptr<CsvReader>;

  static auto make_format(const historical::CsvParsingParams& parsing_params)
      -> csv::CSVFormat;

 private:
  [[nodiscard]]
  auto has_next_record() const noexcept -> bool override;

  auto parse_next_record(historical::Record::Builder& builder) -> void override;

  auto make_depth_config(const csv::CSVReader& reader) const
      -> mapping::DepthConfig;

  auto init_mapping_params(const csv::CSVReader& reader,
                           mapping::DepthConfig depth_config) -> void;

  auto load_csv_data(csv::CSVReader& reader) -> void;

  auto columns_number(const csv::CSVReader& reader) const -> std::uint32_t;

  CsvParsingParams parsing_params_;
  MappingParams mapping_params_;
  std::uint32_t depth_{0};

  std::vector<std::pair<std::uint64_t, csv::CSVRow>> extracted_rows_;

  std::size_t next_row_index_{0};
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_ADAPTERS_CSV_READER_HPP_
