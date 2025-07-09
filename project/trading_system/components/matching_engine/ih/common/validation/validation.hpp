#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_VALIDATION_VALIDATION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_VALIDATION_VALIDATION_HPP_

#include <functional>
#include <memory>
#include <optional>
#include <string_view>

namespace simulator::trading_system::matching_engine {

template <typename T, typename Error>
class Validation {
 public:
  using ErrorDescriber = std::function<std::string_view(Error)>;

  explicit Validation(const T& request, ErrorDescriber error_describer)
      : request_{std::addressof(request)} {
    if (!error_describer) [[unlikely]] {
      throw std::invalid_argument("Error describer cannot be null");
    }
    error_describer_ = std::move(error_describer);
  }

  template <typename Checker>
  auto expect(Checker&& checker) -> Validation& {
    if (successful()) {
      assign(std::invoke(std::forward<Checker>(checker), request()));
    }
    return *this;
  }

  [[nodiscard]]
  auto request() const -> const T& {
    return *request_;
  }

  [[nodiscard]]
  auto successful() const -> bool {
    return !error_.has_value();
  }

  [[nodiscard]]
  auto error() const -> std::optional<Error> {
    return error_;
  }

  [[nodiscard]]
  auto error_description() const -> std::string_view {
    if (error_.has_value()) {
      return error_describer_(*error_);
    }
    return {};
  }

 private:
  auto assign(std::optional<Error> err) -> void { error_ = err; }

  const T* request_ = nullptr;
  std::optional<Error> error_ = std::nullopt;
  ErrorDescriber error_describer_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_VALIDATION_VALIDATION_HPP_
