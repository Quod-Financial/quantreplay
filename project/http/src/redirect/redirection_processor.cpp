#include "ih/redirect/redirection_processor.hpp"

#include <fmt/format.h>

#include <utility>

#include "ih/data_bridge/venue_accessor.hpp"
#include "ih/redirect/destination_resolver.hpp"
#include "ih/redirect/redirector.hpp"
#include "ih/redirect/request_redirector.hpp"
#include "ih/utils/response_formatters.hpp"

namespace simulator::http::redirect {

RedirectionProcessor::RedirectionProcessor(
    const data_bridge::VenueAccessor& venue_accessor)
    : RedirectionProcessor(DestinationResolver::create(venue_accessor),
                           RequestRedirector::create()) {}

RedirectionProcessor::RedirectionProcessor(
    std::shared_ptr<Resolver> resolver,
    std::shared_ptr<Redirector> redirector) noexcept
    : resolver_{std::move(resolver)}, redirector_{std::move(redirector)} {}

auto RedirectionProcessor::redirect_to_venue(const std::string& venue_id,
                                             Pistache::Http::Method method,
                                             const std::string& url) const
    -> Result {
  auto [destination, status] = resolver_->resolve_by_venue_id(venue_id);
  if (!destination.has_value() || status != Resolver::Status::Success) {
    return process_resolve_error(status, venue_id);
  }

  const Request request{*destination, method, url};
  auto [response, redirect_status] = redirector_->redirect(request);
  if (!response.has_value() || redirect_status != Redirector::Status::Success) {
    return process_redirect_error(redirect_status, venue_id);
  }

  return *response;
}

auto RedirectionProcessor::create(
    const data_bridge::VenueAccessor& venue_accessor)
    -> std::shared_ptr<RedirectionProcessor> {
  return std::make_shared<RedirectionProcessor>(venue_accessor);
}

auto RedirectionProcessor::process_resolve_error(Resolver::Status status,
                                                 std::string_view venue_id)
    -> Result {
  auto response_code = Pistache::Http::Code::Ok;
  std::string message;

  switch (status) {
    case Resolver::Status::Success:
    case Resolver::Status::UnknownError: {
      response_code = Pistache::Http::Code::Internal_Server_Error;
      message = format_result_response(
          "Request destination resolving failed with unknown error");
      break;
    }
    case Resolver::Status::NonexistentInstance: {
      response_code = Pistache::Http::Code::Bad_Gateway;
      message = format_result_response(fmt::format(
          "Could not resolve destination instance with {} identifier",
          venue_id));
      break;
    }
    case Resolver::Status::ResolvingFailed: {
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

auto RedirectionProcessor::process_redirect_error(Redirector::Status status,
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