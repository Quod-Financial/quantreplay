#include "ih/controllers/quote_controller.hpp"

#include <fmt/format.h>
#include <pistache/http_defs.h>

#include <exception>
#include <string>
#include <utility>

#include "core/domain/attributes.hpp"
#include "ih/endpoint.hpp"
#include "ih/marshalling/json/detail/keys.hpp"
#include "ih/marshalling/json/quote.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"
#include "middleware/routing/errors.hpp"
#include "middleware/routing/trading_admin_channel.hpp"
#include "protocol/admin/quote.hpp"

namespace simulator::http {
namespace {

using Code = Pistache::Http::Code;

[[nodiscard]]
auto make_published_response(const QuoteId& quote_id)
    -> QuoteController::Result {
  return {Code::Created,
          format_response(json::quote_key::QuoteId, quote_id.value())};
}

[[nodiscard]]
auto make_refused_response(Code code, const std::string& reason)
    -> QuoteController::Result {
  return {code,
          format_result_response(
              fmt::format("Failed to publish the Quote: {}", reason))};
}

[[nodiscard]]
auto make_response(const protocol::QuotePublicationReply& reply)
    -> QuoteController::Result {
  using ReplyResult = protocol::QuotePublicationReply::Result;

  if (reply.result.has_value()) {
    switch (*reply.result) {
      case ReplyResult::Published:
        if (reply.quote_id.has_value()) {
          return make_published_response(*reply.quote_id);
        }
        break;
      case ReplyResult::ListingNotFound:
        return make_refused_response(Code::Not_Found, reply.error_message);
      case ReplyResult::AmbiguousListing:
        return make_refused_response(Code::Conflict, reply.error_message);
      case ReplyResult::InvalidRequest:
        return make_refused_response(Code::Bad_Request, reply.error_message);
    }
  }
  return {Code::Internal_Server_Error,
          format_result_response("Failed to process the request.")};
}

}  // namespace

QuoteControllerImpl::QuoteControllerImpl(
    std::shared_ptr<redirect::RedirectionProcessor> redirector,
    std::string venue_id)
    : redirector_{std::move(redirector)}, venue_id_{std::move(venue_id)} {}

auto QuoteControllerImpl::publish_quote(const std::string& body) const
    -> Result {
  json::QuotePublication publication;
  try {
    json::QuoteUnmarshaller::unmarshall(body, publication);
  } catch (const std::exception& exception) {
    log::warn("failed to unmarshall a Quote: {}", exception.what());
    return {Code::Bad_Request,
            format_result_response(
                fmt::format("Bad request: {}", exception.what()))};
  }

  if (publication.venue_id != venue_id_) {
    return redirect(publication.venue_id, body);
  }
  return publish(publication);
}

auto QuoteControllerImpl::redirect(const std::string& venue_id,
                                   const std::string& body) const -> Result {
  log::info("forwarding the quote publication to the `{}' venue", venue_id);
  const auto result =
      redirector_->redirect_to_venue(venue_id,
                                     Pistache::Http::Method::Post,
                                     std::string{endpoint::Quotes},
                                     body);
  return {result.http_code(), result.body_content()};
}

auto QuoteControllerImpl::publish(const json::QuotePublication& publication)
    -> Result {
  protocol::QuotePublicationReply reply;
  try {
    middleware::send_admin_request(publication.request, reply);
  } catch (const middleware::ChannelUnboundError&) {
    log::err("failed to send request {}", publication.request);
  }
  return make_response(reply);
}

}  // namespace simulator::http
