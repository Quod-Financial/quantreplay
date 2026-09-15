#ifndef SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_FROM_FIX_MAPPER_STUB_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_FROM_FIX_MAPPER_STUB_HPP_

#include <quickfix/Fields.h>
#include <quickfix/Message.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"

namespace simulator::fix::generator_initiator::test {

// Maps the request id only, which is what the reply processor branches on.
struct FromFixMapperStub {
  auto map(const FIX::Message& message,
           protocol::MarketDataSnapshot& snapshot) const -> void {
    map_request_id(message, snapshot.request_id);
  }

  auto map(const FIX::Message& message,
           protocol::MarketDataUpdate& update) const -> void {
    map_request_id(message, update.request_id);
  }

  auto map(const FIX::Message& message,
           protocol::MarketDataReject& reject) const -> void {
    map_request_id(message, reject.request_id);
  }

 private:
  static auto map_request_id(const FIX::Message& message,
                             std::optional<MdRequestId>& destination) -> void {
    FIX::MDReqID fix_field;
    if (message.getFieldIfSet(fix_field)) {
      destination = MdRequestId{fix_field.getValue()};
    }
  }
};

}  // namespace simulator::fix::generator_initiator::test

#endif  // SIMULATOR_GENERATOR_INITIATOR_TESTS_MOCKS_FROM_FIX_MAPPER_STUB_HPP_
