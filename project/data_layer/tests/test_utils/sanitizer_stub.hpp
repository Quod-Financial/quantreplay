#ifndef SIMULATOR_DATA_LAYER_TESTS_TEST_UTILS_SANITIZER_STUB_HPP_
#define SIMULATOR_DATA_LAYER_TESTS_TEST_UTILS_SANITIZER_STUB_HPP_

#include <fmt/format.h>

#include <string>

namespace simulator::data_layer {

struct SanitizerStub {
  template <typename T>
  auto operator()(T&& value) -> std::string {
    return fmt::format("`{}`", std::forward<T>(value));
  }
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_TESTS_TEST_UTILS_SANITIZER_STUB_HPP_
