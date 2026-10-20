#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_DATASOURCE_LISTING_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_DATASOURCE_LISTING_HPP_

#include <cstdint>
#include <string>

namespace simulator::data_layer {

class DatasourceListing {
 public:
  class Patch;

  enum class Attribute { DatasourceId, Symbol };

  [[nodiscard]]
  static auto create(DatasourceListing::Patch snapshot,
                     std::uint64_t datasource_id) noexcept -> DatasourceListing;

  [[nodiscard]]
  auto datasource_id() const noexcept -> std::uint64_t;

  [[nodiscard]]
  auto symbol() const noexcept -> const std::string&;

  [[nodiscard]]
  auto operator==(const DatasourceListing&) const -> bool = default;

 private:
  DatasourceListing() = default;

  std::uint64_t datasource_id_{0};
  std::string symbol_;
};

class DatasourceListing::Patch {
  friend class DatasourceListing;

 public:
  using Attribute = DatasourceListing::Attribute;

  [[nodiscard]]
  auto symbol() const noexcept -> const std::string&;
  auto with_symbol(std::string symbol) noexcept -> Patch&;

  [[nodiscard]]
  auto operator==(const Patch&) const -> bool = default;

 private:
  std::string symbol_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_DATASOURCE_LISTING_HPP_
