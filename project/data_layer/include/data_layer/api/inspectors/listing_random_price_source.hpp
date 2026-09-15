#ifndef SIMULATOR_DATA_LAYER_API_INSPECTORS_LISTING_RANDOM_PRICE_SOURCE_HPP_
#define SIMULATOR_DATA_LAYER_API_INSPECTORS_LISTING_RANDOM_PRICE_SOURCE_HPP_

#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include "data_layer/api/models/listing_random_price_source.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class ListingRandomPriceSourceReader final {
  using Attribute = ListingRandomPriceSource::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit ListingRandomPriceSourceReader(Marshaller& marshaller) noexcept
      : marshaller_{marshaller} {}

  auto read(const ListingRandomPriceSource& source) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class ListingRandomPriceSourcePatchWriter final {
  using Attribute = ListingRandomPriceSource::Attribute;

  template <typename T>
  constexpr inline static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit ListingRandomPriceSourcePatchWriter(
      Unmarshaller& unmarshaller) noexcept
      : unmarshaller_{unmarshaller} {}

  auto write(ListingRandomPriceSource::Patch& patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
inline auto ListingRandomPriceSourceReader<Marshaller>::read(
    const ListingRandomPriceSource& source) -> void {
  static_assert(can_marshall_v<decltype(source.listing_id())>);
  marshaller_(Attribute::ListingId, source.listing_id());

  static_assert(can_marshall_v<decltype(source.datasource_id())>);
  marshaller_(Attribute::DatasourceId, source.datasource_id());

  static_assert(can_marshall_v<decltype(source.symbol())>);
  marshaller_(Attribute::Symbol, source.symbol());
}

template <typename Unmarshaller>
inline auto ListingRandomPriceSourcePatchWriter<Unmarshaller>::write(
    ListingRandomPriceSource::Patch& patch) -> void {
  std::uint64_t datasource_id{};
  static_assert(can_unmarshall_v<decltype(datasource_id)>);
  if (unmarshaller_(Attribute::DatasourceId, datasource_id)) {
    patch.with_datasource_id(datasource_id);
  }

  std::string symbol;
  static_assert(can_unmarshall_v<decltype(symbol)>);
  if (unmarshaller_(Attribute::Symbol, symbol)) {
    patch.with_symbol(std::move(symbol));
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_API_INSPECTORS_LISTING_RANDOM_PRICE_SOURCE_HPP_
