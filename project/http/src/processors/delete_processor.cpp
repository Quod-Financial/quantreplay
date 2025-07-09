#include "ih/processors/delete_processor.hpp"

#include <pistache/http_defs.h>
#include <pistache/router.h>

#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"

namespace simulator::http {

DeleteProcessor::DeleteProcessor(
    const PriceSeedController& price_seed_controller) noexcept
    : price_seed_controller_(price_seed_controller) {}

auto DeleteProcessor::delete_price_seed(const Pistache::Rest::Request& request,
                                        Pistache::Http::ResponseWriter response)
    -> void {
  auto id = request.param(":id").as<std::uint64_t>();
  log::info("requested delete of the price seed - {}", id);

  auto [code, body] = price_seed_controller_.get().delete_price_seed(id);
  respond(request, response, code, body);
}

auto DeleteProcessor::respond(const Pistache::Rest::Request& request,
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
