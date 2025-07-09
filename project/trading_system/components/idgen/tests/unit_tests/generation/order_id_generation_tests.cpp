#include <gmock/gmock.h>

#include <chrono>

#include "ih/contexts/order_id_context.hpp"
#include "ih/generation.hpp"
#include "ih/sequences/chrono_sequence.hpp"
#include "ih/sequences/numeric_sequence.hpp"

namespace simulator::trading_system::idgen::test {
namespace {

using namespace std::chrono_literals;
using namespace ::testing;  // NOLINT

struct IdgenOrderIdGeneration : public Test {
  using TimeSequence = OrderIdContext::Implementation::TimeSequence;
  using CounterSequence = OrderIdContext::Implementation::CounterSequence;
  constexpr inline static std::uint32_t CounterSequenceMax = 999'999;

  static auto make_counter_sequence(std::uint32_t current_counter)
      -> CounterSequence {
    return CounterSequence{current_counter};
  }

  static auto make_time_sequence(std::chrono::seconds current_time)
      -> TimeSequence {
    if (auto sequence = make_sequence<TimeSequence>(current_time)) {
      return *sequence;
    }
    throw std::runtime_error{"Failed to create time sequence"};
  }

  template <typename... ContextArgs>
  static auto make_generation_context(ContextArgs&&... args)
      -> OrderIdContext::Implementation {
    return OrderIdContext::Implementation{std::forward<ContextArgs>(args)...};
  }

  std::chrono::seconds testing_time = std::chrono::floor<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch());
};

TEST_F(IdgenOrderIdGeneration, GeneratesCorrectOrderIdentifier) {
  // 2024-02-05 10:30:11 GMT
  constexpr auto gen_time = 1707129011s;
  constexpr std::uint32_t gen_counter = 42;

  auto context = make_generation_context(make_time_sequence(gen_time),
                                         make_counter_sequence(gen_counter));
  const auto identifier = generate_identifier(context);

  ASSERT_THAT(identifier.value(), Eq(240205103011000042));
}

TEST_F(IdgenOrderIdGeneration, UpdatesTimeSequence) {
  const auto gen_time = testing_time - 1s;
  auto context = make_generation_context(make_time_sequence(gen_time),
                                         make_counter_sequence(0));

  update_time_sequence(context);

  ASSERT_THAT(context.time_sequence().tick(), Ge(testing_time));
}

TEST_F(IdgenOrderIdGeneration, UpdatesCounterSequence) {
  auto context = make_generation_context(make_time_sequence(testing_time),
                                         make_counter_sequence(0));

  update_counter_sequence(context);

  ASSERT_THAT(context.counter_sequence().current(), Eq(1));
}

TEST_F(IdgenOrderIdGeneration, ResetsCounterSequenceAfterLastValueReached) {
  auto context =
      make_generation_context(make_time_sequence(testing_time),
                              make_counter_sequence(CounterSequenceMax));

  update_counter_sequence(context);

  ASSERT_THAT(context.counter_sequence().current(), Eq(0));
}

}  // namespace
}  // namespace simulator::trading_system::idgen::test