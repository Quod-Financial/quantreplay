#ifndef SIMULATOR_DATA_LAYER_API_INSPECTORS_PRICE_TICK_RANGE_HPP_
#define SIMULATOR_DATA_LAYER_API_INSPECTORS_PRICE_TICK_RANGE_HPP_

#include <functional>
#include <optional>
#include <type_traits>

#include "data_layer/api/models/price_tick_range.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class PriceTickRangeReader final {
  using Attribute = PriceTickRange::Attribute;

  template <typename T>
  constexpr static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit PriceTickRangeReader(Marshaller& marshaller) noexcept
      : marshaller_{marshaller} {}

  auto read(const PriceTickRange& range) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class PriceTickRangePatchWriter final {
  using Attribute = PriceTickRange::Attribute;

  template <typename T>
  constexpr static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit PriceTickRangePatchWriter(Unmarshaller& unmarshaller) noexcept
      : unmarshaller_{unmarshaller} {}

  auto write(PriceTickRange::Patch& patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
inline auto PriceTickRangeReader<Marshaller>::read(const PriceTickRange& range)
    -> void {
  static_assert(can_marshall_v<decltype(range.tick_size())>);
  marshaller_(Attribute::TickSize, range.tick_size());

  if (const auto upper_limit = range.upper_limit()) {
    static_assert(can_marshall_v<decltype(*upper_limit)>);
    marshaller_(Attribute::UpperLimit, *upper_limit);
  }
}

template <typename Unmarshaller>
inline auto PriceTickRangePatchWriter<Unmarshaller>::write(
    PriceTickRange::Patch& patch) -> void {
  std::optional<double> tick_size;
  static_assert(can_unmarshall_v<decltype(tick_size)>);
  if (unmarshaller_(Attribute::TickSize, tick_size) && tick_size.has_value()) {
    patch.with_tick_size(*tick_size);
  }

  std::optional<double> upper_limit;
  static_assert(can_unmarshall_v<decltype(upper_limit)>);
  if (unmarshaller_(Attribute::UpperLimit, upper_limit)) {
    patch.with_upper_limit(upper_limit);
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_API_INSPECTORS_PRICE_TICK_RANGE_HPP_
