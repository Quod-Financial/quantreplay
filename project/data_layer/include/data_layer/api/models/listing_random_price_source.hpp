#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_LISTING_RANDOM_PRICE_SOURCE_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_LISTING_RANDOM_PRICE_SOURCE_HPP_

#include <cstdint>
#include <string>

namespace simulator::data_layer {

class ListingRandomPriceSource {
 public:
  class Patch;

  enum class Attribute { ListingId, DatasourceId, Symbol };

  [[nodiscard]]
  static auto create(ListingRandomPriceSource::Patch snapshot,
                     std::uint64_t listing_id) noexcept
      -> ListingRandomPriceSource;

  [[nodiscard]]
  auto listing_id() const noexcept -> std::uint64_t;

  [[nodiscard]]
  auto datasource_id() const noexcept -> std::uint64_t;

  [[nodiscard]]
  auto symbol() const noexcept -> const std::string&;

  [[nodiscard]]
  auto operator==(const ListingRandomPriceSource&) const -> bool = default;

 private:
  ListingRandomPriceSource() = default;

  std::uint64_t listing_id_{0};
  std::uint64_t datasource_id_{0};
  std::string symbol_;
};

class ListingRandomPriceSource::Patch {
  friend class ListingRandomPriceSource;

 public:
  using Attribute = ListingRandomPriceSource::Attribute;

  [[nodiscard]]
  auto datasource_id() const noexcept -> std::uint64_t;
  auto with_datasource_id(std::uint64_t datasource_id) noexcept -> Patch&;

  [[nodiscard]]
  auto symbol() const noexcept -> const std::string&;
  auto with_symbol(std::string symbol) noexcept -> Patch&;

  [[nodiscard]]
  auto operator==(const Patch&) const -> bool = default;

 private:
  std::uint64_t datasource_id_{0};
  std::string symbol_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_LISTING_RANDOM_PRICE_SOURCE_HPP_
