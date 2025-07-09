#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/contexts/instrument_id_context.hpp"
#include "ih/generation.hpp"

namespace simulator::trading_system::idgen::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct IdgenInstrumentIdGeneration : public Test {
  using Context = InstrumentIdContext::Implementation;
  using Sequence = typename Context::SequenceType;
  using Error = typename Context::SequenceError;
  constexpr inline static auto SequenceMax =
      std::numeric_limits<std::uint64_t>::max();

  static auto make_generation_context(std::uint64_t sequence_value) -> Context {
    return Context{Sequence{sequence_value}};
  }
};

TEST_F(IdgenInstrumentIdGeneration, GeneratesIdentifier) {
  const auto context = make_generation_context(113);

  const auto identifier = generate_identifier(context);

  ASSERT_THAT(identifier.has_value(), IsTrue());
  ASSERT_THAT(identifier.value(), Eq(InstrumentId{113}));
}

TEST_F(IdgenInstrumentIdGeneration, FailsToGenerateFromFinishedSequence) {
  auto context = make_generation_context(SequenceMax);
  context.set_sequence_error(Error::Overflow);

  const auto identifier = generate_identifier(context);

  ASSERT_THAT(identifier.has_value(), IsFalse());
  ASSERT_THAT(identifier.error(), Eq(GenerationError::CollisionDetected));
}

TEST_F(IdgenInstrumentIdGeneration, UpdatesCounterSequence) {
  auto context = make_generation_context(12);

  update_counter_sequence(context);

  ASSERT_THAT(context.counter_sequence().has_value(), IsTrue());
  ASSERT_THAT(context.counter_sequence()->current(), Eq(13));
}

TEST_F(IdgenInstrumentIdGeneration, SetsErrorInContextWhenSequenceEnded) {
  auto context = make_generation_context(SequenceMax);

  update_counter_sequence(context);

  ASSERT_THAT(context.counter_sequence().has_value(), IsFalse());
  ASSERT_THAT(context.counter_sequence().error(), Eq(Error::Overflow));
}

TEST_F(IdgenInstrumentIdGeneration, KeepsFinishedSequenceState) {
  auto context = make_generation_context(SequenceMax);
  context.set_sequence_error(Error::Overflow);

  update_counter_sequence(context);

  ASSERT_THAT(context.counter_sequence().has_value(), IsFalse());
  ASSERT_THAT(context.counter_sequence().error(), Eq(Error::Overflow));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::idgen::test