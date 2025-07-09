#include <gmock/gmock.h>

#include "ih/contexts/market_entry_id_context.hpp"
#include "ih/generation.hpp"

namespace simulator::trading_system::idgen::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct IdgenMarketEntryIdGeneration : public Test {
  using Context = MarketEntryIdContext::Implementation;
  using CounterSequence = typename Context::CounterSequence;

  constexpr static auto CounterSequenceMin = CounterSequence::first();
  constexpr static auto CounterSequenceMax = CounterSequence::last();

  static auto make_sequence(std::uint64_t current_value) -> CounterSequence {
    return CounterSequence{current_value};
  }

  static auto make_seed(std::uint64_t value) -> std::uint64_t { return value; }

  template <typename... ContextArgs>
  static auto make_generation_context(ContextArgs&&... args) -> Context {
    return Context{std::forward<ContextArgs>(args)...};
  }
};

TEST_F(IdgenMarketEntryIdGeneration, GeneratesCorrectMarketEntryIdentifier) {
  const auto context =
      make_generation_context(make_seed(123), make_sequence(115));

  const auto identifier = generate_identifier(context);

  ASSERT_THAT(generate_identifier(context), Eq(MarketEntryId{"123:115"}));
}

TEST_F(IdgenMarketEntryIdGeneration, IncrementsCounterSequence) {
  auto context = make_generation_context(make_seed(123), make_sequence(115));

  update_counter_sequence(context);

  EXPECT_THAT(context.counter_sequence().current(), Eq(116));
}

TEST_F(IdgenMarketEntryIdGeneration, ResetsContextWhenCounterSequenceEnds) {
  auto context = make_generation_context(make_seed(125),
                                         make_sequence(CounterSequenceMax));
  update_counter_sequence(context);

  EXPECT_THAT(context.counter_sequence().current(), Eq(CounterSequenceMin));
  EXPECT_THAT(context.seed(), Ne(125));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::idgen::test