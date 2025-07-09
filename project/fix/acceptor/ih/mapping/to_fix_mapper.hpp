#ifndef SIMULATOR_ACCEPTOR_IH_MAPPING_TO_FIX_MAPPER_HPP_
#define SIMULATOR_ACCEPTOR_IH_MAPPING_TO_FIX_MAPPER_HPP_

#include <quickfix/Message.h>

#include <vector>

#include "common/mapping/setting/mapping_settings.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/party.hpp"
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

namespace simulator::fix::acceptor {

struct ToFixMapper {
  static auto map(const protocol::BusinessMessageReject& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::ExecutionReport& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::OrderPlacementConfirmation& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::OrderPlacementReject& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::OrderModificationConfirmation& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::OrderModificationReject& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::OrderCancellationConfirmation& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::OrderCancellationReject& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::MarketDataReject& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::MarketDataSnapshot& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::MarketDataUpdate& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map(const protocol::SecurityStatus& reply,
                  FIX::Message& fix_message,
                  const MappingSettings::Setting& setting) -> void;

  static auto map_instrument(const InstrumentDescriptor& instrument,
                             FIX::FieldMap& destination) -> void;

  static auto map_parties(const std::vector<Party>& parties,
                          FIX::FieldMap& destination) -> void;

 private:
  template <typename InternalMessage>
  static auto map_fix_execution_report_common_fields(
      const InternalMessage& reply,
      FIX::Message& fix_message,
      const MappingSettings::Setting& setting) -> void;

  template <typename InternalMessage>
  static auto map_fix_order_cancel_reject_common_fields(
      const InternalMessage& reply, FIX::Message& fix_message) -> void;
};

}  // namespace simulator::fix::acceptor

#endif  // SIMULATOR_ACCEPTOR_IH_MAPPING_TO_FIX_MAPPER_HPP_
