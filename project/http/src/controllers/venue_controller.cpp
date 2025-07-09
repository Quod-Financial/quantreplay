#include "ih/controllers/venue_controller.hpp"

#include <fmt/format.h>

#include <string>
#include <utility>

#include "data_layer/api/models/venue.hpp"
#include "ih/marshalling/json/venue.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"

namespace simulator::http {

namespace {

[[nodiscard]]
auto unmarshall_request_body(std::string_view body,
                             data_layer::Venue::Patch& dest,
                             Pistache::Http::Code& code,
                             std::string& content) noexcept -> bool {
  try {
    json::VenueUnmarshaller::unmarshall(body, dest);
    return true;
  } catch (const std::runtime_error& ex) {
    code = Pistache::Http::Code::Bad_Request;
    content = format_result_response(fmt::format("Bad request: {}", ex.what()));
  } catch (...) {
    log::err("failed to unmarshall Venue model, unexpected error raised");
    code = Pistache::Http::Code::Internal_Server_Error;
    content = format_result_response("Failed to unmarshall request body");
  }

  return false;
}

}  // namespace

VenueController::VenueController(data_bridge::VenueAccessor& bridge) noexcept
    : data_accessor_(bridge) {}

auto VenueController::select_venue(const std::string& venue_id) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content;

  auto result = data_accessor_.get().select_single(venue_id);
  if (result) {
    const data_layer::Venue& selected = result.value();
    try {
      content = json::VenueMarshaller::marshall(selected);
      code = Pistache::Http::Code::Ok;
    } catch (const std::exception& ex) {
      log::warn("failed to marshall venue model to JSON: {}", ex.what());
      code = Pistache::Http::Code::Internal_Server_Error;
      content = format_result_response("failed to marshall Venue model");
    }
  } else {
    const auto failure = result.error();
    content = format_error_response(failure);
    switch (failure) {
      case data_bridge::Failure::ResponseCardinalityError:
        code = Pistache::Http::Code::Not_Found;
        break;
      default:
        code = Pistache::Http::Code::Internal_Server_Error;
        break;
    }
  }

  return std::make_pair(code, std::move(content));
}

auto VenueController::select_all_venues() const -> Result {
  Pistache::Http::Code code{};
  std::string content;

  auto result = data_accessor_.get().select_all();
  if (result) {
    try {
      const std::vector<data_layer::Venue>& venues = result.value();
      content = json::VenueMarshaller::marshall(venues);
      code = Pistache::Http::Code::Ok;
      log::info("selected {} venue records", venues.size());
    } catch (const std::exception& ex) {
      log::warn("failed to marshall venues list to JSON: {}", ex.what());
      code = Pistache::Http::Code::Internal_Server_Error;
      content = format_result_response("failed to marshall venues list");
    }
  } else {
    const auto failure = result.error();
    content = format_error_response(failure);
    code = Pistache::Http::Code::Internal_Server_Error;
  }

  return std::make_pair(code, std::move(content));
}

auto VenueController::insert_venue(const std::string& body) const -> Result {
  Pistache::Http::Code code{};
  std::string content;

  data_layer::Venue::Patch venue_snapshot;
  if (!unmarshall_request_body(body, venue_snapshot, code, content)) {
    return std::make_pair(code, std::move(content));
  }

  auto result = data_accessor_.get().add(venue_snapshot);
  if (result) {
    log::info("successfully added a new venue");
    code = Pistache::Http::Code::Created;
    content = format_result_response("Successfully inserted a new Venue");
  } else {
    const auto failure = result.error();
    content = format_error_response(failure);
    switch (failure) {
      case data_bridge::Failure::MalformedInput:
      case data_bridge::Failure::DataIntegrityViolationError:
        code = Pistache::Http::Code::Bad_Request;
        break;
      default:
        code = Pistache::Http::Code::Internal_Server_Error;
        break;
    }
  }

  return std::make_pair(code, std::move(content));
}

auto VenueController::update_venue(const std::string& venue_id,
                                   const std::string& body) const -> Result {
  Pistache::Http::Code code{};
  std::string content;

  data_layer::Venue::Patch patch;
  if (!unmarshall_request_body(body, patch, code, content)) {
    return std::make_pair(code, std::move(content));
  }

  auto result = data_accessor_.get().update(patch, venue_id);
  if (result) {
    code = Pistache::Http::Code::Ok;
    content = format_result_response(
        fmt::format("Successfully updated the {} venue", venue_id));

    log::info("successfully updated the venue with `{}' identifier", venue_id);
  } else {
    const auto failure = result.error();
    content = format_error_response(failure);
    switch (failure) {
      case data_bridge::Failure::MalformedInput:
      case data_bridge::Failure::DataIntegrityViolationError:
        code = Pistache::Http::Code::Bad_Request;
        break;
      case data_bridge::Failure::ResponseCardinalityError:
        code = Pistache::Http::Code::Not_Found;
        break;
      default:
        code = Pistache::Http::Code::Internal_Server_Error;
        break;
    }
  }

  return std::make_pair(code, std::move(content));
}

auto VenueController::format_error_response(data_bridge::Failure failure)
    -> std::string {
  std::string message;
  switch (failure) {
    case data_bridge::Failure::MalformedInput:
      message = "Required fields missing for Venue model in request";
      break;
    case data_bridge::Failure::DatabaseConnectionError:
      message = "Database connection failed";
      break;
    case data_bridge::Failure::ResponseDecodingError:
      message = "Failed to decode a database response";
      break;
    case data_bridge::Failure::ResponseCardinalityError:
      message = "Can not resolve a single Venue by a given key";
      break;
    case data_bridge::Failure::DataIntegrityViolationError:
      message = "Requested operation violates data integrity constraints";
      break;
    case data_bridge::Failure::UnknownError:
      message = "Unknown server error occurred";
      break;
  }
  return format_result_response(message);
}

}  // namespace simulator::http
