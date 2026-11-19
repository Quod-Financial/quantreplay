#include "ih/controllers/generator_controller.hpp"

#include <fmt/format.h>

#include <exception>
#include <optional>
#include <string>
#include <utility>

#include "ih/marshalling/json/generator.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"
#include "middleware/routing/errors.hpp"
#include "middleware/routing/generator_admin_channel.hpp"
#include "protocol/admin/generator.hpp"

namespace simulator::http {
namespace {

[[nodiscard]]
auto normalize_seed(std::optional<std::string> seed)
    -> std::optional<std::string> {
  if (!seed.has_value() || seed->empty()) {
    return std::nullopt;
  }
  return seed;
}

template <typename Request, typename Reply>
auto send_generator_request(const Request& request, Reply& reply) -> void {
  try {
    middleware::send_admin_request(request, reply);
  } catch (const middleware::ChannelUnboundError&) {
    log::err("failed to send request {}", request);
  }
}

}  // namespace

auto GeneratorControllerImpl::start_generation(const std::string& body) const
    -> Result {
  protocol::StartGenerationRequest request;

  if (!body.empty()) {
    try {
      json::GenerationStartUnmarshaller::unmarshall(body, request);
    } catch (const std::exception& e) {
      log::err("failed to unmarshall GenerationStart request: {}", e.what());
      return std::make_pair(
          Pistache::Http::Code::Bad_Request,
          format_result_response(
              fmt::format("Failed to unmarshall request body: {}.", e.what())));
    }
  }

  request.seed = normalize_seed(std::move(request.seed));

  protocol::StartGenerationReply reply;
  send_generator_request(request, reply);

  if (reply.result == protocol::StartGenerationReply::Result::Started) {
    return std::make_pair(
        Pistache::Http::Code::Ok,
        format_result_response("Automatic generator started successfully"));
  }
  return std::make_pair(
      Pistache::Http::Code::Internal_Server_Error,
      format_result_response("Failed to start the automatic generator"));
}

auto GeneratorControllerImpl::stop_generation() const -> Result {
  protocol::StopGenerationReply reply;
  send_generator_request(protocol::StopGenerationRequest{}, reply);

  if (reply.result == protocol::StopGenerationReply::Result::Stopped) {
    return std::make_pair(
        Pistache::Http::Code::Ok,
        format_result_response("Automatic generator stopped successfully"));
  }
  return std::make_pair(
      Pistache::Http::Code::Internal_Server_Error,
      format_result_response("Failed to stop the automatic generator"));
}

}  // namespace simulator::http
