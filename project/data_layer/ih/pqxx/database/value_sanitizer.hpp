#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_VALUE_SANITIZER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_VALUE_SANITIZER_HPP_

#include <functional>
#include <pqxx/connection>

namespace simulator::data_layer::internal_pqxx {

class ValueSanitizer {
 public:
  explicit ValueSanitizer(const pqxx::connection& connection) noexcept
      : connection_(connection) {}

  template <typename T>
  auto operator()(const T& value) -> std::string {
    return connection_.get().quote(value);
  }

 private:
  std::reference_wrapper<const pqxx::connection> connection_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_VALUE_SANITIZER_HPP_
