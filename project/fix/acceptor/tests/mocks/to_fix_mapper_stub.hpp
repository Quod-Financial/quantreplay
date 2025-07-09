#ifndef SIMULATOR_ACCEPTOR_TESTS_MOCKS_TO_FIX_MAPPER_STUB_HPP_
#define SIMULATOR_ACCEPTOR_TESTS_MOCKS_TO_FIX_MAPPER_STUB_HPP_

#include <quickfix/Message.h>

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

namespace simulator::fix::acceptor::test {

struct ToFixMapperStub {
  auto map([[maybe_unused]] const protocol::BusinessMessageReject& reply,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}

  auto map([[maybe_unused]] const protocol::ExecutionReport& report,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}

  auto map([[maybe_unused]] const protocol::OrderPlacementConfirmation& reply,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}

  auto map([[maybe_unused]] const protocol::OrderPlacementReject& reply,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}

  auto map(
      [[maybe_unused]] const protocol::OrderModificationConfirmation& reply,
      [[maybe_unused]] FIX::Message& message,
      [[maybe_unused]] const MappingSettings::Setting& setting) const -> void {}

  auto map([[maybe_unused]] const protocol::OrderModificationReject& reply,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}

  auto map(
      [[maybe_unused]] const protocol::OrderCancellationConfirmation& reply,
      [[maybe_unused]] FIX::Message& message,
      [[maybe_unused]] const MappingSettings::Setting& setting) const -> void {}

  auto map([[maybe_unused]] const protocol::OrderCancellationReject& reply,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}

  auto map([[maybe_unused]] const protocol::MarketDataReject& reply,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}

  auto map([[maybe_unused]] const protocol::MarketDataSnapshot& reply,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}

  auto map([[maybe_unused]] const protocol::MarketDataUpdate& reply,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}

  auto map([[maybe_unused]] const protocol::SecurityStatus& reply,
           [[maybe_unused]] FIX::Message& message,
           [[maybe_unused]] const MappingSettings::Setting& setting) const
      -> void {}
};

}  // namespace simulator::fix::acceptor::test

#endif  // SIMULATOR_ACCEPTOR_TESTS_MOCKS_TO_FIX_MAPPER_STUB_HPP_
