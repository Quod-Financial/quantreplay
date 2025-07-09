#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_PARSING_ROW_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_PARSING_ROW_HPP_

#include <fmt/format.h>

#include <csv.hpp>
#include <pqxx/row>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace simulator::generator::historical {

class Row {
 public:
  static auto from(const csv::CSVRow& csv_row) -> Row {
    Row row;
    for (const csv::CSVField& field : csv_row) {
      const std::string_view value = field.get_sv();
      row.emplace_next(value);
    }
    return row;
  }

  static auto from(const pqxx::row& pq_row) -> Row {
    Row row;
    for (const pqxx::field& field : pq_row) {
      const auto value = field.as<std::string_view>(std::string_view{});
      row.emplace_next(value);
    }
    return row;
  }

  Row() = default;
  explicit Row(std::initializer_list<std::string_view> cells) : cells_{cells} {}

  [[nodiscard]]
  auto columns() const noexcept -> std::size_t {
    return cells_.size();
  }

  [[nodiscard]]
  auto operator[](std::size_t index) const -> std::string_view {
    if (index < columns()) {
      return cells_[index];
    }

    throw std::out_of_range(
        fmt::format("column index {} is out of historical row column count {}",
                    index,
                    columns()));
  }

  auto emplace_next(std::string_view cell) -> void {
    cells_.emplace_back(cell);
  }

 private:
  std::vector<std::string_view> cells_;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_PARSING_ROW_HPP_
