#include "ih/controllers/setting_controller.hpp"

#include <fmt/format.h>

#include <string>
#include <utility>

#include "data_layer/api/models/setting.hpp"
#include "ih/marshalling/json/setting.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"

namespace simulator::http {
namespace {

[[nodiscard]]
auto unmarshall_request_body(std::string_view body,
                             std::vector<data_layer::Setting::Patch>& dest,
                             Pistache::Http::Code& code,
                             std::string& content) noexcept -> bool {
  try {
    json::SettingUnmarshaller::unmarshall(body, dest);
    return true;
  } catch (const std::runtime_error& ex) {
    code = Pistache::Http::Code::Bad_Request;
    content = format_result_response(fmt::format("Bad request: {}", ex.what()));
  } catch (...) {
    log::err("failed to unmarshall settings, unexpected error raised");
    code = Pistache::Http::Code::Internal_Server_Error;
    content = format_result_response("Failed to unmarshall request body");
  }

  return false;
}

}  // namespace

SettingController::SettingController(
    data_bridge::SettingAccessor& data_accessor) noexcept
    : settings_accessor_(data_accessor) {}

auto SettingController::select_all_settings() const -> Result {
  Pistache::Http::Code code{};
  std::string content;

  auto result = setting_accessor().select_all();
  if (result) {
    try {
      const std::vector<data_layer::Setting>& settings = result.value();
      content = json::SettingMarshaller::marshall(settings);
      code = Pistache::Http::Code::Ok;
      log::info("selected {} setting records", settings.size());
    } catch (const std::exception& ex) {
      log::warn("failed to marshall settings list to JSON: {}", ex.what());
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

auto SettingController::update_settings(const std::string& body) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content;

  std::vector<data_layer::Setting::Patch> settings;
  if (!unmarshall_request_body(body, settings, code, content)) {
    return std::make_pair(code, std::move(content));
  }

  code = Pistache::Http::Code::Ok;
  content = format_result_response("General settings updated successfully");

  for (const auto& setting : settings) {
    const std::optional<std::string>& key = setting.key();
    const std::optional<std::string>& value = setting.value();
    if (!key.has_value() || !value.has_value() || is_readonly_setting(*key)) {
      continue;
    }

    if (!is_updatable_setting(*key)) {
      code = Pistache::Http::Code::Bad_Request;
      content = format_result_response(
          fmt::format("Unknown `{}' setting update requested", *key));
      break;
    }

    const auto result = setting_accessor().update(setting, *key);
    if (!result) {
      code = Pistache::Http::Code::Internal_Server_Error;
      content = format_error_response(result.error());
      break;
    }
  }

  return std::make_pair(code, std::move(content));
}

auto SettingController::setting_accessor() const noexcept
    -> const data_bridge::SettingAccessor& {
  return settings_accessor_.get();
}

auto SettingController::format_error_response(data_bridge::Failure failure)
    -> std::string {
  std::string message;
  switch (failure) {
    case data_bridge::Failure::MalformedInput:
      message = "Required fields missing for Setting model in request";
      break;
    case data_bridge::Failure::DatabaseConnectionError:
      message = "Database connection failed";
      break;
    case data_bridge::Failure::ResponseDecodingError:
      message = "Failed to decode a database response";
      break;
    case data_bridge::Failure::ResponseCardinalityError:
      message = "Can not resolve a single Setting by a given key";
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

auto SettingController::is_updatable_setting(std::string_view setting_key)
    -> bool {
  return setting_key == DisplayNameSetting ||
         setting_key == ExternalPriceSeedConnectionSetting;
}

auto SettingController::is_readonly_setting(std::string_view setting_key)
    -> bool {
  return setting_key == PriceSeedsLastUpdateTimeSetting;
}

const std::string SettingController::DisplayNameSetting{"DisplayName"};

const std::string SettingController::ExternalPriceSeedConnectionSetting{
    "SeedPriceDatabaseConnection"};

const std::string SettingController::PriceSeedsLastUpdateTimeSetting{
    "SeedPricesLastUpdated"};

}  // namespace simulator::http
