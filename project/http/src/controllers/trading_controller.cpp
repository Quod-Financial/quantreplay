#include "ih/controllers/trading_controller.hpp"

#include "core/common/return_code.hpp"
#include "ih/marshalling/json/halt.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"
#include "middleware/routing/trading_admin_channel.hpp"
#include "protocol/admin/market_state.hpp"
#include "protocol/admin/trading_phase.hpp"

namespace simulator::http {

namespace {
auto response_attributes(protocol::HaltPhaseReply reply)
    -> std::pair<Pistache::Http::Code, std::string> {
  auto code = Pistache::Http::Code::Internal_Server_Error;
  std::string message;
  switch (reply.result.value()) {
    case protocol::HaltPhaseReply::Result::Halted:
      code = Pistache::Http::Code::Ok;
      message = "The market was successfully halted.";
      break;
    case protocol::HaltPhaseReply::Result::AlreadyHaltedByRequest:
      code = Pistache::Http::Code::Conflict;
      message = "The market is already halted.";
      break;
    case protocol::HaltPhaseReply::Result::NoActivePhase:
      code = Pistache::Http::Code::Not_Found;
      message = "There is no phase to halt.";
      break;
    case protocol::HaltPhaseReply::Result::UnableToHalt:
      code = Pistache::Http::Code::Conflict;
      message = "Unable to halt the phase.";
      break;
  }
  return std::make_pair(code, message);
}

auto response_attributes(protocol::ResumePhaseReply reply)
    -> std::pair<Pistache::Http::Code, std::string> {
  auto code = Pistache::Http::Code::Internal_Server_Error;
  std::string message;
  switch (reply.result.value()) {
    case protocol::ResumePhaseReply::Result::Resumed:
      code = Pistache::Http::Code::Ok;
      message = "The market was successfully resumed.";
      break;
    case protocol::ResumePhaseReply::Result::NoRequestedHalt:
      code = Pistache::Http::Code::Conflict;
      message = "There is no halt request to terminate.";
      break;
  }
  return std::make_pair(code, message);
}

auto response_attributes(protocol::StoreMarketStateReply reply)
    -> std::pair<Pistache::Http::Code, std::string> {
  using Result = core::code::StoreMarketState;

  auto code = Pistache::Http::Code::Internal_Server_Error;
  std::string message;
  switch (reply.result.value()) {
    case Result::Stored:
      code = Pistache::Http::Code::Created;
      message = "Matching engine state has been successfully persisted.";
      break;
    case Result::PersistenceDisabled:
      code = Pistache::Http::Code::Forbidden;
      message = "Persistence is disabled.";
      break;
    case Result::PersistenceFilePathIsEmpty:
      code = Pistache::Http::Code::Conflict;
      message = "The persistence file path is empty.";
      break;
    case Result::PersistenceFilePathIsUnreachable:
      code = Pistache::Http::Code::Conflict;
      message = "The persistence file path is unreachable.";
      break;
    case Result::ErrorWhenOpeningPersistenceFile:
      code = Pistache::Http::Code::Conflict;
      message = "An error occurs when opening the persistence file.";
      break;
    case Result::ErrorWhenWritingToPersistenceFile:
      code = Pistache::Http::Code::Conflict;
      message = "An error occurs when writing to the persistence file.";
      break;
  }
  return std::make_pair(code, message);
}

auto append_error_message(std::string_view prefix,
                          const std::string& error_message) -> std::string {
  if (error_message.empty()) {
    return fmt::format("{}.", prefix);
  }
  return fmt::format("{}: {}.", prefix, error_message);
}

auto response_attributes(const protocol::RecoverMarketStateReply& reply)
    -> std::pair<Pistache::Http::Code, std::string> {
  using Result = core::code::RecoverMarketState;

  auto code = Pistache::Http::Code::Internal_Server_Error;
  std::string message;
  switch (reply.result.value()) {
    case Result::Recovered:
      code = Pistache::Http::Code::Created;
      message = "Matching engine state has been successfully recovered.";
      break;
    case Result::PersistenceDisabled:
      code = Pistache::Http::Code::Forbidden;
      message = "Persistence is disabled.";
      break;
    case Result::PersistenceFilePathIsEmpty:
      code = Pistache::Http::Code::Conflict;
      message = "The persistence file path is empty.";
      break;
    case Result::PersistenceFilePathIsUnreachable:
      code = Pistache::Http::Code::Conflict;
      message = "The persistence file path is unreachable.";
      break;
    case Result::ErrorWhenOpeningPersistenceFile:
      code = Pistache::Http::Code::Conflict;
      message = "An error occurs when opening the persistence file.";
      break;
    case Result::PersistenceFileIsMalformed:
      code = Pistache::Http::Code::Internal_Server_Error;
      message = append_error_message("The persistence file is malformed",
                                     reply.error_message);
      break;
  }
  return std::make_pair(code, message);
}

}  // namespace

auto TradingController::halt(const std::string& body) const -> Result {
  auto code = Pistache::Http::Code::Internal_Server_Error;
  std::string message;

  protocol::HaltPhaseRequest halt_phase_request;
  protocol::HaltPhaseReply halt_phase_reply;

  try {
    json::HaltUnmarshaller::unmarshall(body, halt_phase_request);
    middleware::send_admin_request(halt_phase_request, halt_phase_reply);
  } catch (const middleware::ChannelUnboundError&) {
    log::err("failed to send request {}", halt_phase_request);
    message = "Failed to process the request.";
  } catch (const std::exception& e) {
    log::err("failed to unmarshall Halt request: {}", e.what());
    message = fmt::format("Failed to unmarshall request body: {}.", e.what());
  }

  if (halt_phase_reply.result.has_value()) {
    auto [response_code, response_message] =
        response_attributes(halt_phase_reply);
    code = response_code;
    message = std::move(response_message);
  }

  return std::make_pair(code, format_result_response(message));
}

auto TradingController::resume() const -> Result {
  auto code = Pistache::Http::Code::Internal_Server_Error;
  std::string message;

  protocol::ResumePhaseRequest resume_phase_request;
  protocol::ResumePhaseReply resume_phase_reply;

  try {
    middleware::send_admin_request(resume_phase_request, resume_phase_reply);
  } catch (const middleware::ChannelUnboundError&) {
    log::err("failed to send request {}", resume_phase_request);
    message = "Failed to process the request.";
  }

  if (resume_phase_reply.result.has_value()) {
    auto [response_code, response_message] =
        response_attributes(resume_phase_reply);
    code = response_code;
    message = std::move(response_message);
  }

  return std::make_pair(code, format_result_response(message));
}

auto TradingController::store_market_state() const -> Result {
  auto code = Pistache::Http::Code::Internal_Server_Error;
  std::string message;

  protocol::StoreMarketStateRequest request;
  protocol::StoreMarketStateReply reply;

  try {
    middleware::send_admin_request(request, reply);
  } catch (const middleware::ChannelUnboundError&) {
    log::err("failed to send request {}", request);
    message = "Failed to process the request.";
  }

  if (reply.result.has_value()) {
    auto [response_code, response_message] = response_attributes(reply);
    code = response_code;
    message = std::move(response_message);
  }

  return std::make_pair(code, format_result_response(message));
}

auto TradingController::recover_market_state() const -> Result {
  auto code = Pistache::Http::Code::Internal_Server_Error;
  std::string message;

  protocol::RecoverMarketStateRequest request;
  protocol::RecoverMarketStateReply reply;

  try {
    middleware::send_admin_request(request, reply);
  } catch (const middleware::ChannelUnboundError&) {
    log::err("failed to send request {}", request);
    message = "Failed to process the request.";
  }

  if (reply.result.has_value()) {
    auto [response_code, response_message] = response_attributes(reply);
    code = response_code;
    message = std::move(response_message);
  }

  return std::make_pair(code, format_result_response(message));
}

}  // namespace simulator::http