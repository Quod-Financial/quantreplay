#include "ih/redirect/redirection_processor.hpp"

#include <fmt/format.h>

#include <optional>
#include <utility>

#include "cfg/api/cfg.hpp"
#include "ih/data_bridge/venue_accessor.hpp"
#include "ih/redirect/destination_resolver.hpp"
#include "ih/redirect/redirector.hpp"
#include "ih/redirect/request_redirector.hpp"
#include "ih/utils/response_formatters.hpp"

namespace simulator::http::redirect {

RedirectionProcessorImpl::RedirectionProcessorImpl(
    std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
    std::uint16_t current_rest_port)
    : RedirectionProcessorImpl(
          DestinationResolver::create(
              venue_accessor, cfg::venue().name, current_rest_port),
          RequestRedirector::create()) {}

RedirectionProcessorImpl::RedirectionProcessorImpl(
    std::shared_ptr<Resolver> resolver,
    std::shared_ptr<Redirector> redirector) noexcept
    : resolver_{std::move(resolver)}, redirector_{std::move(redirector)} {}

auto RedirectionProcessorImpl::redirect_to_venue(
    const std::string& venue_id,
    Pistache::Http::Method method,
    const std::string& url,
    std::optional<std::string> body) const -> Result {
  auto result = resolver_->resolve_by_venue_id(venue_id);
  if (!result) {
    return process_resolve_error(result.error(), venue_id);
  }

  const Request request{*result, method, url, std::move(body)};
  auto [response, redirect_status] = redirector_->redirect(request);
  if (!response.has_value() || redirect_status != Redirector::Status::Success) {
    return process_redirect_error(redirect_status, venue_id);
  }

  return *response;
}

auto RedirectionProcessorImpl::create(
    std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
    std::uint16_t current_rest_port) -> std::shared_ptr<RedirectionProcessor> {
  return std::make_shared<RedirectionProcessorImpl>(std::move(venue_accessor),
                                                    current_rest_port);
}

auto RedirectionProcessorImpl::process_resolve_error(Resolver::Error status,
                                                     std::string_view venue_id)
    -> Result {
  auto response_code = Pistache::Http::Code::Ok;
  std::string message;

  switch (status) {
    case Resolver::Error::UnknownError: {
      response_code = Pistache::Http::Code::Internal_Server_Error;
      message = format_result_response(
          "Request destination resolving failed with unknown error");
      break;
    }
    case Resolver::Error::NonexistentInstance: {
      response_code = Pistache::Http::Code::Bad_Gateway;
      message = format_result_response(fmt::format(
          "Could not resolve destination instance with {} identifier",
          venue_id));
      break;
    }
    case Resolver::Error::ResolvingFailed: {
      response_code = Pistache::Http::Code::Bad_Gateway;
      message = format_result_response(fmt::format(
          "Could not access an instance with {} identifier", venue_id));
      break;
    }
    case Resolver::Error::SelfRedirect: {
      response_code = Pistache::Http::Code::Service_Unavailable;
      message = format_result_response(
          fmt::format("Request redirection to venue {} was blocked to prevent "
                      "self-redirect",
                      venue_id));
      break;
    }
  }

  Result response{response_code};
  response.set_body_content(std::move(message));
  return response;
}

auto RedirectionProcessorImpl::process_redirect_error(Redirector::Status status,
                                                      std::string_view venue_id)
    -> Result {
  auto response_code = Pistache::Http::Code::Ok;
  std::string message;

  switch (status) {
    case Redirector::Status::Success:
    case Redirector::Status::UnknownError: {
      response_code = Pistache::Http::Code::Internal_Server_Error;
      message = format_result_response(
          "Request forwarding failed with unknown error");
      break;
    }
    case Redirector::Status::ConnectionFailed: {
      response_code = Pistache::Http::Code::Bad_Gateway;
      message = format_result_response(fmt::format(
          "Could not access an instance with {} identifier", venue_id));
      break;
    }
  }

  Result response{response_code};
  response.set_body_content(std::move(message));
  return response;
}

}  // namespace simulator::http::redirect