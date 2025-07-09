#include "api/models/column_mapping.hpp"

#include <string>
#include <utility>

namespace simulator::data_layer {

auto ColumnMapping::create(ColumnMapping::Patch snapshot,
                           std::uint64_t datasource_id) noexcept
    -> ColumnMapping {
  ColumnMapping column_mapping;
  column_mapping.column_from_ = std::move(snapshot.column_from_);
  column_mapping.column_to_ = std::move(snapshot.column_to_);
  column_mapping.datasource_id_ = datasource_id;
  return column_mapping;
}

auto ColumnMapping::column_from() const noexcept -> const std::string& {
  return column_from_;
}

auto ColumnMapping::column_to() const noexcept -> const std::string& {
  return column_to_;
}

auto ColumnMapping::datasource_id() const noexcept -> std::uint64_t {
  return datasource_id_;
}

auto ColumnMapping::Patch::column_from() const noexcept -> const std::string& {
  return column_from_;
}

auto ColumnMapping::Patch::with_column_from(std::string column) noexcept
    -> Patch& {
  column_from_ = std::move(column);
  return *this;
}

auto ColumnMapping::Patch::column_to() const noexcept -> const std::string& {
  return column_to_;
}

auto ColumnMapping::Patch::with_column_to(std::string column) noexcept
    -> Patch& {
  column_to_ = std::move(column);
  return *this;
}

}  // namespace simulator::data_layer
