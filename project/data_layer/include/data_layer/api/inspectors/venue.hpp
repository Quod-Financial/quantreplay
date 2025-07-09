#ifndef SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_VENUE_HPP_
#define SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_VENUE_HPP_

#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

#include "data_layer/api/models/venue.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class VenueReader final {
  using Attribute = Venue::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit VenueReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const Venue& venue) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Marshaller>
class VenuePatchReader final {
  using Attribute = Venue::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit VenuePatchReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const Venue::Patch& patch) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class VenuePatchWriter final {
  using Attribute = Venue::Attribute;

  template <typename T>
  constexpr inline static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit VenuePatchWriter(Unmarshaller& unmarshaller) noexcept
      : unmarshaller_(unmarshaller) {}

  auto write(Venue::Patch& patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
auto VenueReader<Marshaller>::read(const Venue& venue) -> void {
  const auto& venue_id = venue.venue_id();
  static_assert(can_marshall_v<decltype(venue_id)>);
  marshaller_(Attribute::VenueId, venue_id);

  if (const auto& value = venue.name()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Name, *value);
  }

  if (const auto& value = venue.engine_type()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::EngineType, *value);
  }

  if (const auto& value = venue.support_tif_ioc_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SupportTifIoc, *value);
  }

  if (const auto& value = venue.support_tif_fok_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SupportTifFok, *value);
  }

  if (const auto& value = venue.support_tif_day_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SupportTifDay, *value);
  }

  if (const auto& value = venue.include_own_orders_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::IncludeOwnOrders, *value);
  }

  if (const auto& value = venue.rest_port()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RestPort, *value);
  }

  if (const auto& value = venue.order_on_startup_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::OrderOnStartup, *value);
  }

  if (const auto& value = venue.random_parties_count()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomPartiesCount, *value);
  }

  if (const auto& value = venue.tns_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TnsEnabled, *value);
  }

  if (const auto& value = venue.tns_qty_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TnsQtyEnabled, *value);
  }

  if (const auto& value = venue.tns_side_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TnsSideEnabled, *value);
  }

  if (const auto& value = venue.tns_parties_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TnsPartiesEnabled, *value);
  }

  if (const auto& value = venue.timezone()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Timezone, *value);
  }

  if (const auto& value = venue.cancel_on_disconnect_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::CancelOnDisconnect, *value);
  }

  if (const auto& value = venue.persistence_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PersistenceEnabled, *value);
  }

  if (const auto& value = venue.persistence_file_path()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PersistenceFilePath, *value);
  }
}

template <typename Marshaller>
inline auto VenuePatchReader<Marshaller>::read(const Venue::Patch& patch)
    -> void {
  if (const auto& value = patch.venue_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::VenueId, *value);
  }

  if (const auto& value = patch.name()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Name, *value);
  }

  if (const auto& value = patch.engine_type()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::EngineType, *value);
  }

  if (const auto& value = patch.support_tif_ioc_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SupportTifIoc, *value);
  }

  if (const auto& value = patch.support_tif_fok_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SupportTifFok, *value);
  }

  if (const auto& value = patch.support_tif_day_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SupportTifDay, *value);
  }

  if (const auto& value = patch.include_own_orders_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::IncludeOwnOrders, *value);
  }

  if (const auto& value = patch.rest_port()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RestPort, *value);
  }

  if (const auto& value = patch.orders_on_startup_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::OrderOnStartup, *value);
  }

  if (const auto& value = patch.random_parties_count()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomPartiesCount, *value);
  }

  if (const auto& value = patch.tns_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TnsEnabled, *value);
  }

  if (const auto& value = patch.tns_qty_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TnsQtyEnabled, *value);
  }

  if (const auto& value = patch.tns_side_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TnsSideEnabled, *value);
  }

  if (const auto& value = patch.tns_parties_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TnsPartiesEnabled, *value);
  }

  if (const auto& value = patch.timezone()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Timezone, *value);
  }

  if (const auto& value = patch.cancel_on_disconnect_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::CancelOnDisconnect, *value);
  }

  if (const auto& value = patch.persistence_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PersistenceEnabled, *value);
  }

  if (const auto& value = patch.persistence_file_path()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PersistenceFilePath, *value);
  }
}

template <typename Unmarshaller>
inline auto VenuePatchWriter<Unmarshaller>::write(Venue::Patch& patch) -> void {
  std::string venue_id{};
  static_assert(can_unmarshall_v<decltype(venue_id)>);
  if (unmarshaller_(Attribute::VenueId, venue_id)) {
    patch.with_venue_id(std::move(venue_id));
  }

  std::string name{};
  static_assert(can_unmarshall_v<decltype(name)>);
  if (unmarshaller_(Attribute::Name, name)) {
    patch.with_name(std::move(name));
  }

  Venue::EngineType engine_type{};
  static_assert(can_unmarshall_v<decltype(engine_type)>);
  if (unmarshaller_(Attribute::EngineType, engine_type)) {
    patch.with_engine_type(engine_type);
  }

  bool support_tif_ioc{};
  static_assert(can_unmarshall_v<decltype(support_tif_ioc)>);
  if (unmarshaller_(Attribute::SupportTifIoc, support_tif_ioc)) {
    patch.with_support_tif_ioc_flag(support_tif_ioc);
  }

  bool support_tif_fok{};
  static_assert(can_unmarshall_v<decltype(support_tif_fok)>);
  if (unmarshaller_(Attribute::SupportTifFok, support_tif_fok)) {
    patch.with_support_tif_fok_flag(support_tif_fok);
  }

  bool support_tif_day{};
  static_assert(can_unmarshall_v<decltype(support_tif_day)>);
  if (unmarshaller_(Attribute::SupportTifDay, support_tif_day)) {
    patch.with_support_tif_day_flag(support_tif_day);
  }

  bool include_own_orders{};
  static_assert(can_unmarshall_v<decltype(include_own_orders)>);
  if (unmarshaller_(Attribute::IncludeOwnOrders, include_own_orders)) {
    patch.with_include_own_orders_flag(include_own_orders);
  }

  std::uint16_t rest_port{};
  static_assert(can_unmarshall_v<decltype(rest_port)>);
  if (unmarshaller_(Attribute::RestPort, rest_port)) {
    patch.with_rest_port(rest_port);
  }

  bool orders_on_startup{};
  static_assert(can_unmarshall_v<decltype(orders_on_startup)>);
  if (unmarshaller_(Attribute::OrderOnStartup, orders_on_startup)) {
    patch.with_orders_on_startup_flag(orders_on_startup);
  }

  std::uint32_t random_parties_count{};
  static_assert(can_unmarshall_v<decltype(random_parties_count)>);
  if (unmarshaller_(Attribute::RandomPartiesCount, random_parties_count)) {
    patch.with_random_parties_count(random_parties_count);
  }

  bool tns_enabled{};
  static_assert(can_unmarshall_v<decltype(tns_enabled)>);
  if (unmarshaller_(Attribute::TnsEnabled, tns_enabled)) {
    patch.with_tns_enabled_flag(tns_enabled);
  }

  bool tns_qty_enabled{};
  static_assert(can_unmarshall_v<decltype(tns_qty_enabled)>);
  if (unmarshaller_(Attribute::TnsQtyEnabled, tns_qty_enabled)) {
    patch.with_tns_qty_enabled_flag(tns_qty_enabled);
  }

  bool tns_side_enabled{};
  static_assert(can_unmarshall_v<decltype(tns_side_enabled)>);
  if (unmarshaller_(Attribute::TnsSideEnabled, tns_side_enabled)) {
    patch.with_tns_side_enabled_flag(tns_side_enabled);
  }

  bool tns_parties_enabled{};
  static_assert(can_unmarshall_v<decltype(tns_parties_enabled)>);
  if (unmarshaller_(Attribute::TnsPartiesEnabled, tns_parties_enabled)) {
    patch.with_tns_parties_enabled_flag(tns_parties_enabled);
  }

  std::string timezone{};
  static_assert(can_unmarshall_v<decltype(timezone)>);
  if (unmarshaller_(Attribute::Timezone, timezone)) {
    patch.with_timezone(std::move(timezone));
  }

  bool cancel_on_disconnect{};
  static_assert(can_unmarshall_v<decltype(cancel_on_disconnect)>);
  if (unmarshaller_(Attribute::CancelOnDisconnect, cancel_on_disconnect)) {
    patch.with_cancel_on_disconnect_flag(cancel_on_disconnect);
  }

  bool persistence_enabled{};
  static_assert(can_unmarshall_v<decltype(persistence_enabled)>);
  if (unmarshaller_(Attribute::PersistenceEnabled, persistence_enabled)) {
    patch.with_persistence_enabled_flag(persistence_enabled);
  }

  std::string persistence_file_path{};
  static_assert(can_unmarshall_v<decltype(persistence_file_path)>);
  if (unmarshaller_(Attribute::PersistenceFilePath, persistence_file_path)) {
    patch.with_persistence_file_path(std::move(persistence_file_path));
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_VENUE_HPP_
