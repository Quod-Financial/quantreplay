#ifndef SIMULATOR_TRADING_SYSTEM_ACCEPTOR_IH_MAPPING_FROM_FIX_MAPPER_HPP_
#define SIMULATOR_TRADING_SYSTEM_ACCEPTOR_IH_MAPPING_FROM_FIX_MAPPER_HPP_

#include <quickfix/Message.h>

#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"

namespace simulator::fix::trading_system_acceptor {

struct FromFixMapper {
  static auto map(const FIX::Message& fix_message,
                  protocol::OrderPlacementRequest& request) -> void;

  static auto map(const FIX::Message& fix_message,
                  protocol::OrderModificationRequest& request) -> void;

  static auto map(const FIX::Message& fix_message,
                  protocol::OrderCancellationRequest& request) -> void;

  static auto map(const FIX::Message& fix_message,
                  protocol::MarketDataRequest& request) -> void;

  static auto map(const FIX::Message& fix_message,
                  protocol::SecurityStatusRequest& request) -> void;
};

}  // namespace simulator::fix::trading_system_acceptor

#endif  // SIMULATOR_TRADING_SYSTEM_ACCEPTOR_IH_MAPPING_FROM_FIX_MAPPER_HPP_
