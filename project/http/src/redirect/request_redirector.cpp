#include "ih/redirect/request_redirector.hpp"

#include <httplib.h>
#include <pistache/http.h>

#include <map>
#include <stdexcept>

#include "ih/formatters/redirect.hpp"
#include "log/logging.hpp"

namespace simulator::http::redirect {

using MethodHandler =
    std::function<httplib::Result(httplib::Client&, const Request&)>;

static const std::map<Pistache::Http::Method, MethodHandler> Handlers{
    {Pistache::Http::Method::Get,
     [](auto& client, const auto& request) {
       return client.Get(request.url());
     }},
    {Pistache::Http::Method::Post,
     [](auto& client, const auto& request) {
       return client.Post(request.url());
     }},
    {Pistache::Http::Method::Put,
     [](auto& client, const auto& request) {
       return client.Put(request.url());
     }},
};

static auto find_handler(Pistache::Http::Method method)
    -> const MethodHandler& {
  auto iter = Handlers.find(method);
  if (iter == std::end(Handlers)) {
    throw std::logic_error{
        fmt::format("request can not be redirected, {} method is not supported "
                    "for redirection",
                    Pistache::Http::methodString(method))};
  }
  return iter->second;
}

static auto process_response(httplib::Result response, const Request& request)
    -> Redirector::RedirectionResult {
  auto& received_response = response.value();
  const int response_status = received_response.status;
  const auto response_code = static_cast<Pistache::Http::Code>(response_status);

  Result result{response_code};
  result.set_body_content(std::move(received_response.body));

  log::debug("received a {} on a {}", result, request);
  return std::make_pair(std::move(result), Redirector::Status::Success);
}

static auto process_error(httplib::Error error, const Request& request)
    -> Redirector::RedirectionResult {
  using Status = Redirector::Status;

  if (error == httplib::Error::ConnectionTimeout ||
      error == httplib::Error::Connection) {
    log::err("{} failed - connection failed", request);
    return std::make_pair(std::nullopt, Status::ConnectionFailed);
  }

  log::err("{} failed - httplib reported about error that can not be handled",
           request);
  return std::make_pair(std::nullopt, Status::UnknownError);
}

auto RequestRedirector::redirect(const Request& request) const noexcept
    -> Redirector::RedirectionResult {
  const auto& destination = request.destination();
  try {
    log::debug("forwarding {}", request);
    httplib::Client client{destination.host(), destination.port()};
    const auto& method_handler = find_handler(request.method());
    auto request_result = method_handler(client, request);

    if (request_result) {
      return process_response(std::move(request_result), request);
    }

    return process_error(request_result.error(), request);
  } catch (const std::exception& ex) {
    log::err(
        "failed to forward {} - an error occurred: {}", request, ex.what());
    return std::make_pair(std::nullopt, Redirector::Status::UnknownError);
  } catch (...) {
    log::err("failed to redirect {} - unknown error occurred", request);
    return std::make_pair(std::nullopt, Redirector::Status::UnknownError);
  }
}

auto RequestRedirector::create() -> std::shared_ptr<RequestRedirector> {
  return std::make_shared<RequestRedirector>();
}

}  // namespace simulator::http::redirect