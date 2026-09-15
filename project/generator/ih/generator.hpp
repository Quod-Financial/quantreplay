#ifndef SIMULATOR_GENERATOR_IH_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_IH_GENERATOR_HPP_

#include <optional>
#include <string>

#include "ih/adaptation/protocol_conversion.hpp"
#include "ih/generator_impl.hpp"
#include "log/logging.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::generator {

struct Generator::Implementation {
  Implementation(data_layer::Venue simulated_venue,
                 data_layer::database::Context db)
      : generator_(std::make_unique<Generator>(std::move(simulated_venue),
                                               std::move(db))) {}

  auto is_running() const -> bool { return generator_->status(); }

  auto launch() -> void { generator_->start(); }

  auto resume(const std::optional<std::string>& user_seed) -> void {
    generator_->resume(user_seed);
  }

  auto suspend() -> void { generator_->suspend(); }

  auto terminate() noexcept -> void { generator_->terminate(); }

  auto enrich(const protocol::OrderPlacementConfirmation& reply) -> void {
    handle_reply(reply);
  }

  auto enrich(const protocol::OrderPlacementReject& reply) -> void {
    handle_reply(reply);
  }

  auto enrich(const protocol::OrderModificationConfirmation& reply) -> void {
    handle_reply(reply);
  }

  auto enrich(const protocol::OrderCancellationConfirmation& reply) -> void {
    handle_reply(reply);
  }

  auto enrich(const protocol::ExecutionReport& reply) -> void {
    handle_reply(reply);
  }

  auto process_market_data(const protocol::MarketDataSnapshot& snapshot)
      -> void {
    generator_->process_market_data(snapshot);
  }

  auto process_market_data(const protocol::MarketDataReject& reject) -> void {
    generator_->process_market_data(reject);
  }

  auto process_session_connection(const protocol::SessionConnectedEvent& event)
      -> void {
    generator_->process_session_connection(event);
  }

  auto process_session_disconnection(
      const protocol::SessionTerminatedEvent& event) -> void {
    generator_->process_session_disconnection(event);
  }

 private:
  auto handle_reply(const auto& reply) -> void {
    const std::optional<RequesterInstrumentId> instrument_id =
        reply.instrument.requester_instrument_id;
    if (!instrument_id) {
      log::warn(
          "received reply message without the requester instrument identifier, "
          "can not handle {}",
          reply);
      return;
    }

    generator_->process_reply(static_cast<std::uint64_t>(*instrument_id),
                              generator::convert_to_generated_message(reply));
  }

  using Generator = generator::GeneratorImpl;
  std::unique_ptr<Generator> generator_;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_GENERATOR_HPP_