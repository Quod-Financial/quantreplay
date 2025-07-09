#ifndef SIMULATOR_MIDDLEWARE_ROUTING_GENERATOR_ADMIN_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_ROUTING_GENERATOR_ADMIN_CHANNEL_HPP_

#include "middleware/routing/errors.hpp"
#include "protocol/admin/generator.hpp"

namespace simulator::middleware {

auto send_admin_request(const protocol::GenerationStatusRequest& request,
                        protocol::GenerationStatusReply& reply) -> void;

auto send_admin_request(const protocol::StartGenerationRequest& request,
                        protocol::StartGenerationReply& reply) -> void;

auto send_admin_request(const protocol::StopGenerationRequest& request,
                        protocol::StopGenerationReply& reply) -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_ROUTING_GENERATOR_ADMIN_CHANNEL_HPP_