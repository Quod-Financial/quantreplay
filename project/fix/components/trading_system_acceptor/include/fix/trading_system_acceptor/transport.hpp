#ifndef SIMULATOR_FIX_TRADING_SYSTEM_ACCEPTOR_TRANSPORT_HPP_
#define SIMULATOR_FIX_TRADING_SYSTEM_ACCEPTOR_TRANSPORT_HPP_

#include "fix/trading_system_acceptor/trading_system_acceptor.hpp"
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

namespace simulator::fix {

auto send_reply(const protocol::BusinessMessageReject& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::ExecutionReport& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderPlacementConfirmation& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderPlacementReject& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderModificationConfirmation& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderModificationReject& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderCancellationConfirmation& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderCancellationReject& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::MarketDataReject& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::MarketDataSnapshot& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::MarketDataUpdate& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::SecurityStatus& reply,
                TradingSystemAcceptor& acceptor) noexcept -> void;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_TRADING_SYSTEM_ACCEPTOR_TRANSPORT_HPP_