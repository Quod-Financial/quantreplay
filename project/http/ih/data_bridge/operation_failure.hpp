#ifndef SIMULATOR_HTTP_IH_DATA_BRIDGE_OPERATION_FAILURE_HPP_
#define SIMULATOR_HTTP_IH_DATA_BRIDGE_OPERATION_FAILURE_HPP_

namespace simulator::http::data_bridge {

enum class Failure : std::uint8_t {
  MalformedInput,
  DatabaseConnectionError,
  ResponseDecodingError,
  ResponseCardinalityError,
  DataIntegrityViolationError,
  UnknownError
};

struct FailureInfo {
  Failure failure;
  std::string message;
};

}  // namespace simulator::http::data_bridge

#endif  // SIMULATOR_HTTP_IH_DATA_BRIDGE_OPERATION_FAILURE_HPP_
