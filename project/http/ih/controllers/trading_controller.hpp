#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_TRADING_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_TRADING_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <string>
#include <utility>

namespace simulator::http {

class TradingController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  [[nodiscard]]
  auto halt(const std::string& body) const -> Result;

  [[nodiscard]]
  auto resume() const -> Result;

  [[nodiscard]]
  auto store_market_state() const -> Result;

  [[nodiscard]]
  auto recover_market_state() const -> Result;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_TRADING_CONTROLLER_HPP_
