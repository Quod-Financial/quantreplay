#ifndef SIMULATOR_DATA_LAYER_API_INSPECTORS_FIX_SESSION_HPP_
#define SIMULATOR_DATA_LAYER_API_INSPECTORS_FIX_SESSION_HPP_

#include <string>
#include <type_traits>
#include <utility>

#include "data_layer/api/models/fix_session.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class FixSessionReader final {
  using Attribute = FixSession::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit FixSessionReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const FixSession& fix_session) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Marshaller>
class FixSessionPatchReader final {
  using Attribute = FixSession::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit FixSessionPatchReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const FixSession::Patch& patch) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class FixSessionPatchWriter final {
  using Attribute = FixSession::Attribute;

  template <typename T>
  constexpr inline static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit FixSessionPatchWriter(Unmarshaller& unmarshaller) noexcept
      : unmarshaller_(unmarshaller) {}

  auto write(FixSession::Patch& patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
auto FixSessionReader<Marshaller>::read(const FixSession& fix_session) -> void {
  const std::string& venue_id = fix_session.venue_id();
  static_assert(can_marshall_v<decltype(venue_id)>);
  marshaller_(Attribute::VenueId, venue_id);

  const std::string& session_id = fix_session.session_id();
  static_assert(can_marshall_v<decltype(session_id)>);
  marshaller_(Attribute::SessionId, session_id);

  if (const auto& value = fix_session.last_connected_time()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::LastConnectedTime, *value);
  }
}

template <typename Marshaller>
inline auto FixSessionPatchReader<Marshaller>::read(
    const FixSession::Patch& patch) -> void {
  if (const auto& value = patch.venue_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::VenueId, *value);
  }

  if (const auto& value = patch.session_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SessionId, *value);
  }

  if (const auto& value = patch.last_connected_time()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::LastConnectedTime, *value);
  }
}

template <typename Unmarshaller>
inline auto FixSessionPatchWriter<Unmarshaller>::write(FixSession::Patch& patch)
    -> void {
  std::string venue_id;
  static_assert(can_unmarshall_v<decltype(venue_id)>);
  if (unmarshaller_(Attribute::VenueId, venue_id)) {
    patch.with_venue_id(std::move(venue_id));
  }

  std::string session_id;
  static_assert(can_unmarshall_v<decltype(session_id)>);
  if (unmarshaller_(Attribute::SessionId, session_id)) {
    patch.with_session_id(std::move(session_id));
  }

  core::sys_us last_connected_time;
  static_assert(can_unmarshall_v<decltype(last_connected_time)>);
  if (unmarshaller_(Attribute::LastConnectedTime, last_connected_time)) {
    patch.with_last_connected_time(last_connected_time);
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_API_INSPECTORS_FIX_SESSION_HPP_
