#ifndef SIMULATOR_APP_IH_DISPATCHERS_MARKET_DATA_REQUEST_DISPATCHER_HPP_
#define SIMULATOR_APP_IH_DISPATCHERS_MARKET_DATA_REQUEST_DISPATCHER_HPP_

#include <memory>
#include <vector>

#include "middleware/channels/market_data_request_channel.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator {

// Forwards each market data request to the receiver routed for the request's
// FIX session and drops a request whose session has no route.
class MarketDataRequestDispatcher final
    : public middleware::MarketDataRequestReceiver {
 public:
  // Sessions are unique across routes, as fix::create_generator_initiator
  // refuses a settings file whose session is already in use.
  struct Route {
    protocol::fix::Session session;
    std::shared_ptr<middleware::MarketDataRequestReceiver> receiver;
  };

  explicit MarketDataRequestDispatcher(std::vector<Route> routes) noexcept;

  auto process(protocol::MarketDataRequest request) -> void override;

 private:
  std::vector<Route> routes_;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_DISPATCHERS_MARKET_DATA_REQUEST_DISPATCHER_HPP_
