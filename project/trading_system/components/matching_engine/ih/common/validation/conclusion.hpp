#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_VALIDATION_CONCLUSION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_VALIDATION_CONCLUSION_HPP_

#include <string_view>

namespace simulator::trading_system::matching_engine {

class Conclusion {
 public:
  [[nodiscard]]
  auto failed() const -> bool {
    return failed_;
  }

  [[nodiscard]]
  auto error() const -> std::string_view {
    return error_;
  }

  [[nodiscard]]
  static auto success() -> Conclusion {
    return {};
  };

  [[nodiscard]]
  static auto failure(std::string_view reason) -> Conclusion {
    Conclusion conclusion;
    conclusion.failed_ = true;
    conclusion.error_ = reason;
    return conclusion;
  }

 private:
  std::string_view error_;
  bool failed_ = false;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_VALIDATION_CONCLUSION_HPP_
