#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_PRICE_SEED_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_PRICE_SEED_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <string>
#include <utility>

#include "ih/data_bridge/price_seed_accessor.hpp"
#include "ih/data_bridge/setting_accessor.hpp"

namespace simulator::http {

class PriceSeedController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  PriceSeedController(
      std::unique_ptr<data_bridge::PriceSeedAccessor> seed_accessor,
      std::shared_ptr<data_bridge::SettingAccessor> setting_accessor) noexcept;

  [[nodiscard]]
  auto select_price_seed(std::uint64_t seed_id) const -> Result;

  [[nodiscard]]
  auto select_all_price_seeds() const -> Result;

  [[nodiscard]]
  auto insert_price_seed(const std::string& body) const -> Result;

  [[nodiscard]]
  auto update_price_seed(std::uint64_t seed_id, const std::string& body) const
      -> Result;

  [[nodiscard]]
  auto delete_price_seed(std::uint64_t seed_id) const -> Result;

  [[nodiscard]]
  auto sync_price_seeds() const -> Result;

 private:
  static auto format_error_response(data_bridge::Failure failure)
      -> std::string;

  std::unique_ptr<data_bridge::PriceSeedAccessor> seed_accessor_;
  std::shared_ptr<data_bridge::SettingAccessor> setting_accessor_;

  static const std::string ConnectionSetting;
  static const std::string SyncTimeSetting;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_PRICE_SEED_CONTROLLER_HPP_
