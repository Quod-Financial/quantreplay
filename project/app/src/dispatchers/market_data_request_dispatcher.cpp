#include "ih/dispatchers/market_data_request_dispatcher.hpp"

#include <algorithm>
#include <utility>
#include <variant>
#include <vector>

#include "log/logging.hpp"

namespace simulator {
namespace {

[[nodiscard]]
auto serves(const MarketDataRequestDispatcher::Route& route,
            const protocol::Session& session) -> bool {
  const auto* fix_session =
      std::get_if<protocol::fix::Session>(&session.value);
  return fix_session != nullptr && *fix_session == route.session;
}

}  // namespace

MarketDataRequestDispatcher::MarketDataRequestDispatcher(
    std::vector<Route> routes) noexcept
    : routes_{std::move(routes)} {}

auto MarketDataRequestDispatcher::process(protocol::MarketDataRequest request)
    -> void {
  const auto route =
      std::ranges::find_if(routes_, [&request](const Route& candidate) {
        return serves(candidate, request.session);
      });
  if (route == routes_.end()) {
    log::err("no FIX initiator serves session {}, undelivered message - {}",
             request.session,
             request);
    return;
  }
  route->receiver->process(std::move(request));
}

}  // namespace simulator
