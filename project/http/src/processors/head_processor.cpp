#include "ih/processors/head_processor.hpp"

#include <httplib.h>
#include <pistache/http_defs.h>
#include <pistache/http_header.h>
#include <pistache/router.h>

#include <cassert>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "core/common/session_settings.hpp"
#include "ih/headers/content_disposition_attachment.hpp"
#include "ih/utils/path.hpp"
#include "log/logging.hpp"

namespace simulator::http {

HeadProcessorImpl::HeadProcessorImpl(
    std::shared_ptr<redirect::RedirectionProcessor> redirector,
    std::shared_ptr<ConfigProvider> config_provider)
    : redirector_{std::move(redirector)},
      config_provider_{std::move(config_provider)} {}

auto HeadProcessorImpl::get_data_dictionaries(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) -> void {
  const auto venue_id = httplib::detail::decode_url(
      request.param(":venueId").as<std::string>(), false);
  const auto session_id = httplib::detail::decode_url(
      request.param(":sessionId").as<std::string>(), false);
  log::info("requested data dictionaries metadata for venue '{}' session '{}'",
            venue_id,
            session_id);

  if (venue_id != config_provider_->venue_id()) {
    relay_data_dictionaries(request, response, venue_id, session_id);
    return;
  }

  if (!has_session(session_id)) {
    respond(request, response, Pistache::Http::Code::Not_Found);
    return;
  }

  respond_ok(request,
             response,
             {std::make_shared<ContentDispositionAttachment>(
                 make_dictionaries_filename(venue_id, session_id))});
}

auto HeadProcessorImpl::has_session(const std::string& session_id) const
    -> bool {
  const auto& sessions = config_provider_->session_settings();
  for (const auto& session : sessions) {
    if (session.id.has_value() && session.id.value() == session_id) {
      return true;
    }
  }
  return false;
}

auto HeadProcessorImpl::relay_data_dictionaries(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter& response,
    const std::string& venue_id,
    const std::string& session_id) const -> void {
  const auto redirect_response = redirect(request, venue_id);
  const auto code = redirect_response.http_code();

  if (code == Pistache::Http::Code::Ok) {
    // redirection does not transfer HTTP headers
    respond_ok(request,
               response,
               {std::make_shared<ContentDispositionAttachment>(
                   make_dictionaries_filename(venue_id, session_id))});
  } else {
    respond(request, response, code);
  }
}

auto HeadProcessorImpl::redirect(const Pistache::Rest::Request& request,
                                 const std::string& instance_id) const
    -> redirect::Result {
  assert(redirector_);
  return redirector_->redirect_to_venue(
      instance_id, request.method(), request.resource(), std::nullopt);
}

auto HeadProcessorImpl::respond(const Pistache::Rest::Request& request,
                                Pistache::Http::ResponseWriter& response,
                                Pistache::Http::Code code) -> void {
  log::debug("sending response on {} {} from {}:{} with code: {} ({})",
             Pistache::Http::methodString(request.method()),
             request.resource(),
             request.address().host(),
             static_cast<std::uint16_t>(request.address().port()),
             Pistache::Http::codeString(code),
             static_cast<int>(code));

  response.send(code);
}

auto HeadProcessorImpl::respond_ok(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter& response,
    const std::vector<std::shared_ptr<Pistache::Http::Header::Header>>& headers)
    -> void {
  constexpr auto code = Pistache::Http::Code::Ok;

  log::debug("sending response on {} {} from {}:{} with code: {} ({})",
             Pistache::Http::methodString(request.method()),
             request.resource(),
             request.address().host(),
             static_cast<std::uint16_t>(request.address().port()),
             Pistache::Http::codeString(code),
             static_cast<int>(code));

  for (const auto& header : headers) {
    response.headers().add(header);
  }
  response.send(code);
}

}  // namespace simulator::http
