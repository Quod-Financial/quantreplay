#ifndef SIMULATOR_DATA_LAYER_API_INSPECTORS_DATASOURCE_LISTING_HPP_
#define SIMULATOR_DATA_LAYER_API_INSPECTORS_DATASOURCE_LISTING_HPP_

#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include "data_layer/api/models/datasource_listing.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class DatasourceListingReader final {
  using Attribute = DatasourceListing::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit DatasourceListingReader(Marshaller& marshaller) noexcept
      : marshaller_{marshaller} {}

  auto read(const DatasourceListing& listing) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class DatasourceListingPatchWriter final {
  using Attribute = DatasourceListing::Attribute;

  template <typename T>
  constexpr inline static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit DatasourceListingPatchWriter(Unmarshaller& unmarshaller) noexcept
      : unmarshaller_{unmarshaller} {}

  auto write(DatasourceListing::Patch& patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
inline auto DatasourceListingReader<Marshaller>::read(
    const DatasourceListing& listing) -> void {
  static_assert(can_marshall_v<decltype(listing.datasource_id())>);
  marshaller_(Attribute::DatasourceId, listing.datasource_id());

  static_assert(can_marshall_v<decltype(listing.symbol())>);
  marshaller_(Attribute::Symbol, listing.symbol());
}

template <typename Unmarshaller>
inline auto DatasourceListingPatchWriter<Unmarshaller>::write(
    DatasourceListing::Patch& patch) -> void {
  std::string symbol;
  static_assert(can_unmarshall_v<decltype(symbol)>);
  if (unmarshaller_(Attribute::Symbol, symbol)) {
    patch.with_symbol(std::move(symbol));
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_API_INSPECTORS_DATASOURCE_LISTING_HPP_
