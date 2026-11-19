#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_QUOTE_PUBLISHING_ENGINE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_QUOTE_PUBLISHING_ENGINE_HPP_

#include "protocol/admin/quote.hpp"

namespace simulator::trading_system {

class QuotePublishingEngine {
 public:
  QuotePublishingEngine() = default;
  QuotePublishingEngine(const QuotePublishingEngine&) = delete;
  QuotePublishingEngine(QuotePublishingEngine&&) = delete;
  virtual ~QuotePublishingEngine() = default;

  auto operator=(const QuotePublishingEngine&)
      -> QuotePublishingEngine& = delete;
  auto operator=(QuotePublishingEngine&&) -> QuotePublishingEngine& = delete;

  virtual auto execute(const protocol::QuotePublicationRequest& request,
                       protocol::QuotePublicationReply& reply) -> void = 0;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_QUOTE_PUBLISHING_ENGINE_HPP_
