#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_ACTIONS_SAVE_COPY_CONSTRUCTIBLE_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_ACTIONS_SAVE_COPY_CONSTRUCTIBLE_HPP_

#include <gmock/gmock.h>

#include <concepts>
#include <memory>

namespace simulator::trading_system::matching_engine::test {

template <std::size_t k, std::copy_constructible T>
struct SaveMoveConstructibleAction {
  std::unique_ptr<T>& pointer_ref;

  template <typename... Args>
  void operator()(const Args&... args) const {
    pointer_ref = std::make_unique<T>(std::get<k>(std::tie(args...)));
  }
};

template <std::size_t k, std::copy_constructible T>
auto save_copy_constructible(std::unique_ptr<T>& pointer)
    -> SaveMoveConstructibleAction<k, T> {
  return {pointer};
}

}

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_ACTIONS_SAVE_COPY_CONSTRUCTIBLE_HPP_