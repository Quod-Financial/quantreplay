#include "ih/processors/get_processor.hpp"

#include <fmt/format.h>
#include <pistache/http_defs.h>
#include <pistache/router.h>

#include <cassert>
#include <regex>
#include <utility>

#include "cfg/api/cfg.hpp"
#include "ih/endpoint.hpp"
#include "ih/marshalling/json/venue.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"
#include "middleware/routing/generator_admin_channel.hpp"
#include "protocol/admin/generator.hpp"

namespace simulator::http {

GetProcessor::GetProcessor(const data_bridge::VenueAccessor& venue_accessor,
                           const DatasourceController& datasource_controller,
                           const ListingController& listing_controller,
                           const PriceSeedController& price_seed_controller,
                           const SettingController& setting_controller,
                           const VenueController& venue_controller)
    : redirector_(redirect::RedirectionProcessor::create(venue_accessor)),
      venue_accessor_(venue_accessor),
      datasource_controller_(datasource_controller),
      listing_controller_(listing_controller),
      price_seed_controller_(price_seed_controller),
      setting_controller_(setting_controller),
      venue_controller_(venue_controller) {}

auto GetProcessor::get_venue(const Pistache::Rest::Request& request,
                             Pistache::Http::ResponseWriter response) -> void {
  const auto venue_id = request.param(":id").as<std::string>();
  log::info("requested venue - {}", venue_id);

  auto [code, body] = venue_controller_.get().select_venue(venue_id);
  respond(request, response, code, body);
}

auto GetProcessor::get_venues(const Pistache::Rest::Request& request,
                              Pistache::Http::ResponseWriter response) -> void {
  log::info("requested all venues");

  auto [code, body] = venue_controller_.get().select_all_venues();
  respond(request, response, code, body);
}

auto GetProcessor::get_listing(const Pistache::Rest::Request& request,
                               Pistache::Http::ResponseWriter response)
    -> void {
  const auto symbol = request.param(":symbol").as<std::string>();
  const auto key = std::regex_replace(symbol, std::regex("%2F"), "/");
  log::info("requested listing - {}", key);

  auto [code, body] = listing_controller_.get().select_listing(key);
  respond(request, response, code, body);
}

auto GetProcessor::get_listings(const Pistache::Rest::Request& request,
                                Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested all listings");

  auto [code, body] = listing_controller_.get().select_all_listings();
  respond(request, response, code, body);
}

auto GetProcessor::get_data_source(const Pistache::Rest::Request& request,
                                   Pistache::Http::ResponseWriter response)
    -> void {
  const auto source_id = request.param(":id").as<std::uint64_t>();
  log::info("requested a data source with identifier - {}", source_id);

  auto [code, body] = datasource_controller_.get().select_datasource(source_id);
  respond(request, response, code, body);
}

auto GetProcessor::get_data_sources(const Pistache::Rest::Request&,
                                    Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested all data sources");

  auto [code, body] = datasource_controller_.get().select_all_datasources();
  response.send(code, body);
}

auto GetProcessor::get_price_seed(const Pistache::Rest::Request& request,
                                  Pistache::Http::ResponseWriter response)
    -> void {
  const auto id = request.param(":id").as<std::uint64_t>();
  log::info("requested price seed - {}", id);

  auto [code, body] = price_seed_controller_.get().select_price_seed(id);
  respond(request, response, code, body);
}

auto GetProcessor::get_price_seeds(const Pistache::Rest::Request& request,
                                   Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested all price seeds");

  auto [code, body] = price_seed_controller_.get().select_all_price_seeds();
  respond(request, response, code, body);
}

auto GetProcessor::get_status(const Pistache::Rest::Request& request,
                              Pistache::Http::ResponseWriter response) -> void {
  Pistache::Http::Code response_code{};
  std::string response_body;

  const auto result = venue_accessor_.get().select_single(cfg::venue().name);
  if (result) {
    bool available = false;
    response_body = get_venue_status_str(result.value(), false, available);
    response_code = available ? Pistache::Http::Code::Ok
                              : Pistache::Http::Code::Service_Unavailable;
  } else {
    response_code = Pistache::Http::Code::Service_Unavailable;
    response_body = format_result_response("failed to select venue");
  }

  respond(request, response, response_code, response_body);
}

auto GetProcessor::get_venue_status(const Pistache::Rest::Request& request,
                                    Pistache::Http::ResponseWriter response)
    -> void {
  const auto venue_id = request.param(":id").as<std::string>();
  log::info("requested status of venue - {}", venue_id);

  Pistache::Http::Code response_code{};
  std::string response_body;

  const auto result = venue_accessor_.get().select_single(venue_id);
  if (result) {
    bool available = false;
    response_body = get_venue_status_str(result.value(), true, available);
    response_code = available ? Pistache::Http::Code::Ok
                              : Pistache::Http::Code::Service_Unavailable;
  } else {
    response_code = Pistache::Http::Code::Service_Unavailable;
    response_body = format_result_response("failed to select venue");
  }

  respond(request, response, response_code, response_body);
}

auto GetProcessor::get_venue_statuses(const Pistache::Rest::Request& request,
                                      Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested statuses of all venues");

  Pistache::Http::Code response_code{};
  std::string response_body;
  bool available = false;

  const auto result = venue_accessor_.get().select_all();
  if (!result) {
    response_code = Pistache::Http::Code::Service_Unavailable;
    response_body = format_result_response("failed to select venues");
    respond(request, response, response_code, response_body);
    return;
  }

  for (const auto& venue : result.value()) {
    if (!response_body.empty()) {
      response_body.append(",");
    }
    response_body.append(get_venue_status_str(venue, true, available));
  }

  response_code = Pistache::Http::Code::Ok;
  response_body =
      std::string("{\"venuestatus\": [").append(response_body).append("]}");

  respond(request, response, response_code, response_body);
}

auto GetProcessor::get_venue_status_str(const data_layer::Venue& venue,
                                        bool send_response_code,
                                        bool& available) const -> std::string {
  available = false;
  const auto& venue_id = venue.venue_id();

  if (venue_id == cfg::venue().name) {
    available = true;
  } else {
    auto result = redirector_->redirect_to_venue(
        venue_id,
        Pistache::Http::Method::Get,
        fmt::format(endpoint::VenueStatusByVenueIdFmt, venue_id));
    available = result.http_code() == Pistache::Http::Code::Ok;
  }

  const auto response_code = available
                                 ? Pistache::Http::Code::Ok
                                 : Pistache::Http::Code::Service_Unavailable;
  return format_venue_status(
      venue, send_response_code ? static_cast<int>(response_code) : 0);
}

auto GetProcessor::get_settings(
    [[maybe_unused]] const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  log::info("requested all settings");

  auto [code, body] = setting_controller_.get().select_all_settings();
  respond(request, response, code, body);
}

auto GetProcessor::get_order_gen_status(const Pistache::Rest::Request& request,
                                        Pistache::Http::ResponseWriter response)
    -> void {
  const auto instance_id = request.param(":venueId").as<std::string>();
  log::info("received request to retrieve random order generator status for {}",
            instance_id);

  if (instance_id == cfg::venue().name) {
    handle_generation_status_request(request, std::move(response));
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto GetProcessor::handle_generation_status_request(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  protocol::GenerationStatusReply reply;

  try {
    protocol::GenerationStatusRequest protocol_request;
    middleware::send_admin_request(protocol_request, reply);
  } catch (const middleware::ChannelUnboundError&) {
  }

  if (reply.status == protocol::GenerationStatusReply::Status::Stopped) {
    respond(request,
            response,
            Pistache::Http::Code::Ok,
            format_result_response("Stopped"));
  } else if (reply.status == protocol::GenerationStatusReply::Status::Running) {
    respond(request,
            response,
            Pistache::Http::Code::Ok,
            format_result_response("Running"));
  } else {
    respond(request,
            response,
            Pistache::Http::Code::Internal_Server_Error,
            format_result_response("Failed to retrieve generation status"));
  }
}

auto GetProcessor::respond(const Pistache::Rest::Request& request,
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

auto GetProcessor::redirect(const Pistache::Rest::Request& request,
                            const std::string& instance_id) const
    -> redirect::Result {
  assert(redirector_);
  return redirector_->redirect_to_venue(
      instance_id, request.method(), request.resource());
}

}  // namespace simulator::http
