#include <gmock/gmock.h>

#include <limits>

#include "common/attributes.hpp"
#include "core/domain/attributes.hpp"
#include "ih/contexts/execution_id_context.hpp"
#include "ih/generation.hpp"
#include "ih/sequences/numeric_sequence.hpp"

namespace simulator::trading_system::idgen::test {
namespace {

using namespace ::testing;  // NOLINT

struct IdgenExecutionIdGeneration : public Test {
  using Context = ExecutionIdContext::Implementation;
  using CounterSequence = typename Context::CounterSequence;
  constexpr inline static auto CounterSequenceMax =
      std::numeric_limits<std::uint64_t>::max();

  static auto make_sequence(std::uint64_t current_value) -> CounterSequence {
    return CounterSequence{current_value};
  }

  static auto make_order_id(std::uint64_t value) -> OrderId {
    return OrderId{value};
  }

  template <typename... ContextArgs>
  static auto make_generation_context(ContextArgs&&... args) -> Context {
    return Context{std::forward<ContextArgs>(args)...};
  }
};

// NOLINTBEGIN(*-magic-numbers)

TEST_F(IdgenExecutionIdGeneration, GeneratesCorrectExecutionIdentifier) {
  const auto context = make_generation_context(
      make_order_id(240205103011000042), make_sequence(115));

  const auto identifier = generate_identifier(context);

  ASSERT_THAT(identifier.has_value(), IsTrue());
  ASSERT_THAT(identifier->value(), Eq("240205103011000042-115"));
}

TEST_F(IdgenExecutionIdGeneration, FailsToGenerateFromFinishedSequence) {
  auto context = make_generation_context(make_order_id(123), make_sequence(1));
  context.set_counter_sequence_error(numeric::SequenceError::Overflow);

  const auto identifier = generate_identifier(context);

  ASSERT_THAT(identifier.has_value(), IsFalse());
  ASSERT_THAT(identifier.error(), Eq(GenerationError::CollisionDetected));
}

TEST_F(IdgenExecutionIdGeneration, UpdatesCounterSequence) {
  auto context = make_generation_context(make_order_id(123), make_sequence(1));

  update_counter_sequence(context);

  ASSERT_THAT(context.counter_sequence().has_value(), IsTrue());
  ASSERT_THAT(context.counter_sequence()->current(), Eq(2));
}

TEST_F(IdgenExecutionIdGeneration, SetsErrorInContextWhenCounterSequenceEnded) {
  auto context = make_generation_context(make_order_id(123),
                                         make_sequence(CounterSequenceMax));

  update_counter_sequence(context);

  ASSERT_THAT(context.counter_sequence().has_value(), IsFalse());
  ASSERT_THAT(context.counter_sequence().error(),
              Eq(numeric::SequenceError::Overflow));
}

TEST_F(IdgenExecutionIdGeneration, KeepsFinishedSequenceState) {
  auto context = make_generation_context(make_order_id(123), make_sequence(1));
  context.set_counter_sequence_error(numeric::SequenceError::Overflow);

  update_counter_sequence(context);

  ASSERT_THAT(context.counter_sequence().has_value(), IsFalse());
  ASSERT_THAT(context.counter_sequence().error(),
              Eq(numeric::SequenceError::Overflow));
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::trading_system::idgen::test