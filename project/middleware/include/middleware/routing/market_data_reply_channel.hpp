#ifndef SIMULATOR_MIDDLEWARE_ROUTING_MARKET_DATA_REPLY_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_ROUTING_MARKET_DATA_REPLY_CHANNEL_HPP_

#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"

namespace simulator::middleware {

auto send_market_data_reply(protocol::MarketDataSnapshot snapshot) -> void;

auto send_market_data_reply(protocol::MarketDataUpdate update) -> void;

auto send_market_data_reply(protocol::MarketDataReject reject) -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_ROUTING_MARKET_DATA_REPLY_CHANNEL_HPP_
