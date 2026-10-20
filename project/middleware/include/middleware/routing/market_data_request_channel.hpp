#ifndef SIMULATOR_MIDDLEWARE_ROUTING_MARKET_DATA_REQUEST_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_ROUTING_MARKET_DATA_REQUEST_CHANNEL_HPP_

#include "protocol/app/market_data_request.hpp"

namespace simulator::middleware {

auto send_market_data_request(protocol::MarketDataRequest request) -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_ROUTING_MARKET_DATA_REQUEST_CHANNEL_HPP_
