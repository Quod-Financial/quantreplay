#include "ih/orders/actions/auction_indicative_reporter.hpp"

#include <utility>

#include "ih/common/data/market_data_updates.hpp"
#include "ih/common/events/order_book_notification.hpp"

namespace simulator::trading_system::matching_engine::order {

AuctionIndicativeReporter::AuctionIndicativeReporter(
    EventListener& event_listener, const PhaseHandler& phase_handler)
    : EventReporter{event_listener}, phase_handler_{&phase_handler} {}

auto AuctionIndicativeReporter::operator()(
    const std::optional<AuctionResult>& auction_result) const -> void {
  if (!phase_handler_->in_auction_call()) {
    return;
  }

  AuctionIndicativeUpdate update{
      .auction_phase = phase_handler_->current_phase().trading_phase(),
      .price_qty = std::nullopt,
      .imbalance = std::nullopt};
  if (auction_result.has_value()) {
    update.price_qty = AuctionIndicativeUpdate::IndicativePriceQuantity{
        .price = auction_result->price, .quantity = auction_result->quantity};
    update.imbalance = AuctionIndicativeUpdate::Imbalance{
        .size = auction_result->imbalance,
        .side = auction_result->imbalance_side};
  } else {
    update.price_qty = AuctionIndicativeUpdate::IndicativePriceQuantity{
        .price = std::nullopt, .quantity = Quantity{0}};
  }

  emit(OrderBookNotification{std::move(update)});
}

auto AuctionIndicativeReporter::report_cleared() const -> void {
  emit(OrderBookNotification{AuctionIndicativeUpdate{}});
}

}  // namespace simulator::trading_system::matching_engine::order
