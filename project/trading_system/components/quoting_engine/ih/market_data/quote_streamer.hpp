#ifndef SIMULATOR_QUOTING_ENGINE_IH_MARKET_DATA_QUOTE_STREAMER_HPP_
#define SIMULATOR_QUOTING_ENGINE_IH_MARKET_DATA_QUOTE_STREAMER_HPP_

#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "ih/quotes/quote.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::quoting_engine {

class QuoteStreamer {
 public:
  auto process(const protocol::MarketDataRequest& request,
               const Quote* latest_quote) -> void;

  auto publish(const Quote& quote) const -> void;

  auto unsubscribe(const protocol::Session& session) -> void;

 private:
  struct Subscription {
    protocol::Session session;
    InstrumentDescriptor instrument;
    std::optional<MdRequestId> request_id;
    std::vector<MdEntryType> entry_types;
  };

  auto subscribe(const protocol::MarketDataRequest& request,
                 const Quote* latest_quote) -> void;

  auto unsubscribe(const protocol::MarketDataRequest& request) -> void;

  [[nodiscard]]
  static auto make_subscription(const protocol::MarketDataRequest& request)
      -> Subscription;

  [[nodiscard]]
  static auto is_identified_by(const Subscription& subscription,
                               const protocol::MarketDataRequest& request)
      -> bool;

  static auto send_snapshot(const Subscription& subscription,
                            const Quote* latest_quote) -> void;

  std::vector<Subscription> subscriptions_;
};

}  // namespace simulator::trading_system::quoting_engine

#endif  // SIMULATOR_QUOTING_ENGINE_IH_MARKET_DATA_QUOTE_STREAMER_HPP_
