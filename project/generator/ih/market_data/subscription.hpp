#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_HPP_

#include <utility>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata {

class Subscription {
 public:
  Subscription(MdRequestId request_id,
               protocol::Session session,
               InstrumentDescriptor instrument,
               MarketDepth depth) noexcept
      : request_id_{std::move(request_id)},
        session_{std::move(session)},
        instrument_{std::move(instrument)},
        depth_{depth} {}

  [[nodiscard]]
  auto request_id() const noexcept -> const MdRequestId& {
    return request_id_;
  }

  [[nodiscard]]
  auto session() const noexcept -> const protocol::Session& {
    return session_;
  }

  [[nodiscard]]
  auto instrument() const noexcept -> const InstrumentDescriptor& {
    return instrument_;
  }

  [[nodiscard]]
  auto depth() const noexcept -> MarketDepth {
    return depth_;
  }

  [[nodiscard]]
  auto subscribed() const noexcept -> bool {
    return subscribed_;
  }

  auto set_depth(MarketDepth depth) noexcept -> void { depth_ = depth; }

  auto mark_subscribed() noexcept -> void { subscribed_ = true; }

  auto mark_unsubscribed() noexcept -> void { subscribed_ = false; }

 private:
  MdRequestId request_id_;
  protocol::Session session_;
  InstrumentDescriptor instrument_;
  MarketDepth depth_;
  bool subscribed_ = false;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_HPP_
