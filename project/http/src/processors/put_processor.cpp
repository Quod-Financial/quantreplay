#include "ih/processors/put_processor.hpp"

#include <pistache/http_defs.h>

#include <regex>
#include <string>

#include "ih/controllers/trading_controller.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"
#include "middleware/routing/generator_admin_channel.hpp"
#include "protocol/admin/generator.hpp"

namespace simulator::http {

PutProcessor::PutProcessor(const data_bridge::VenueAccessor& venue_accessor,
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

auto PutProcessor::update_venue(const Pistache::Rest::Request& request,
                                Pistache::Http::ResponseWriter response)
    -> void {
  const auto venue_id = request.param(":id").as<std::string>();
  log::info("requested update of the venue - {}", venue_id);

  auto [code, body] =
      venue_controller_.get().update_venue(venue_id, request.body());
  respond(request, response, code, body);
}

auto PutProcessor::update_listing(const Pistache::Rest::Request& request,
                                  Pistache::Http::ResponseWriter response)
    -> void {
  const auto symbol = request.param(":symbol").as<std::string>();
  const auto key = std::regex_replace(symbol, std::regex("%2F"), "/");
  log::info("requested update of the listing - {}", key);

  auto [code, body] =
      listing_controller_.get().update_listing(key, request.body());
  respond(request, response, code, body);
}

auto PutProcessor::update_data_source(const Pistache::Rest::Request& request,
                                      Pistache::Http::ResponseWriter response)
    -> void {
  const auto source_id = request.param(":id").as<std::uint64_t>();
  log::info("requested update of the datasource - {}", source_id);

  auto [code, body] =
      datasource_controller_.get().update_datasource(source_id, request.body());
  respond(request, response, code, body);
}

auto PutProcessor::update_price_seed(const Pistache::Rest::Request& request,
                                     Pistache::Http::ResponseWriter response)
    -> void {
  const auto id = request.param(":id").as<std::uint64_t>();
  log::info("requested update of the price seed - {}", id);

  auto [code, body] =
      price_seed_controller_.get().update_price_seed(id, request.body());
  respond(request, response, code, body);
}

auto PutProcessor::sync_price_seeds(const Pistache::Rest::Request& request,
                                    Pistache::Http::ResponseWriter response)
    -> void {
  log::info("received a request to synchronize price seeds");

  auto [code, body] = price_seed_controller_.get().sync_price_seeds();
  respond(request, response, code, body);
}

auto PutProcessor::update_settings(const Pistache::Rest::Request& request,
                                   Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested update of general settings");

  auto [code, body] = setting_controller_.get().update_settings(request.body());
  respond(request, response, code, body);
}

auto PutProcessor::stop_order_gen(const Pistache::Rest::Request& request,
                                  Pistache::Http::ResponseWriter response)
    -> void {
  const auto instance_id = request.param(":venueId").as<std::string>();
  log::info("requested to stop order generation for {}", instance_id);

  if (instance_id == cfg::venue().name) {
    handle_generation_stop_request(request, std::move(response));
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PutProcessor::start_order_gen(const Pistache::Rest::Request& request,
                                   Pistache::Http::ResponseWriter response)
    -> void {
  const auto instance_id = request.param(":venueId").as<std::string>();
  log::info("requested to start order generation for {}", instance_id);

  if (instance_id == cfg::venue().name) {
    handle_generation_start_request(request, std::move(response));
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PutProcessor::halt_phase(const Pistache::Rest::Request& request,
                              Pistache::Http::ResponseWriter response) -> void {
  const auto instance_id = request.param(":venueId").as<std::string>();
  if (instance_id == cfg::venue().name) {
    const auto [code, body] = trading_controller_.get().halt(request.body());
    respond(request, response, code, body);
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PutProcessor::resume_phase(const Pistache::Rest::Request& request,
                                Pistache::Http::ResponseWriter response)
    -> void {
  const auto instance_id = request.param(":venueId").as<std::string>();
  if (instance_id == cfg::venue().name) {
    const auto [code, body] = trading_controller_.get().resume();
    respond(request, response, code, body);
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PutProcessor::handle_generation_start_request(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  protocol::StartGenerationReply reply;

  try {
    protocol::StartGenerationRequest protocol_request;
    middleware::send_admin_request(protocol_request, reply);
  } catch (const middleware::ChannelUnboundError&) {
  }

  if (reply.result.has_value() &&
      *reply.result == protocol::StartGenerationReply::Result::Started) {
    respond(request,
            response,
            Pistache::Http::Code::Ok,
            format_result_response("Generator was started"));
  } else {
    respond(request,
            response,
            Pistache::Http::Code::Internal_Server_Error,
            format_result_response("Failed to start the generator"));
  }
}

auto PutProcessor::handle_generation_stop_request(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  protocol::StopGenerationReply reply;

  try {
    protocol::StopGenerationRequest protocol_request;
    middleware::send_admin_request(protocol_request, reply);
  } catch (const middleware::ChannelUnboundError&) {
  }

  if (reply.result.has_value() &&
      *reply.result == protocol::StopGenerationReply::Result::Stopped) {
    respond(request,
            response,
            Pistache::Http::Code::Ok,
            format_result_response("Generator was stopped"));
  } else {
    respond(request,
            response,
            Pistache::Http::Code::Internal_Server_Error,
            format_result_response("Failed to stop the generator"));
  }
}

auto PutProcessor::respond(const Pistache::Rest::Request& request,
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

auto PutProcessor::redirect(const Pistache::Rest::Request& request,
                            const std::string& instance_id) const
    -> redirect::Result {
  assert(redirector_);
  return redirector_->redirect_to_venue(
      instance_id, request.method(), request.resource());
}

}  // namespace simulator::http
