#ifndef SIMULATOR_APP_IH_COMPONENTS_GENERATOR_HPP_
#define SIMULATOR_APP_IH_COMPONENTS_GENERATOR_HPP_

#include "data_layer/api/database/context.hpp"
#include "generator/generator.hpp"
#include "log/logging.hpp"
#include "middleware/channels/generator_admin_channel.hpp"
#include "middleware/channels/trading_reply_channel.hpp"

namespace simulator {

class Generator final : public middleware::TradingReplyReceiver,
                        public middleware::GeneratorAdminRequestReceiver {
 public:
  explicit Generator(data_layer::database::Context database)
      : generator_(generator::create_generator(std::move(database))) {}

  auto launch() -> void { generator::launch_generator(generator_); }

  auto terminate() -> void { generator::terminate_generator(generator_); }

  auto process(protocol::BusinessMessageReject reject) -> void override {
    // The generator is not expected to receive BusinessMessageReject messages.
    log::warn(
        "unexpected BusinessMessageReject message received by the generator, "
        "ignoring {}",
        reject);
  }

  auto process(protocol::ExecutionReport report) -> void override {
    generator::accept_reply(report, generator_);
  }

  auto process(protocol::OrderPlacementReject reject) -> void override {
    generator::accept_reply(reject, generator_);
  }

  auto process(protocol::OrderPlacementConfirmation confirmation)
      -> void override {
    generator::accept_reply(confirmation, generator_);
  }

  auto process(protocol::OrderModificationReject reject) -> void override {
    generator::accept_reply(reject, generator_);
  }

  auto process(protocol::OrderModificationConfirmation confirmation)
      -> void override {
    generator::accept_reply(confirmation, generator_);
  }

  auto process(protocol::OrderCancellationReject reject) -> void override {
    generator::accept_reply(reject, generator_);
  }

  auto process(protocol::OrderCancellationConfirmation confirmation)
      -> void override {
    generator::accept_reply(confirmation, generator_);
  }

  auto process(protocol::MarketDataReject reject) -> void override {
    // The generator does not send MarketDataRequest messages.
    // Thus, it is not expected to receive MarketDataReject messages.
    log::warn(
        "unexpected MarketDataReject message received by the generator, "
        "ignoring {}",
        reject);
  }

  auto process(protocol::MarketDataSnapshot snapshot) -> void override {
    // The generator does not send MarketDataRequest messages/
    // Thus, it is not expected to receive MarketDataSnapshot messages.
    log::warn(
        "unexpected MarketDataSnapshot message received by generator, "
        "ignoring {}",
        snapshot);
  }

  auto process(protocol::MarketDataUpdate update) -> void override {
    // The generator does not send MarketDataRequest messages/
    // Thus, it is not expected to receive MarketDataUpdate messages
    log::warn(
        "unexpected MarketDataUpdate message received by generator, "
        "ignoring {}",
        update);
  }

  auto process(protocol::SecurityStatus status) -> void override {
    // The generator does not send SecurityStatusRequest messages.
    // Thus, it is not expected to receive SecurityStatus messages.
    log::warn(
        "unexpected SecurityStatus message received by generator, "
        "ignoring {}",
        status);
  }

  auto process(const protocol::GenerationStatusRequest& request,
               protocol::GenerationStatusReply& reply) -> void override {
    generator::process_admin_request(generator_, request, reply);
  }

  auto process(const protocol::StartGenerationRequest& request,
               protocol::StartGenerationReply& reply) -> void override {
    generator::process_admin_request(generator_, request, reply);
  }

  auto process(const protocol::StopGenerationRequest& request,
               protocol::StopGenerationReply& reply) -> void override {
    generator::process_admin_request(generator_, request, reply);
  }

 private:
  generator::Generator generator_;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_COMPONENTS_GENERATOR_HPP_