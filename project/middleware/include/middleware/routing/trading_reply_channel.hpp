#ifndef SIMULATOR_MIDDLEWARE_ROUTING_TRADING_REPLY_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_ROUTING_TRADING_REPLY_CHANNEL_HPP_

#include "middleware/routing/errors.hpp"
#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/security_status.hpp"

namespace simulator::middleware {

auto send_trading_reply(protocol::BusinessMessageReject reply) -> void;

auto send_trading_reply(protocol::ExecutionReport reply) -> void;

auto send_trading_reply(protocol::OrderPlacementReject reply) -> void;

auto send_trading_reply(protocol::OrderPlacementConfirmation reply) -> void;

auto send_trading_reply(protocol::OrderModificationReject reply) -> void;

auto send_trading_reply(protocol::OrderModificationConfirmation reply) -> void;

auto send_trading_reply(protocol::OrderCancellationReject reply) -> void;

auto send_trading_reply(protocol::OrderCancellationConfirmation reply) -> void;

auto send_trading_reply(protocol::MarketDataReject reply) -> void;

auto send_trading_reply(protocol::MarketDataSnapshot reply) -> void;

auto send_trading_reply(protocol::MarketDataUpdate reply) -> void;

auto send_trading_reply(protocol::SecurityStatus reply) -> void;

}  // namespace simulator::middleware

#endif  // #define SIMULATOR_MIDDLEWARE_ROUTING_TRADING_REPLY_CHANNEL_HPP_