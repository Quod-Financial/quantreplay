#include "ih/controllers/listing_controller.hpp"

#include <fmt/format.h>

#include <string>
#include <utility>

#include "core/tools/numeric.hpp"
#include "data_layer/api/models/listing.hpp"
#include "ih/marshalling/json/listing.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"

namespace simulator::http {

namespace {

[[nodiscard]]
auto unmarshall_request_body(std::string_view body,
                             data_layer::Listing::Patch& dest,
                             Pistache::Http::Code& code,
                             std::string& content) noexcept -> bool {
  try {
    json::ListingUnmarshaller::unmarshall(body, dest);
    return true;
  } catch (const std::runtime_error& ex) {
    code = Pistache::Http::Code::Bad_Request;
    content = format_result_response(fmt::format("Bad request: {}", ex.what()));
  } catch (...) {
    log::err("failed to unmarshall Listing model, unexpected error raised");

    code = Pistache::Http::Code::Internal_Server_Error;
    content = format_result_response("Failed to unmarshall request body");
  }

  return false;
}

}  // namespace

ListingController::ListingController(
    data_bridge::ListingAccessor& data_accessor) noexcept
    : data_accessor_(data_accessor) {}

auto ListingController::select_listing(const std::string& key) const -> Result {
  Pistache::Http::Code code{};
  std::string content{};

  auto result = [&] {
    if (core::is_number(key)) {
      const std::uint64_t listing_id = std::stoull(key);
      return data_accessor_.get().select_single(listing_id);
    }
    return data_accessor_.get().select_single(key);
  }();

  if (result) {
    const data_layer::Listing& selected = result.value();
    try {
      content = json::ListingMarshaller::marshall(selected);
      code = Pistache::Http::Code::Ok;
    } catch (const std::exception& ex) {
      log::warn("failed to marshall listing model to JSON: {}", ex.what());
      code = Pistache::Http::Code::Internal_Server_Error;
      content = format_result_response("failed to marshall Listing model");
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

auto ListingController::select_all_listings() const -> Result {
  Pistache::Http::Code code{};
  std::string content{};

  auto result = data_accessor_.get().select_all();
  if (result) {
    try {
      const std::vector<data_layer::Listing>& selected = result.value();
      content = json::ListingMarshaller::marshall(selected);
      code = Pistache::Http::Code::Ok;
      log::info("selected {} listing records", selected.size());
    } catch (const std::exception& ex) {
      log::warn("failed to marshall listings list to JSON: {}", ex.what());
      code = Pistache::Http::Code::Internal_Server_Error;
      content = format_result_response("failed to marshall listings list");
    }
  } else {
    const auto failure = result.error();
    content = format_error_response(failure);
    code = Pistache::Http::Code::Internal_Server_Error;
  }

  return std::make_pair(code, std::move(content));
}

auto ListingController::insert_listing(const std::string& body) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content{};

  data_layer::Listing::Patch listing_snapshot;
  if (!unmarshall_request_body(body, listing_snapshot, code, content)) {
    return std::make_pair(code, std::move(content));
  }

  auto result = data_accessor_.get().add(listing_snapshot);
  if (result) {
    log::info("successfully added a new listing");
    code = Pistache::Http::Code::Created;
    content = format_result_response("Successfully inserted a new Listing");
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

auto ListingController::update_listing(const std::string& key,
                                       const std::string& body) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content{};

  data_layer::Listing::Patch patch{};
  if (!unmarshall_request_body(body, patch, code, content)) {
    return std::make_pair(code, std::move(content));
  }

  auto result = [&] {
    if (core::is_number(key)) {
      const std::uint64_t listing_id = std::stoull(key);
      return data_accessor_.get().update(std::move(patch), listing_id);
    }
    return data_accessor_.get().update(std::move(patch), key);
  }();

  if (result) {
    code = Pistache::Http::Code::Ok;
    content = format_result_response(
        fmt::format("Successfully updated the {} listing", key));

    log::info("successfully updated the listing by `{}' key", key);
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

auto ListingController::format_error_response(data_bridge::Failure failure)
    -> std::string {
  std::string message;
  switch (failure) {
    case data_bridge::Failure::MalformedInput:
      message = "Required fields missing for Listing model in request";
      break;
    case data_bridge::Failure::DatabaseConnectionError:
      message = "Database connection failed";
      break;
    case data_bridge::Failure::ResponseDecodingError:
      message = "Failed to decode a database response";
      break;
    case data_bridge::Failure::ResponseCardinalityError:
      message = "Can not resolve a single Listing by a given key";
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
