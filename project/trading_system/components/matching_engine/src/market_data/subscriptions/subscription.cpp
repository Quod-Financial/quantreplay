#include "ih/market_data/subscriptions/subscription.hpp"

#include "ih/market_data/tools/notification_creators.hpp"

namespace simulator::trading_system::matching_engine::mdata {

Subscription::Subscription(EventListener& listener,
                           MdRequestId request_id,
                           protocol::Session session,
                           InstrumentDescriptor instrument,
                           StreamingSettings settings)
    : EventReporter(listener),
      instrument_(std::move(instrument)),
      session_(std::move(session)),
      request_id_(std::move(request_id)),
      settings_(std::move(settings)) {}

auto Subscription::send_initial(const MarketDataProvider& provider) -> void {
  protocol::MarketDataSnapshot snapshot{session_};
  snapshot.request_id = request_id_;
  snapshot.instrument = instrument_;
  snapshot.market_data_entries = provider.compose_initial(settings_);
  emit(make_snapshot_published_notification(std::move(snapshot)));
}

auto Subscription::send_snapshot(const MarketDataProvider& provider) -> void {
  send_initial(provider);
}

auto Subscription::send_update(const MarketDataProvider& provider) -> void {
  if (settings_.is_full_update_requested()) {
    send_full_update(provider);
  } else {
    send_incremental_update(provider);
  }
}

auto Subscription::send_full_update(const MarketDataProvider& provider)
    -> void {
  protocol::MarketDataSnapshot update{session_};
  update.request_id = request_id_;
  update.instrument = instrument_;
  update.market_data_entries = provider.compose_update(settings_);
  emit(make_snapshot_published_notification(std::move(update)));
}

auto Subscription::send_incremental_update(const MarketDataProvider& provider)
    -> void {
  protocol::MarketDataUpdate update{session_};
  update.request_id = request_id_;
  update.market_data_entries = provider.compose_update(settings_);
  if (!update.market_data_entries.empty()) {
    emit(make_update_published_notification(std::move(update)));
  }
}

}  // namespace simulator::trading_system::matching_engine::mdata