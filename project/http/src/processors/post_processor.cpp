#include "ih/processors/post_processor.hpp"

#include <pistache/http_defs.h>

#include "ih/redirect/redirection_processor.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"

namespace simulator::http {

PostProcessor::PostProcessor(const data_bridge::VenueAccessor& venue_accessor,
                             const DatasourceController& datasource_controller,
                             const ListingController& listing_controller,
                             const PriceSeedController& price_seed_controller,
                             const SettingController& setting_controller,
                             const TradingController& trading_controller,
                             const VenueController& venue_controller)
    : redirector_(redirect::RedirectionProcessor::create(venue_accessor)),
      datasource_controller_(datasource_controller),
      listing_controller_(listing_controller),
      price_seed_controller_(price_seed_controller),
      setting_controller_(setting_controller),
      trading_controller_(trading_controller),
      venue_controller_(venue_controller) {}

auto PostProcessor::add_venue(const Pistache::Rest::Request& request,
                              Pistache::Http::ResponseWriter response) -> void {
  log::info("requested insert of the new venue");

  auto [code, body] = venue_controller_.get().insert_venue(request.body());
  respond(request, response, code, body);
}

auto PostProcessor::add_listing(const Pistache::Rest::Request& request,
                                Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested insert of the new listing");

  auto [code, body] = listing_controller_.get().insert_listing(request.body());
  respond(request, response, code, body);
}

auto PostProcessor::add_data_source(const Pistache::Rest::Request& request,
                                    Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested insert of the new datasource");

  auto [code, body] =
      datasource_controller_.get().insert_datasource(request.body());
  respond(request, response, code, body);
}

auto PostProcessor::add_price_seed(const Pistache::Rest::Request& request,
                                   Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested insert of the new price seed");

  auto [code, body] =
      price_seed_controller_.get().insert_price_seed(request.body());
  respond(request, response, code, body);
}

auto PostProcessor::handle_store_request(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  const auto instance_id = request.hasParam(":venueId")
                               ? request.param(":venueId").as<std::string>()
                               : std::string{};

  if (instance_id.empty() || instance_id == cfg::venue().name) {
    const auto [code, body] = trading_controller_.get().store_market_state();
    respond(request, response, code, body);
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PostProcessor::handle_recover_request(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  const auto instance_id = request.hasParam(":venueId")
                               ? request.param(":venueId").as<std::string>()
                               : std::string{};

  if (instance_id.empty() || instance_id == cfg::venue().name) {
    const auto [code, body] = trading_controller_.get().recover_market_state();
    respond(request, response, code, body);
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PostProcessor::respond(const Pistache::Rest::Request& request,
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

auto PostProcessor::redirect(const Pistache::Rest::Request& request,
                             const std::string& instance_id) const
    -> redirect::Result {
  assert(redirector_);
  return redirector_->redirect_to_venue(
      instance_id, request.method(), request.resource());
}

}  // namespace simulator::http
