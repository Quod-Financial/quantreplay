#include "fix/trading_system_acceptor/trading_system_acceptor.hpp"

#include <cstdlib>
#include <memory>
#include <utility>

#include "ih/implementation.hpp"
#include "log/logging.hpp"

namespace simulator::fix {

TradingSystemAcceptor::TradingSystemAcceptor(
    std::unique_ptr<Implementation> impl) noexcept
    : impl_(std::move(impl)) {}

TradingSystemAcceptor::TradingSystemAcceptor(
    simulator::fix::TradingSystemAcceptor&&) noexcept = default;

TradingSystemAcceptor::~TradingSystemAcceptor() noexcept = default;

auto TradingSystemAcceptor::operator=(
    simulator::fix::TradingSystemAcceptor&&) noexcept
    -> TradingSystemAcceptor& = default;

auto TradingSystemAcceptor::implementation() noexcept -> Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  log::err(
      "fix acceptor implementation has not been allocated/initialized, "
      "this may indicate a critical bug in the component, can not continue "
      "execution, aborting...");

  std::abort();
}

}  // namespace simulator::fix
