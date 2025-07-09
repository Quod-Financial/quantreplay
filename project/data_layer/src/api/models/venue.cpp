#include "api/models/venue.hpp"

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <vector>

#include "ih/common/exceptions.hpp"

namespace simulator::data_layer {

auto Venue::create(Patch snapshot) -> Venue {
  if (!snapshot.patched_venue_id_.has_value()) {
    throw RequiredAttributeMissing("Venue", "VenueID");
  }

  Venue venue{};

#define SIM_ASSIGN_FIELD(VENUE_FIELD, SNAPSHOT_FIELD)          \
  if (snapshot.SNAPSHOT_FIELD.has_value()) {                   \
    venue.VENUE_FIELD = std::move(*(snapshot.SNAPSHOT_FIELD)); \
  }                                                            \
  void(0)

  SIM_ASSIGN_FIELD(venue_id_, patched_venue_id_);
  SIM_ASSIGN_FIELD(name_, patched_name_);
  SIM_ASSIGN_FIELD(timezone_, patched_timezone_);
  SIM_ASSIGN_FIELD(persistence_file_path_, patched_persistence_file_path_);
  SIM_ASSIGN_FIELD(random_parties_count_, patched_random_parties_count_);
  SIM_ASSIGN_FIELD(engine_type_, patched_engine_type_);
  SIM_ASSIGN_FIELD(rest_port_, patched_rest_port_);
  SIM_ASSIGN_FIELD(support_tif_ioc_flag_, patched_support_tif_ioc_flag_);
  SIM_ASSIGN_FIELD(support_tif_fok_flag_, patched_support_tif_fok_flag_);
  SIM_ASSIGN_FIELD(support_tif_day_flag_, patched_support_tif_day_flag_);
  SIM_ASSIGN_FIELD(include_own_orders_flag_, patched_include_own_orders_flag_);
  SIM_ASSIGN_FIELD(order_on_startup_flag_, patched_order_on_startup_flag_);
  SIM_ASSIGN_FIELD(tns_enabled_flag_, patched_tns_enabled_flag_);
  SIM_ASSIGN_FIELD(tns_qty_enabled_flag_, patched_tns_qty_enabled_flag_);
  SIM_ASSIGN_FIELD(tns_side_enabled_flag_, patched_tns_side_enabled_flag_);
  SIM_ASSIGN_FIELD(tns_parties_enabled_flag_,
                   patched_tns_parties_enabled_flag_);
  SIM_ASSIGN_FIELD(cancel_on_disconnect_flag_,
                   patched_cancel_on_disconnect_flag_);
  SIM_ASSIGN_FIELD(persistence_enabled_flag_,
                   patched_persistence_enabled_flag_);

#undef SIM_ASSIGN_FIELD

  if (auto& phases = snapshot.patched_market_phases_) {
    venue.market_phases_.reserve(phases->size());
    std::transform(
        std::make_move_iterator(phases->begin()),
        std::make_move_iterator(phases->end()),
        std::back_inserter(venue.market_phases_),
        [&venue](MarketPhase::Patch&& phase_snapshot) -> MarketPhase {
          return MarketPhase::create(std::move(phase_snapshot),
                                     venue.venue_id_);
        });
  }

  return venue;
}

auto Venue::venue_id() const noexcept -> const std::string& {
  return venue_id_;
}

auto Venue::name() const noexcept -> const std::optional<std::string>& {
  return name_;
}

auto Venue::engine_type() const noexcept -> std::optional<EngineType> {
  return engine_type_;
}

auto Venue::support_tif_ioc_flag() const noexcept -> std::optional<bool> {
  return support_tif_ioc_flag_;
}

auto Venue::support_tif_fok_flag() const noexcept -> std::optional<bool> {
  return support_tif_fok_flag_;
}

auto Venue::support_tif_day_flag() const noexcept -> std::optional<bool> {
  return support_tif_day_flag_;
}

auto Venue::include_own_orders_flag() const noexcept -> std::optional<bool> {
  return include_own_orders_flag_;
}

auto Venue::rest_port() const noexcept -> std::optional<std::uint16_t> {
  return rest_port_;
}

auto Venue::order_on_startup_flag() const noexcept -> std::optional<bool> {
  return order_on_startup_flag_;
}

auto Venue::random_parties_count() const noexcept
    -> std::optional<std::uint32_t> {
  return random_parties_count_;
}

auto Venue::tns_enabled_flag() const noexcept -> std::optional<bool> {
  return tns_enabled_flag_;
}

auto Venue::tns_qty_enabled_flag() const noexcept -> std::optional<bool> {
  return tns_qty_enabled_flag_;
}

auto Venue::tns_side_enabled_flag() const noexcept -> std::optional<bool> {
  return tns_side_enabled_flag_;
}

auto Venue::tns_parties_enabled_flag() const noexcept -> std::optional<bool> {
  return tns_parties_enabled_flag_;
}

auto Venue::timezone() const noexcept -> const std::optional<std::string>& {
  return timezone_;
}

auto Venue::cancel_on_disconnect_flag() const noexcept -> std::optional<bool> {
  return cancel_on_disconnect_flag_;
}

auto Venue::persistence_enabled_flag() const noexcept -> std::optional<bool> {
  return persistence_enabled_flag_;
}

auto Venue::persistence_file_path() const noexcept
    -> const std::optional<std::string>& {
  return persistence_file_path_;
}

auto Venue::market_phases() const noexcept -> const std::vector<MarketPhase>& {
  return market_phases_;
}

auto Venue::Patch::venue_id() const noexcept
    -> const std::optional<std::string>& {
  return patched_venue_id_;
}

auto Venue::Patch::with_venue_id(std::string venue_id) noexcept -> Patch& {
  patched_venue_id_ = std::move(venue_id);
  return *this;
}

auto Venue::Patch::name() const noexcept -> const std::optional<std::string>& {
  return patched_name_;
}

auto Venue::Patch::with_name(std::string name) noexcept -> Patch& {
  patched_name_ = std::move(name);
  return *this;
}

auto Venue::Patch::engine_type() const noexcept -> std::optional<EngineType> {
  return patched_engine_type_;
}

auto Venue::Patch::with_engine_type(EngineType engine_type) noexcept -> Patch& {
  patched_engine_type_ = engine_type;
  return *this;
}

auto Venue::Patch::support_tif_ioc_flag() const noexcept
    -> std::optional<bool> {
  return patched_support_tif_ioc_flag_;
}

auto Venue::Patch::with_support_tif_ioc_flag(bool flag) noexcept -> Patch& {
  patched_support_tif_ioc_flag_ = flag;
  return *this;
}

auto Venue::Patch::support_tif_fok_flag() const noexcept
    -> std::optional<bool> {
  return patched_support_tif_fok_flag_;
}

auto Venue::Patch::with_support_tif_fok_flag(bool flag) noexcept -> Patch& {
  patched_support_tif_fok_flag_ = flag;
  return *this;
}

auto Venue::Patch::support_tif_day_flag() const noexcept
    -> std::optional<bool> {
  return patched_support_tif_day_flag_;
}

auto Venue::Patch::with_support_tif_day_flag(bool flag) noexcept -> Patch& {
  patched_support_tif_day_flag_ = flag;
  return *this;
}

auto Venue::Patch::include_own_orders_flag() const noexcept
    -> std::optional<bool> {
  return patched_include_own_orders_flag_;
}

auto Venue::Patch::with_include_own_orders_flag(bool flag) noexcept -> Patch& {
  patched_include_own_orders_flag_ = flag;
  return *this;
}

auto Venue::Patch::rest_port() const noexcept -> std::optional<std::uint16_t> {
  return patched_rest_port_;
}

auto Venue::Patch::with_rest_port(std::uint32_t port) noexcept -> Patch& {
  patched_rest_port_ = port;
  return *this;
}

auto Venue::Patch::orders_on_startup_flag() const noexcept
    -> std::optional<bool> {
  return patched_order_on_startup_flag_;
}

auto Venue::Patch::with_orders_on_startup_flag(bool flag) noexcept -> Patch& {
  patched_order_on_startup_flag_ = flag;
  return *this;
}

auto Venue::Patch::random_parties_count() const noexcept
    -> std::optional<std::uint32_t> {
  return patched_random_parties_count_;
}

auto Venue::Patch::with_random_parties_count(std::uint32_t count) noexcept
    -> Patch& {
  patched_random_parties_count_ = count;
  return *this;
}

auto Venue::Patch::tns_enabled_flag() const noexcept -> std::optional<bool> {
  return patched_tns_enabled_flag_;
}

auto Venue::Patch::with_tns_enabled_flag(bool flag) noexcept -> Patch& {
  patched_tns_enabled_flag_ = flag;
  return *this;
}

auto Venue::Patch::tns_qty_enabled_flag() const noexcept
    -> std::optional<bool> {
  return patched_tns_qty_enabled_flag_;
}

auto Venue::Patch::with_tns_qty_enabled_flag(bool flag) noexcept -> Patch& {
  patched_tns_qty_enabled_flag_ = flag;
  return *this;
}

auto Venue::Patch::tns_side_enabled_flag() const noexcept
    -> std::optional<bool> {
  return patched_tns_side_enabled_flag_;
}

auto Venue::Patch::with_tns_side_enabled_flag(bool flag) noexcept -> Patch& {
  patched_tns_side_enabled_flag_ = flag;
  return *this;
}

auto Venue::Patch::tns_parties_enabled_flag() const noexcept
    -> std::optional<bool> {
  return patched_tns_parties_enabled_flag_;
}

auto Venue::Patch::with_tns_parties_enabled_flag(bool flag) noexcept -> Patch& {
  patched_tns_parties_enabled_flag_ = flag;
  return *this;
}

auto Venue::Patch::timezone() const noexcept
    -> const std::optional<std::string>& {
  return patched_timezone_;
}

auto Venue::Patch::with_timezone(std::string timezone) noexcept -> Patch& {
  patched_timezone_ = std::move(timezone);
  return *this;
}

auto Venue::Patch::cancel_on_disconnect_flag() const noexcept
    -> std::optional<bool> {
  return patched_cancel_on_disconnect_flag_;
}

auto Venue::Patch::with_cancel_on_disconnect_flag(bool flag) noexcept
    -> Patch& {
  patched_cancel_on_disconnect_flag_ = flag;
  return *this;
}

auto Venue::Patch::persistence_enabled_flag() const noexcept
    -> std::optional<bool> {
  return patched_persistence_enabled_flag_;
}

auto Venue::Patch::with_persistence_enabled_flag(bool flag) noexcept -> Patch& {
  patched_persistence_enabled_flag_ = flag;
  return *this;
}

auto Venue::Patch::persistence_file_path() const noexcept
    -> const std::optional<std::string>& {
  return patched_persistence_file_path_;
}

auto Venue::Patch::with_persistence_file_path(std::string path) noexcept
    -> Patch& {
  patched_persistence_file_path_ = std::move(path);
  return *this;
}

auto Venue::Patch::market_phases() const noexcept
    -> const std::optional<std::vector<MarketPhase::Patch>>& {
  return patched_market_phases_;
}

auto Venue::Patch::with_market_phase(MarketPhase::Patch phase_snapshot)
    -> Patch& {
  if (!patched_market_phases_.has_value()) {
    patched_market_phases_ = std::vector<MarketPhase::Patch>{};
  }

  patched_market_phases_->emplace_back(std::move(phase_snapshot));
  return *this;
}

auto Venue::Patch::without_market_phases() noexcept -> Patch& {
  patched_market_phases_ = decltype(patched_market_phases_)::value_type{};
  return *this;
}

}  // namespace simulator::data_layer