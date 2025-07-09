#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_SUBSCRIPTIONS_SUBSCRIPTION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_SUBSCRIPTIONS_SUBSCRIPTION_HPP_

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/market_data/cache/market_data_provider.hpp"
#include "ih/market_data/streaming_settings.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class Subscription final : private EventReporter {
 public:
  Subscription(EventListener& listener,
               MdRequestId request_id,
               protocol::Session session,
               InstrumentDescriptor instrument,
               StreamingSettings settings);

  auto request_id() const -> const MdRequestId& { return request_id_; }

  auto session() const -> const protocol::Session& { return session_; }

  auto send_initial(const MarketDataProvider& provider) -> void;

  auto send_snapshot(const MarketDataProvider& provider) -> void;

  auto send_update(const MarketDataProvider& provider) -> void;

 private:
  auto send_full_update(const MarketDataProvider& provider) -> void;

  auto send_incremental_update(const MarketDataProvider& provider) -> void;

  InstrumentDescriptor instrument_;
  protocol::Session session_;
  MdRequestId request_id_;
  StreamingSettings settings_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_SUBSCRIPTIONS_SUBSCRIPTION_HPP_
