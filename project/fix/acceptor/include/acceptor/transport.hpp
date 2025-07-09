#ifndef SIMULATOR_FIX_ACCEPTOR_TRANSPORT_HPP_
#define SIMULATOR_FIX_ACCEPTOR_TRANSPORT_HPP_

#include "acceptor/acceptor.hpp"
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
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::ExecutionReport& reply,
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderPlacementConfirmation& reply,
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderPlacementReject& reply,
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderModificationConfirmation& reply,
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderModificationReject& reply,
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderCancellationConfirmation& reply,
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::OrderCancellationReject& reply,
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::MarketDataReject& reply,
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::MarketDataSnapshot& reply,
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::MarketDataUpdate& reply,
                Acceptor& acceptor) noexcept -> void;

auto send_reply(const protocol::SecurityStatus& reply,
                Acceptor& acceptor) noexcept -> void;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_ACCEPTOR_TRANSPORT_HPP_