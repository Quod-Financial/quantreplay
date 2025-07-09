#include "ih/controllers/price_seed_controller.hpp"

#include <fmt/chrono.h>

#include <ctime>
#include <string>
#include <utility>

#include "data_layer/api/models/price_seed.hpp"
#include "data_layer/api/models/setting.hpp"
#include "ih/marshalling/json/price_seed.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"

namespace simulator::http {

namespace {

[[nodiscard]]
auto unmarshall_request_body(std::string_view body,
                             data_layer::PriceSeed::Patch& dest,
                             Pistache::Http::Code& code,
                             std::string& content) noexcept -> bool {
  try {
    json::PriceSeedUnmarshaller::unmarshall(body, dest);
    return true;
  } catch (const std::runtime_error& ex) {
    code = Pistache::Http::Code::Bad_Request;
    content = format_result_response(fmt::format("Bad request: {}", ex.what()));
  } catch (...) {
    log::err("failed to unmarshall PriceSeed model, unexpected error raised");
    code = Pistache::Http::Code::Internal_Server_Error;
    content = format_result_response("Failed to unmarshall request body");
  }

  return false;
}

[[nodiscard]]
auto make_seed_update_time_patch() -> data_layer::Setting::Patch {
  using std::chrono::system_clock;

  const std::time_t now = system_clock::to_time_t(system_clock::now());
  std::tm utc_time{};
  gmtime_r(&now, &utc_time);

  std::string update_time = fmt::format("{:%F %T}", utc_time);
  return data_layer::Setting::Patch{}.with_value(std::move(update_time));
}

}  // namespace

PriceSeedController::PriceSeedController(
    const data_bridge::PriceSeedAccessor& seed_accessor,
    const data_bridge::SettingAccessor& setting_accessor) noexcept
    : seed_accessor_(seed_accessor), setting_accessor_(setting_accessor) {}

auto PriceSeedController::select_price_seed(std::uint64_t seed_id) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content;

  auto result = seed_accessor_.get().select_single(seed_id);
  if (result) {
    const data_layer::PriceSeed& selected = result.value();
    try {
      content = json::PriceSeedMarshaller::marshall(selected);
      code = Pistache::Http::Code::Ok;
    } catch (const std::exception& ex) {
      log::warn("failed to marshall price seed to JSON: {}", ex.what());
      code = Pistache::Http::Code::Internal_Server_Error;
      content = format_result_response("failed to marshall price seed");
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

auto PriceSeedController::select_all_price_seeds() const -> Result {
  Pistache::Http::Code code{};
  std::string content;

  auto result = seed_accessor_.get().select_all();
  if (result) {
    try {
      const std::vector<data_layer::PriceSeed>& seeds = result.value();
      content = json::PriceSeedMarshaller::marshall(seeds);
      code = Pistache::Http::Code::Ok;
      log::info("selected {} price seed records", seeds.size());
    } catch (const std::exception& ex) {
      log::warn("failed to marshall price seeds list to JSON: {}", ex.what());
      code = Pistache::Http::Code::Internal_Server_Error;
      content = format_result_response("failed to marshall price seeds");
    }
  } else {
    const auto failure = result.error();
    content = format_error_response(failure);
    code = Pistache::Http::Code::Internal_Server_Error;
  }

  return std::make_pair(code, std::move(content));
}

auto PriceSeedController::insert_price_seed(const std::string& body) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content;

  data_layer::PriceSeed::Patch seed_snapshot;
  if (!unmarshall_request_body(body, seed_snapshot, code, content)) {
    return std::make_pair(code, std::move(content));
  }

  auto result = seed_accessor_.get().add(seed_snapshot);
  if (result) {
    log::info("successfully added a new price seed");
    code = Pistache::Http::Code::Created;
    content = format_result_response("Successfully inserted new price seed");
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

auto PriceSeedController::update_price_seed(std::uint64_t seed_id,
                                            const std::string& body) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content;

  data_layer::PriceSeed::Patch patch{};
  if (!unmarshall_request_body(body, patch, code, content)) {
    return std::make_pair(code, std::move(content));
  }

  auto result = seed_accessor_.get().update(patch, seed_id);
  if (result) {
    code = Pistache::Http::Code::Ok;
    content = format_result_response(fmt::format(
        "Successfully updated the price seed with {} identifier", seed_id));

    log::info("successfully updated the price seed with `{}' identifier",
              seed_id);
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

auto PriceSeedController::delete_price_seed(std::uint64_t seed_id) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content;

  auto result = seed_accessor_.get().drop_single(seed_id);
  if (result) {
    code = Pistache::Http::Code::No_Content;
    content.clear();
  } else {
    const auto failure = result.error();
    content = format_error_response(failure);
    switch (failure) {
      case data_bridge::Failure::ResponseCardinalityError:
        code = Pistache::Http::Code::Bad_Request;
        break;
      default:
        code = Pistache::Http::Code::Internal_Server_Error;
        break;
    }
  }

  return std::make_pair(code, std::move(content));
}

auto PriceSeedController::sync_price_seeds() const -> Result {
  Pistache::Http::Code code{};
  std::string content;

  auto setting = setting_accessor_.get().select_single(ConnectionSetting);
  if (!setting) {
    code = Pistache::Http::Code::Conflict;
    content =
        format_result_response("Price seed database connection in not defined");
    return std::make_pair(code, std::move(content));
  }

  const auto& connection = setting->value();
  if (!connection.has_value()) {
    code = Pistache::Http::Code::Conflict;
    content =
        format_result_response("Price seed database connection has no value");
    return std::make_pair(code, std::move(content));
  }

  log::info("synchronizing price seeds from `{}'", *connection);
  auto result = seed_accessor_.get().sync(*connection);
  if (!result) {
    code = Pistache::Http::Code::Internal_Server_Error;
    content = format_result_response(
        "Failed to synchronize price seeds because of unknown reason");
    return std::make_pair(code, std::move(content));
  }

  log::info("updating the last price seed sync time general setting");
  const auto patch = make_seed_update_time_patch();
  auto update_result = setting_accessor_.get().update(patch, SyncTimeSetting);
  if (!update_result) {
    log::warn(
        "Failed to update last price seeds sync setting in the general "
        "settings table");
  }

  code = Pistache::Http::Code::Ok;
  content = format_result_response("Price seeds successfully synchronized");

  return std::make_pair(code, std::move(content));
}

auto PriceSeedController::format_error_response(data_bridge::Failure failure)
    -> std::string {
  std::string message;
  switch (failure) {
    case data_bridge::Failure::MalformedInput:
      message = "Required fields missing for PriceSeed model in request";
      break;
    case data_bridge::Failure::DatabaseConnectionError:
      message = "Database connection failed";
      break;
    case data_bridge::Failure::ResponseDecodingError:
      message = "Failed to decode a database response";
      break;
    case data_bridge::Failure::ResponseCardinalityError:
      message = "Can not resolve a single PriceSeed by a given key";
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

const std::string PriceSeedController::ConnectionSetting{
    "SeedPriceDatabaseConnection"};

const std::string PriceSeedController::SyncTimeSetting{"SeedPricesLastUpdated"};

}  // namespace simulator::http
