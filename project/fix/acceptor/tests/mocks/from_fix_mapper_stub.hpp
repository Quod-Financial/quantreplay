#ifndef SIMULATOR_ACCEPTOR_TESTS_MOCKS_FROM_FIX_MAPPER_STUB_HPP_
#define SIMULATOR_ACCEPTOR_TESTS_MOCKS_FROM_FIX_MAPPER_STUB_HPP_

#include <quickfix/Message.h>

#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"

namespace simulator::fix::acceptor::test {

struct FromFixMapperStub {
  auto map([[maybe_unused]] const FIX::Message& message,
           [[maybe_unused]] protocol::OrderPlacementRequest& request) const
      -> void {}

  auto map([[maybe_unused]] const FIX::Message& message,
           [[maybe_unused]] protocol::OrderModificationRequest& request) const
      -> void {}

  auto map([[maybe_unused]] const FIX::Message& message,
           [[maybe_unused]] protocol::OrderCancellationRequest& request) const
      -> void {}

  auto map([[maybe_unused]] const FIX::Message& message,
           [[maybe_unused]] protocol::MarketDataRequest& request) const
      -> void {}

  auto map([[maybe_unused]] const FIX::Message& message,
           [[maybe_unused]] protocol::SecurityStatusRequest& request) const
      -> void {}
};

}  // namespace simulator::fix::acceptor::test

#endif  // SIMULATOR_ACCEPTOR_TESTS_MOCKS_FROM_FIX_MAPPER_STUB_HPP_