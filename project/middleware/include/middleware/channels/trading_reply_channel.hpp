#ifndef SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_REPLY_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_REPLY_CHANNEL_HPP_

#include <memory>

#include "detail/receiver.hpp"
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

struct TradingReplyReceiver : public detail::Receiver {
  virtual auto process(protocol::BusinessMessageReject reject) -> void = 0;
  virtual auto process(protocol::ExecutionReport report) -> void = 0;

  virtual auto process(protocol::OrderPlacementReject reject) -> void = 0;
  virtual auto process(protocol::OrderPlacementConfirmation confirmation)
      -> void = 0;

  virtual auto process(protocol::OrderModificationReject reject) -> void = 0;
  virtual auto process(protocol::OrderModificationConfirmation confirmation)
      -> void = 0;

  virtual auto process(protocol::OrderCancellationReject reject) -> void = 0;
  virtual auto process(protocol::OrderCancellationConfirmation confirmation)
      -> void = 0;

  virtual auto process(protocol::MarketDataReject reject) -> void = 0;
  virtual auto process(protocol::MarketDataSnapshot snapshot) -> void = 0;
  virtual auto process(protocol::MarketDataUpdate update) -> void = 0;
  virtual auto process(protocol::SecurityStatus status) -> void = 0;
};

// Allows the receiver receiving messages sent via the channel,
// unbinds previous receiver from the channel if any.
auto bind_trading_reply_channel(std::shared_ptr<TradingReplyReceiver> receiver)
    -> void;

// Unbinds previous receiver from the channel if any.
auto release_trading_reply_channel() noexcept -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_REPLY_CHANNEL_HPP_