#include "ih/processors/put_processor.hpp"

#include <pistache/http_defs.h>

#include <regex>
#include <string>

#include "ih/controllers/trading_controller.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"
#include "protocol/admin/generator.hpp"

namespace simulator::http {
PutProcessorImpl::PutProcessorImpl(
    std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
    std::shared_ptr<DatasourceController> datasource_controller,
    std::shared_ptr<ListingController> listing_controller,
    std::shared_ptr<PriceSeedController> price_seed_controller,
    std::shared_ptr<SettingController> setting_controller,
    std::shared_ptr<TradingController> trading_controller,
    std::shared_ptr<VenueController> venue_controller)
    : redirector_{
          redirect::RedirectionProcessor::create(std::move(venue_accessor))},
      datasource_controller_{std::move(datasource_controller)},
      listing_controller_{std::move(listing_controller)},
      price_seed_controller_{std::move(price_seed_controller)},
      setting_controller_{std::move(setting_controller)},
      trading_controller_{std::move(trading_controller)},
      venue_controller_{std::move(venue_controller)} {}

auto PutProcessorImpl::update_venue(const Pistache::Rest::Request& request,
                                    Pistache::Http::ResponseWriter response)
    -> void {
  const auto venue_id = request.param(":id").as<std::string>();
  log::info("requested update of the venue - {}", venue_id);

  auto [code, body] = venue_controller_->update_venue(venue_id, request.body());
  respond(request, response, code, body);
}

auto PutProcessorImpl::update_listing(const Pistache::Rest::Request& request,
                                      Pistache::Http::ResponseWriter response)
    -> void {
  const auto symbol = request.param(":symbol").as<std::string>();
  const auto key = std::regex_replace(symbol, std::regex("%2F"), "/");
  log::info("requested update of the listing - {}", key);

  auto [code, body] = listing_controller_->update_listing(key, request.body());
  respond(request, response, code, body);
}

auto PutProcessorImpl::update_data_source(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  const auto source_id = request.param(":id").as<std::uint64_t>();
  log::info("requested update of the datasource - {}", source_id);

  auto [code, body] =
      datasource_controller_->update_datasource(source_id, request.body());
  respond(request, response, code, body);
}

auto PutProcessorImpl::update_price_seed(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  const auto id = request.param(":id").as<std::uint64_t>();
  log::info("requested update of the price seed - {}", id);

  auto [code, body] =
      price_seed_controller_->update_price_seed(id, request.body());
  respond(request, response, code, body);
}

auto PutProcessorImpl::update_settings(const Pistache::Rest::Request& request,
                                       Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested update of general settings");

  auto [code, body] = setting_controller_->update_settings(request.body());
  respond(request, response, code, body);
}

auto PutProcessorImpl::respond(const Pistache::Rest::Request& request,
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

auto PutProcessorImpl::redirect(const Pistache::Rest::Request& request,
                                const std::string& instance_id) const
    -> redirect::Result {
  assert(redirector_);
  return redirector_->redirect_to_venue(
      instance_id, request.method(), request.resource());
}

}  // namespace simulator::http
