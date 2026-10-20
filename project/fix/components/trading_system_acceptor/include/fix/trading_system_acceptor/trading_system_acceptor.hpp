#ifndef SIMULATOR_FIX_TRADING_SYSTEM_ACCEPTOR_TRADING_SYSTEM_ACCEPTOR_HPP_
#define SIMULATOR_FIX_TRADING_SYSTEM_ACCEPTOR_TRADING_SYSTEM_ACCEPTOR_HPP_

#include <memory>

namespace simulator::fix {

struct TradingSystemAcceptor {
  struct Implementation;

  explicit TradingSystemAcceptor(std::unique_ptr<Implementation> impl) noexcept;
  TradingSystemAcceptor(const TradingSystemAcceptor&) = delete;
  TradingSystemAcceptor(TradingSystemAcceptor&&) noexcept;
  ~TradingSystemAcceptor() noexcept;

  auto operator=(const TradingSystemAcceptor&)
      -> TradingSystemAcceptor& = delete;
  auto operator=(TradingSystemAcceptor&&) noexcept -> TradingSystemAcceptor&;

  auto implementation() noexcept -> Implementation&;

 private:
  std::unique_ptr<Implementation> impl_;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_TRADING_SYSTEM_ACCEPTOR_TRADING_SYSTEM_ACCEPTOR_HPP_