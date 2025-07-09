#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_COLUMN_MAPPING_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_COLUMN_MAPPING_HPP_

#include <cstdint>
#include <string>

namespace simulator::data_layer {

class ColumnMapping {
 public:
  class Patch;

  enum class Attribute { ColumnFrom, ColumnTo, DatasourceId };

  [[nodiscard]]
  static auto create(ColumnMapping::Patch snapshot,
                     std::uint64_t datasource_id) noexcept -> ColumnMapping;

  [[nodiscard]]
  auto column_from() const noexcept -> const std::string&;

  [[nodiscard]]
  auto column_to() const noexcept -> const std::string&;

  [[nodiscard]]
  auto datasource_id() const noexcept -> std::uint64_t;

  [[nodiscard]]
  auto operator==(const ColumnMapping&) const -> bool = default;

 private:
  ColumnMapping() = default;

  std::string column_from_;
  std::string column_to_;
  std::uint64_t datasource_id_{0};
};

class ColumnMapping::Patch {
  friend class ColumnMapping;

 public:
  using Attribute = ColumnMapping::Attribute;

  [[nodiscard]]
  auto column_from() const noexcept -> const std::string&;
  auto with_column_from(std::string column) noexcept -> Patch&;

  [[nodiscard]]
  auto column_to() const noexcept -> const std::string&;
  auto with_column_to(std::string column) noexcept -> Patch&;

  [[nodiscard]]
  auto operator==(const Patch&) const -> bool = default;

 private:
  std::string column_from_;
  std::string column_to_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_COLUMN_MAPPING_HPP_
