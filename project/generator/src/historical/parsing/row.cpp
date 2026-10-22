#include "ih/historical/parsing/row.hpp"

#include <fmt/format.h>

#include <stdexcept>

namespace simulator::generator::historical {

auto Row::operator[](std::size_t index) const -> std::string_view {
  if (index < columns()) {
    return cells_[index];
  }

  throw std::out_of_range(
      fmt::format("column index {} is out of historical row column count {}",
                  index,
                  columns()));
}

}  // namespace simulator::generator::historical
