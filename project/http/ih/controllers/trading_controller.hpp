#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_TRADING_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_TRADING_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <string>
#include <utility>

namespace simulator::http {

class TradingController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  virtual ~TradingController() = default;

  [[nodiscard]]
  virtual auto halt(const std::string& body) const -> Result = 0;

  [[nodiscard]]
  virtual auto resume() const -> Result = 0;

  [[nodiscard]]
  virtual auto store_market_state() const -> Result = 0;

  [[nodiscard]]
  virtual auto recover_market_state() const -> Result = 0;
};

class TradingControllerImpl : public TradingController {
 public:
  [[nodiscard]]
  auto halt(const std::string& body) const -> Result override;

  [[nodiscard]]
  auto resume() const -> Result override;

  [[nodiscard]]
  auto store_market_state() const -> Result override;

  [[nodiscard]]
  auto recover_market_state() const -> Result override;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_TRADING_CONTROLLER_HPP_
