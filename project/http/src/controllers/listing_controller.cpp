#include "ih/controllers/listing_controller.hpp"

#include <fmt/format.h>

#include <cstdint>
#include <optional>
#include <string>
#include <tl/expected.hpp>
#include <unordered_set>
#include <utility>
#include <vector>

#include "core/tools/numeric.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "ih/marshalling/json/detail/enumeration_resolver.hpp"
#include "ih/marshalling/json/detail/keys.hpp"
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

struct RandomPriceSettings {
  std::unordered_set<std::uint64_t> datasource_ids;
  std::optional<std::string> venue_id;
};

[[nodiscard]]
auto resolve_random_price_settings(
    const data_layer::Listing::Patch& patch,
    const std::optional<data_layer::Listing>& current) -> RandomPriceSettings {
  RandomPriceSettings settings;

  if (current.has_value()) {
    settings.venue_id = current->venue_id();
  }

  if (const auto& patched_sources = patch.random_price_sources()) {
    for (const auto& patched : *patched_sources) {
      settings.datasource_ids.emplace(patched.datasource_id());
    }
  }
  if (patch.venue_id().has_value()) {
    settings.venue_id = *patch.venue_id();
  }

  return settings;
}

}  // namespace

ListingController::ListingController(
    std::unique_ptr<data_bridge::ListingAccessor> data_accessor,
    std::unique_ptr<data_bridge::DatasourceAccessor>
        datasource_accessor) noexcept
    : data_accessor_{std::move(data_accessor)},
      datasource_accessor_{std::move(datasource_accessor)} {}

auto ListingController::select_listing(const std::string& key) const -> Result {
  Pistache::Http::Code code{};
  std::string content{};

  auto result = [&] {
    if (core::is_number(key)) {
      const std::uint64_t listing_id = std::stoull(key);
      return data_accessor_->select_single(listing_id);
    }
    return data_accessor_->select_single(key);
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

  auto result = data_accessor_->select_all();
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

  if (auto validation =
          validate_random_price_source(listing_snapshot, std::nullopt);
      !validation.has_value()) {
    return std::move(validation.error());
  }

  auto result = data_accessor_->add(listing_snapshot);
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

  if (patch.random_price_sources().has_value()) {
    auto current = [&] {
      if (core::is_number(key)) {
        return data_accessor_->select_single(std::stoull(key));
      }
      return data_accessor_->select_single(key);
    }();

    if (!current) {
      const auto failure = current.error();
      content = format_error_response(failure);
      code = failure == data_bridge::Failure::ResponseCardinalityError
                 ? Pistache::Http::Code::Not_Found
                 : Pistache::Http::Code::Internal_Server_Error;
      return std::make_pair(code, std::move(content));
    }

    if (auto validation = validate_random_price_source(patch, current.value());
        !validation.has_value()) {
      return std::move(validation.error());
    }
  }

  auto result = [&] {
    if (core::is_number(key)) {
      const std::uint64_t listing_id = std::stoull(key);
      return data_accessor_->update(std::move(patch), listing_id);
    }
    return data_accessor_->update(std::move(patch), key);
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

auto ListingController::validate_random_price_source(
    const data_layer::Listing::Patch& patch,
    const std::optional<data_layer::Listing>& current) const -> Validation {
  const auto settings = resolve_random_price_settings(patch, current);
  return validate_random_price_datasources(settings.datasource_ids,
                                           settings.venue_id);
}

auto ListingController::validate_random_price_datasources(
    const std::unordered_set<std::uint64_t>& datasource_ids,
    const std::optional<std::string>& venue_id) const -> Validation {
  for (const std::uint64_t datasource_id : datasource_ids) {
    auto validation =
        select_random_price_datasource(datasource_id)
            .and_then(validate_datasource_format)
            .and_then([&venue_id](const data_layer::Datasource& datasource) {
              return validate_datasource_venue(datasource, venue_id);
            });
    if (!validation.has_value()) {
      return validation;
    }
  }

  return {};
}

auto ListingController::select_random_price_datasource(
    std::uint64_t datasource_id) const
    -> tl::expected<data_layer::Datasource, Result> {
  auto datasource = datasource_accessor_->select_single(datasource_id);
  if (datasource.has_value()) {
    return std::move(*datasource);
  }

  const auto& failure = datasource.error();
  if (failure.failure == data_bridge::Failure::ResponseCardinalityError) {
    return tl::make_unexpected(
        Result{Pistache::Http::Code::Bad_Request,
               format_result_response(fmt::format(
                   "{} `{}' does not refer to an existing data source",
                   json::listing_random_price_source_key::DatasourceId,
                   datasource_id))});
  }

  log::warn("failed to select the `{}' data source referenced by {}: {}",
            datasource_id,
            json::listing_random_price_source_key::DatasourceId,
            failure.message);
  return tl::make_unexpected(
      Result{Pistache::Http::Code::Internal_Server_Error,
             format_result_response(fmt::format(
                 "Failed to select the data source referenced by {}",
                 json::listing_random_price_source_key::DatasourceId))});
}

auto ListingController::validate_datasource_format(
    data_layer::Datasource datasource)
    -> tl::expected<data_layer::Datasource, Result> {
  if (datasource.format() == data_layer::Datasource::Format::Fix) {
    return datasource;
  }

  return tl::make_unexpected(
      Result{Pistache::Http::Code::Bad_Request,
             format_result_response(fmt::format(
                 "{} `{}' must refer to a data source with the {} format",
                 json::listing_random_price_source_key::DatasourceId,
                 datasource.datasource_id(),
                 json::EnumerationResolver::resolve(
                     data_layer::Datasource::Format::Fix)))});
}

auto ListingController::validate_datasource_venue(
    const data_layer::Datasource& datasource,
    const std::optional<std::string>& venue_id) -> Validation {
  if (!venue_id.has_value() || datasource.venue_id() == *venue_id) {
    return {};
  }

  return tl::make_unexpected(
      Result{Pistache::Http::Code::Bad_Request,
             format_result_response(fmt::format(
                 "{} `{}' must refer to a data source on the `{}' venue",
                 json::listing_random_price_source_key::DatasourceId,
                 datasource.datasource_id(),
                 *venue_id))});
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
