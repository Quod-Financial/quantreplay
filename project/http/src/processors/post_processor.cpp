#include "ih/processors/post_processor.hpp"

#include <pistache/http_defs.h>

#include "ih/redirect/redirection_processor.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"
#include "middleware/routing/generator_admin_channel.hpp"

namespace simulator::http {

PostProcessorImpl::PostProcessorImpl(
    std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
    std::shared_ptr<DatasourceController> datasource_controller,
    std::shared_ptr<ListingController> listing_controller,
    std::shared_ptr<PriceSeedController> price_seed_controller,
    std::shared_ptr<SettingController> setting_controller,
    std::shared_ptr<TradingController> trading_controller,
    std::shared_ptr<VenueController> venue_controller,
    ControlCallbacks callbacks)
    : redirector_{redirect::RedirectionProcessor::create(venue_accessor)},
      datasource_controller_{std::move(datasource_controller)},
      listing_controller_{std::move(listing_controller)},
      price_seed_controller_{std::move(price_seed_controller)},
      setting_controller_{std::move(setting_controller)},
      trading_controller_{std::move(trading_controller)},
      venue_controller_{std::move(venue_controller)},
      app_controller_{
          std::move(venue_accessor), cfg::venue(), std::move(callbacks)} {}

auto PostProcessorImpl::add_venue(const Pistache::Rest::Request& request,
                                  Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested insert of the new venue");

  auto [code, body] = venue_controller_->insert_venue(request.body());
  respond(request, response, code, body);
}

auto PostProcessorImpl::add_listing(const Pistache::Rest::Request& request,
                                    Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested insert of the new listing");

  auto [code, body] = listing_controller_->insert_listing(request.body());
  respond(request, response, code, body);
}

auto PostProcessorImpl::add_data_source(const Pistache::Rest::Request& request,
                                        Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested insert of the new datasource");

  auto [code, body] = datasource_controller_->insert_datasource(request.body());
  respond(request, response, code, body);
}

auto PostProcessorImpl::add_price_seed(const Pistache::Rest::Request& request,
                                       Pistache::Http::ResponseWriter response)
    -> void {
  log::info("requested insert of the new price seed");

  auto [code, body] = price_seed_controller_->insert_price_seed(request.body());
  respond(request, response, code, body);
}

auto PostProcessorImpl::sync_price_seeds(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  log::info("received a request to synchronize price seeds");

  auto [code, body] = price_seed_controller_->sync_price_seeds();
  respond(request, response, code, body);
}

auto PostProcessorImpl::stop_order_gen(const Pistache::Rest::Request& request,
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

auto PostProcessorImpl::start_order_gen(const Pistache::Rest::Request& request,
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

auto PostProcessorImpl::halt_phase(const Pistache::Rest::Request& request,
                                   Pistache::Http::ResponseWriter response)
    -> void {
  const auto instance_id = request.param(":venueId").as<std::string>();
  if (instance_id == cfg::venue().name) {
    const auto [code, body] = trading_controller_->halt(request.body());
    respond(request, response, code, body);
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PostProcessorImpl::resume_phase(const Pistache::Rest::Request& request,
                                     Pistache::Http::ResponseWriter response)
    -> void {
  const auto instance_id = request.param(":venueId").as<std::string>();
  if (instance_id == cfg::venue().name) {
    const auto [code, body] = trading_controller_->resume();
    respond(request, response, code, body);
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PostProcessorImpl::handle_store_request(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  const auto instance_id = request.hasParam(":venueId")
                               ? request.param(":venueId").as<std::string>()
                               : std::string{};

  if (instance_id.empty() || instance_id == cfg::venue().name) {
    const auto [code, body] = trading_controller_->store_market_state();
    respond(request, response, code, body);
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PostProcessorImpl::handle_recover_request(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  const auto instance_id = request.hasParam(":venueId")
                               ? request.param(":venueId").as<std::string>()
                               : std::string{};

  if (instance_id.empty() || instance_id == cfg::venue().name) {
    const auto [code, body] = trading_controller_->recover_market_state();
    respond(request, response, code, body);
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PostProcessorImpl::reset_app(const Pistache::Rest::Request& request,
                                  Pistache::Http::ResponseWriter response)
    -> void {
  const auto instance_id = request.hasParam(":venueId")
                               ? request.param(":venueId").as<std::string>()
                               : std::string{};

  if (instance_id.empty() || instance_id == cfg::venue().name) {
    log::info("before reset_app_state()");
    const auto [code, body] = app_controller_.ready_to_reset();

    respond(request, response, code, body);
    if (code == Pistache::Http::Code::Ok) {
      app_controller_.reset_app_state();
    }
  } else {
    const auto redirect_response = redirect(request, instance_id);
    respond(request,
            response,
            redirect_response.http_code(),
            redirect_response.body_content());
  }
}

auto PostProcessorImpl::respond(const Pistache::Rest::Request& request,
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

auto PostProcessorImpl::redirect(const Pistache::Rest::Request& request,
                                 const std::string& instance_id) const
    -> redirect::Result {
  assert(redirector_);
  return redirector_->redirect_to_venue(
      instance_id, request.method(), request.resource());
}

auto PostProcessorImpl::handle_generation_stop_request(
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

auto PostProcessorImpl::handle_generation_start_request(
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

}  // namespace simulator::http
