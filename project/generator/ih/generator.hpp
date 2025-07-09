#ifndef SIMULATOR_GENERATOR_IH_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_IH_GENERATOR_HPP_

#include "ih/adaptation/protocol_conversion.hpp"
#include "ih/generator_impl.hpp"
#include "log/logging.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"

namespace simulator::generator {

struct Generator::Implementation {
  Implementation(data_layer::Venue simulated_venue,
                 data_layer::database::Context db)
      : generator_(std::make_unique<Generator>(std::move(simulated_venue),
                                               std::move(db))) {}

  auto is_running() const -> bool { return generator_->status(); }

  auto launch() -> void { generator_->start(); }

  auto resume() -> void { generator_->resume(); }

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