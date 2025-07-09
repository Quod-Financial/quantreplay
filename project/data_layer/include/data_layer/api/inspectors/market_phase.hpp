#ifndef SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_MARKET_PHASE_HPP_
#define SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_MARKET_PHASE_HPP_

#include <type_traits>
#include <utility>

#include "data_layer/api/models/market_phase.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class MarketPhaseReader final {
  using Attribute = MarketPhase::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit MarketPhaseReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const MarketPhase& phase) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class MarketPhasePatchWriter final {
  using Attribute = MarketPhase::Attribute;

  template <typename T>
  constexpr inline static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit MarketPhasePatchWriter(Unmarshaller& unmarshaller) noexcept
      : unmarshaller_(unmarshaller) {}

  auto write(MarketPhase::Patch& patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
inline auto MarketPhaseReader<Marshaller>::read(const MarketPhase& phase)
    -> void {
  static_assert(can_marshall_v<decltype(phase.phase())>);
  marshaller_(Attribute::Phase, phase.phase());

  static_assert(can_marshall_v<decltype(phase.start_time())>);
  marshaller_(Attribute::StartTime, phase.start_time());

  static_assert(can_marshall_v<decltype(phase.end_time())>);
  marshaller_(Attribute::EndTime, phase.end_time());

  static_assert(can_marshall_v<decltype(phase.venue_id())>);
  marshaller_(Attribute::VenueId, phase.venue_id());

  if (auto&& value = phase.allow_cancels()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::AllowCancels, *value);
  }

  if (auto&& value = phase.end_time_range()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::EndTimeRange, *value);
  }
}

template <typename Unmarshaller>
inline auto MarketPhasePatchWriter<Unmarshaller>::write(
    MarketPhase::Patch& patch) -> void {
  MarketPhase::Phase phase{};
  static_assert(can_unmarshall_v<decltype(phase)>);
  if (unmarshaller_(Attribute::Phase, phase)) {
    patch.with_phase(phase);
  }

  std::string start_time{};
  static_assert(can_unmarshall_v<decltype(start_time)>);
  if (unmarshaller_(Attribute::StartTime, start_time)) {
    patch.with_start_time(std::move(start_time));
  }

  std::string end_time{};
  static_assert(can_unmarshall_v<decltype(end_time)>);
  if (unmarshaller_(Attribute::EndTime, end_time)) {
    patch.with_end_time(std::move(end_time));
  }

  bool allow_cancels = false;
  static_assert(can_unmarshall_v<decltype(allow_cancels)>);
  if (unmarshaller_(Attribute::AllowCancels, allow_cancels)) {
    patch.with_allow_cancels(allow_cancels);
  }

  std::int32_t end_time_range{};
  static_assert(can_unmarshall_v<decltype(end_time_range)>);
  if (unmarshaller_(Attribute::EndTimeRange, end_time_range)) {
    patch.with_end_time_range(end_time_range);
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_MARKET_PHASE_HPP_
