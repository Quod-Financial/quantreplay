#include "fix/generator_initiator/transport.hpp"

#include <exception>

#include "ih/implementation.hpp"
#include "log/logging.hpp"

namespace simulator::fix {
namespace {

auto send_request_message(
    const protocol::MarketDataRequest& request,
    GeneratorInitiator::Implementation& initiator) noexcept -> void {
  try {
    initiator.requester().process_request(request);
  } catch (const std::exception& exception) {
    log::err(
        "failed to send request message, an error occurred: {}, "
        "undelivered message - {}",
        exception.what(),
        request);
  } catch (...) {
    log::err(
        "failed to send request message, unknown error occurred, "
        "undelivered message - {}",
        request);
  }
}

}  // namespace

auto send_request(const protocol::MarketDataRequest& request,
                  GeneratorInitiator& initiator) noexcept -> void {
  log::debug("fix initiator sending MarketDataRequest");
  send_request_message(request, initiator.implementation());
}

}  // namespace simulator::fix
