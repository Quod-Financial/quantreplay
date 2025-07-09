#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_VENUE_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_VENUE_HPP_

#include <optional>
#include <string>
#include <vector>

#include "data_layer/api/models/market_phase.hpp"
#include "data_layer/api/predicate/definitions.hpp"

namespace simulator::data_layer {

class Venue {
 public:
  class Patch;

  using Predicate = predicate::Expression<Venue>;

  enum class Attribute {
    VenueId,
    Name,
    EngineType,
    SupportTifIoc,
    SupportTifFok,
    SupportTifDay,
    IncludeOwnOrders,
    RestPort,
    OrderOnStartup,
    RandomPartiesCount,
    TnsEnabled,
    TnsQtyEnabled,
    TnsSideEnabled,
    TnsPartiesEnabled,
    Timezone,
    CancelOnDisconnect,
    PersistenceEnabled,
    PersistenceFilePath,
  };

  enum class EngineType { Matching, Quoting };

  [[nodiscard]]
  static auto create(Patch snapshot) -> Venue;

  [[nodiscard]]
  auto venue_id() const noexcept -> const std::string&;

  [[nodiscard]]
  auto name() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto engine_type() const noexcept -> std::optional<EngineType>;

  [[nodiscard]]
  auto support_tif_ioc_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto support_tif_fok_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto support_tif_day_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto include_own_orders_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto rest_port() const noexcept -> std::optional<std::uint16_t>;

  [[nodiscard]]
  auto order_on_startup_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto random_parties_count() const noexcept
      -> std::optional<std::uint32_t>;

  [[nodiscard]]
  auto tns_enabled_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto tns_qty_enabled_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto tns_side_enabled_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto tns_parties_enabled_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto timezone() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto cancel_on_disconnect_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto persistence_enabled_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto persistence_file_path() const noexcept
      -> const std::optional<std::string>&;

  [[nodiscard]]
  auto market_phases() const noexcept -> const std::vector<MarketPhase>&;

 private:
  Venue() = default;

  std::optional<std::string> name_;
  std::optional<std::string> timezone_;
  std::optional<std::string> persistence_file_path_;

  std::string venue_id_;

  std::vector<data_layer::MarketPhase> market_phases_;

  std::optional<std::uint32_t> random_parties_count_;

  std::optional<EngineType> engine_type_;

  std::optional<std::uint16_t> rest_port_;

  std::optional<bool> support_tif_ioc_flag_;
  std::optional<bool> support_tif_fok_flag_;
  std::optional<bool> support_tif_day_flag_;
  std::optional<bool> include_own_orders_flag_;
  std::optional<bool> order_on_startup_flag_;
  std::optional<bool> tns_enabled_flag_;
  std::optional<bool> tns_qty_enabled_flag_;
  std::optional<bool> tns_side_enabled_flag_;
  std::optional<bool> tns_parties_enabled_flag_;
  std::optional<bool> cancel_on_disconnect_flag_;
  std::optional<bool> persistence_enabled_flag_;
};

class Venue::Patch {
  friend class Venue;

 public:
  using Attribute = Venue::Attribute;

  [[nodiscard]]
  auto venue_id() const noexcept -> const std::optional<std::string>&;
  auto with_venue_id(std::string venue_id) noexcept -> Patch&;

  [[nodiscard]]
  auto name() const noexcept -> const std::optional<std::string>&;
  auto with_name(std::string name) noexcept -> Patch&;

  [[nodiscard]]
  auto engine_type() const noexcept -> std::optional<EngineType>;
  auto with_engine_type(EngineType engine_type) noexcept -> Patch&;

  [[nodiscard]]
  auto support_tif_ioc_flag() const noexcept -> std::optional<bool>;
  auto with_support_tif_ioc_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto support_tif_fok_flag() const noexcept -> std::optional<bool>;
  auto with_support_tif_fok_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto support_tif_day_flag() const noexcept -> std::optional<bool>;
  auto with_support_tif_day_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto include_own_orders_flag() const noexcept -> std::optional<bool>;
  auto with_include_own_orders_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto rest_port() const noexcept -> std::optional<std::uint16_t>;
  auto with_rest_port(std::uint32_t port) noexcept -> Patch&;

  [[nodiscard]]
  auto orders_on_startup_flag() const noexcept -> std::optional<bool>;
  auto with_orders_on_startup_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto random_parties_count() const noexcept
      -> std::optional<std::uint32_t>;
  auto with_random_parties_count(std::uint32_t count) noexcept -> Patch&;

  [[nodiscard]]
  auto tns_enabled_flag() const noexcept -> std::optional<bool>;
  auto with_tns_enabled_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto tns_qty_enabled_flag() const noexcept -> std::optional<bool>;
  auto with_tns_qty_enabled_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto tns_side_enabled_flag() const noexcept -> std::optional<bool>;
  auto with_tns_side_enabled_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto tns_parties_enabled_flag() const noexcept -> std::optional<bool>;
  auto with_tns_parties_enabled_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto timezone() const noexcept -> const std::optional<std::string>&;
  auto with_timezone(std::string timezone) noexcept -> Patch&;

  [[nodiscard]]
  auto cancel_on_disconnect_flag() const noexcept -> std::optional<bool>;
  auto with_cancel_on_disconnect_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto persistence_enabled_flag() const noexcept -> std::optional<bool>;
  auto with_persistence_enabled_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto persistence_file_path() const noexcept
      -> const std::optional<std::string>&;
  auto with_persistence_file_path(std::string path) noexcept -> Patch&;

  [[nodiscard]]
  auto market_phases() const noexcept
      -> const std::optional<std::vector<MarketPhase::Patch>>&;
  auto with_market_phase(MarketPhase::Patch phase_snapshot) -> Patch&;
  auto without_market_phases() noexcept -> Patch&;

 private:
  std::optional<std::string> patched_venue_id_;
  std::optional<std::string> patched_name_;
  std::optional<std::string> patched_timezone_;
  std::optional<std::string> patched_persistence_file_path_;

  std::optional<std::vector<MarketPhase::Patch>> patched_market_phases_;

  std::optional<std::uint32_t> patched_random_parties_count_;

  std::optional<EngineType> patched_engine_type_;

  std::optional<std::uint16_t> patched_rest_port_;

  std::optional<bool> patched_support_tif_ioc_flag_;
  std::optional<bool> patched_support_tif_fok_flag_;
  std::optional<bool> patched_support_tif_day_flag_;
  std::optional<bool> patched_include_own_orders_flag_;
  std::optional<bool> patched_order_on_startup_flag_;
  std::optional<bool> patched_tns_enabled_flag_;
  std::optional<bool> patched_tns_qty_enabled_flag_;
  std::optional<bool> patched_tns_side_enabled_flag_;
  std::optional<bool> patched_tns_parties_enabled_flag_;
  std::optional<bool> patched_cancel_on_disconnect_flag_;
  std::optional<bool> patched_persistence_enabled_flag_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_VENUE_HPP_
