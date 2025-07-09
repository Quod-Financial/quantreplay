#ifndef SIMULATOR_MIDDLEWARE_CHANNELS_GENERATOR_ADMIN_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_CHANNELS_GENERATOR_ADMIN_CHANNEL_HPP_

#include <memory>

#include "middleware/channels/detail/receiver.hpp"
#include "protocol/admin/generator.hpp"

namespace simulator::middleware {

struct GeneratorAdminRequestReceiver : public detail::Receiver {
  virtual auto process(const protocol::GenerationStatusRequest& request,
                       protocol::GenerationStatusReply& reply) -> void = 0;

  virtual auto process(const protocol::StartGenerationRequest& request,
                       protocol::StartGenerationReply& reply) -> void = 0;

  virtual auto process(const protocol::StopGenerationRequest& request,
                       protocol::StopGenerationReply& reply) -> void = 0;
};

// Allows the receiver receiving messages sent via the channel,
// unbinds previous receiver from the channel if any.
auto bind_generator_admin_channel(
    std::shared_ptr<GeneratorAdminRequestReceiver> receiver) -> void;

// Unbinds previous receiver from the channel if any.
auto release_generator_admin_channel() noexcept -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_CHANNELS_GENERATOR_ADMIN_CHANNEL_HPP_