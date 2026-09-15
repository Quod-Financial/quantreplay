#ifndef SIMULATOR_GENERATOR_INCLUDE_GENERATOR_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_INCLUDE_GENERATOR_GENERATOR_HPP_

#include <memory>

#include "data_layer/api/database/context.hpp"
#include "protocol/admin/generator.hpp"
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
#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"

namespace simulator::generator {

struct Generator {
  struct Implementation;

  explicit Generator(std::unique_ptr<Implementation> impl) noexcept;

  Generator(const Generator&) = delete;
  Generator(Generator&&) noexcept = default;
  ~Generator() noexcept;

  auto operator=(const Generator&) -> Generator& = delete;
  auto operator=(Generator&&) noexcept -> Generator& = default;

  [[nodiscard]] auto implementation() noexcept -> Implementation&;

 private:
  std::unique_ptr<Implementation> impl_;
};

[[nodiscard]] auto create_generator(data_layer::database::Context db)
    -> Generator;

auto launch_generator(Generator& generator) -> void;

auto terminate_generator(Generator& generator) noexcept -> void;

auto accept_reply(const protocol::ExecutionReport& reply, Generator& generator)
    -> void;

auto accept_reply(const protocol::OrderPlacementConfirmation& reply,
                  Generator& generator) -> void;

auto accept_reply(const protocol::OrderPlacementReject& reply,
                  Generator& generator) -> void;

auto accept_reply(const protocol::OrderModificationConfirmation& reply,
                  Generator& generator) -> void;

auto accept_reply(const protocol::OrderModificationReject& reply,
                  Generator& generator) -> void;

auto accept_reply(const protocol::OrderCancellationConfirmation& reply,
                  Generator& generator) -> void;

auto accept_reply(const protocol::OrderCancellationReject& reply,
                  Generator& generator) -> void;

auto accept_reply(const protocol::MarketDataSnapshot& reply,
                  Generator& generator) -> void;

auto accept_reply(const protocol::MarketDataUpdate& reply, Generator& generator)
    -> void;

auto accept_reply(const protocol::MarketDataReject& reply, Generator& generator)
    -> void;

auto react_on(const protocol::SessionConnectedEvent& event,
              Generator& generator) -> void;

auto react_on(const protocol::SessionTerminatedEvent& event,
              Generator& generator) -> void;

auto process_admin_request(Generator& generator,
                           const protocol::GenerationStatusRequest& request,
                           protocol::GenerationStatusReply& reply) -> void;

auto process_admin_request(Generator& generator,
                           const protocol::StartGenerationRequest& request,
                           protocol::StartGenerationReply& reply) -> void;

auto process_admin_request(Generator& generator,
                           const protocol::StopGenerationRequest& request,
                           protocol::StopGenerationReply& reply) -> void;

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_INCLUDE_GENERATOR_GENERATOR_HPP_