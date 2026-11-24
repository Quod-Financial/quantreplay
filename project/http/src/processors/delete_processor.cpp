#include "ih/processors/delete_processor.hpp"

#include <pistache/http_defs.h>
#include <pistache/router.h>

#include <optional>
#include <string>
#include <utility>

#include "core/domain/attributes.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"

namespace simulator::http {

DeleteProcessorImpl::DeleteProcessorImpl(
    std::shared_ptr<redirect::RedirectionProcessor> redirector,
    std::shared_ptr<PriceSeedController> price_seed_controller,
    std::shared_ptr<QuoteController> quote_controller,
    std::string venue_id) noexcept
    : redirector_{std::move(redirector)},
      price_seed_controller_{std::move(price_seed_controller)},
      quote_controller_{std::move(quote_controller)},
      venue_id_{std::move(venue_id)} {}

auto DeleteProcessorImpl::delete_price_seed(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  auto id = request.param(":id").as<std::uint64_t>();
  log::info("requested delete of the price seed - {}", id);

  auto [code, body] = price_seed_controller_->delete_price_seed(id);
  respond(request, response, code, body);
}

auto DeleteProcessorImpl::cancel_quote(const Pistache::Rest::Request& request,
                                       Pistache::Http::ResponseWriter response)
    -> void {
  const QuoteId quote_id{request.param(":quoteId").as<std::string>()};
  log::info("requested cancellation of the `{}' quote", quote_id.value());

  const auto venue_id = request.query().get("venueId").value_or(std::string{});
  if (venue_id.empty() || venue_id == venue_id_) {
    const auto [code, body] = quote_controller_->cancel_quote(quote_id);
    respond(request, response, code, body);
  } else {
    const auto redirect_response = redirector_->redirect_to_venue(
        venue_id, request.method(), request.resource(), std::nullopt);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto DeleteProcessorImpl::respond(const Pistache::Rest::Request& request,
                                  Pistache::Http::ResponseWriter& response,
                                  Pistache::Http::Code code,
                                  const std::string& body) -> void {
  log::debug("sending response on {} {} from {}:{} with code: {} ({})",
             Pistache::Http::methodString(request.method()),
             request.resource(),
             request.address().host(),
             static_cast<std::uint16_t>(request.address().port()),
             Pistache::Http::codeString(code),
             static_cast<int>(code));

  response.send(code, body);
}

}  // namespace simulator::http
