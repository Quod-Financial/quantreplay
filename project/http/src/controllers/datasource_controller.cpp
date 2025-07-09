#include "ih/controllers/datasource_controller.hpp"

#include <fmt/format.h>

#include <string>
#include <utility>

#include "data_layer/api/models/datasource.hpp"
#include "ih/marshalling/json/datasource.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"

namespace simulator::http {

namespace {

[[nodiscard]]
auto unmarshall_request_body(std::string_view body,
                             data_layer::Datasource::Patch& dest,
                             Pistache::Http::Code& code,
                             std::string& content) noexcept -> bool {
  try {
    json::DatasourceUnmarshaller::unmarshall(body, dest);
    return true;
  } catch (const std::runtime_error& ex) {
    code = Pistache::Http::Code::Bad_Request;
    content = format_result_response(fmt::format("Bad request: {}", ex.what()));
  } catch (...) {
    log::err("ailed to unmarshall datasource, unexpected error raised");
    code = Pistache::Http::Code::Internal_Server_Error;
    content = format_result_response("Failed to unmarshall request body");
  }

  return false;
}

}  // namespace

DatasourceController::DatasourceController(
    data_bridge::DatasourceAccessor& data_accessor) noexcept
    : data_accessor_(data_accessor) {}

auto DatasourceController::select_datasource(std::uint64_t datasource_id) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content;

  auto result = data_accessor_.get().select_single(datasource_id);
  if (result) {
    const data_layer::Datasource& selected = result.value();
    try {
      content = json::DatasourceMarshaller::marshall(selected);
      code = Pistache::Http::Code::Ok;
    } catch (const std::exception& ex) {
      log::warn("failed to marshall datasource to JSON: {}", ex.what());
      code = Pistache::Http::Code::Internal_Server_Error;
      content = format_result_response("failed to marshall datasource");
    }
  } else {
    const auto failure_info = result.error();
    content = format_error_response(failure_info);
    switch (failure_info.failure) {
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

auto DatasourceController::select_all_datasources() const -> Result {
  Pistache::Http::Code code{};
  std::string content;

  auto result = data_accessor_.get().select_all();
  if (!result) {
    content = format_error_response(result.error());
    code = Pistache::Http::Code::Internal_Server_Error;
    return std::make_pair(code, std::move(content));
  }

  const std::vector<data_layer::Datasource>& selected = result.value();
  try {
    content = json::DatasourceMarshaller::marshall(selected);
    code = Pistache::Http::Code::Ok;
    log::info("selected {} datasource records", selected.size());
  } catch (const std::exception& ex) {
    log::warn("failed to marshall datasources to JSON: {}", ex.what());
    code = Pistache::Http::Code::Internal_Server_Error;
    content = format_result_response("failed to marshall datasources");
  }

  return std::make_pair(code, std::move(content));
}

auto DatasourceController::insert_datasource(const std::string& body) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content{};

  data_layer::Datasource::Patch datasource{};
  if (!unmarshall_request_body(body, datasource, code, content)) {
    return std::make_pair(code, std::move(content));
  }

  auto result = data_accessor_.get().add(datasource);
  if (result) {
    log::info("successfully added a new venue");
    code = Pistache::Http::Code::Created;
    content = format_result_response("Successfully inserted a new datasource");
    return std::make_pair(code, std::move(content));
  }

  const auto failure_info = result.error();
  content = format_error_response(failure_info);
  switch (failure_info.failure) {
    case data_bridge::Failure::MalformedInput:
    case data_bridge::Failure::DataIntegrityViolationError:
      code = Pistache::Http::Code::Bad_Request;
      break;
    default:
      code = Pistache::Http::Code::Internal_Server_Error;
      break;
  }
  return std::make_pair(code, std::move(content));
}

auto DatasourceController::update_datasource(std::uint64_t datasource_id,
                                             const std::string& body) const
    -> Result {
  Pistache::Http::Code code{};
  std::string content{};

  data_layer::Datasource::Patch patch{};
  if (!unmarshall_request_body(body, patch, code, content)) {
    return std::make_pair(code, std::move(content));
  }

  auto result = data_accessor_.get().update(patch, datasource_id);
  if (result) {
    code = Pistache::Http::Code::Ok;
    content = format_result_response(
        fmt::format("Successfully updated the datasource with `{}' identifier",
                    datasource_id));

    log::info("successfully updated the datasource with `{}' identifier",
              datasource_id);
  } else {
    const auto failure_info = result.error();
    content = format_error_response(failure_info);
    switch (failure_info.failure) {
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

auto DatasourceController::format_error_response(
    data_bridge::FailureInfo failure_info) -> std::string {
  std::string message;
  switch (failure_info.failure) {
    case data_bridge::Failure::MalformedInput:
      message = failure_info.message.empty()
                    ? "Required fields missing for datasource model in request"
                    : failure_info.message;
      break;
    case data_bridge::Failure::DatabaseConnectionError:
      message = "Database connection failed";
      break;
    case data_bridge::Failure::ResponseDecodingError:
      message = "Failed to decode a database response";
      break;
    case data_bridge::Failure::ResponseCardinalityError:
      message = "Can not resolve a single datasource by a given key";
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
