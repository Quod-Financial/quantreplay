#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "common/instrument.hpp"
#include "common/order_modifying_engine.hpp"
#include "common/phase_aware_engine.hpp"
#include "common/quote_publishing_engine.hpp"
#include "common/state_persisting_engine.hpp"
#include "ih/repository/trading_engines_repository.hpp"
#include "mocks/order_modifying_engine_mock.hpp"
#include "mocks/phase_aware_engine_mock.hpp"
#include "mocks/quote_publishing_engine_mock.hpp"
#include "mocks/state_persisting_engine_mock.hpp"
#include "mocks/trading_engine_mock.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct PhaseAwareTradingEngineMock : TradingEngineMock, PhaseAwareEngineMock {
  using PhaseAwareEngineMock::handle;
  using TradingEngineMock::handle;
};

struct OrderModifyingTradingEngineMock : TradingEngineMock,
                                         OrderModifyingEngineMock {
  using OrderModifyingEngineMock::execute;
  using TradingEngineMock::execute;
};

struct StatePersistingTradingEngineMock : TradingEngineMock,
                                          StatePersistingEngineMock {};

struct QuotePublishingTradingEngineMock : TradingEngineMock,
                                          QuotePublishingEngineMock {
  using QuotePublishingEngineMock::execute;
  using TradingEngineMock::execute;
};

struct TradingSystemTradingEnginesRepository : public Test {
  auto add_engine(InstrumentId instrument_id) -> TradingEngine& {
    return add_engine<TradingEngineMock>(instrument_id);
  }

  template <typename EngineType>
  auto add_engine(InstrumentId instrument_id) -> EngineType& {
    return repository.add_engine(instrument_id, std::make_unique<EngineType>());
  }

  auto add_engine(std::nullptr_t) -> TradingEngine& {
    return repository.add_engine(InstrumentId{42},
                                 std::unique_ptr<TradingEngineMock>{});
  }

  TradingEnginesRepository repository;
};

TEST_F(TradingSystemTradingEnginesRepository, IsCreatedEmpty) {
  ASSERT_THAT(repository.size(), Eq(0));
}

TEST_F(TradingSystemTradingEnginesRepository, AddsEngine) {
  add_engine(InstrumentId{42});

  ASSERT_THAT(repository.size(), Eq(1));
}

TEST_F(TradingSystemTradingEnginesRepository, AddsMultipleEngines) {
  add_engine(InstrumentId{1});
  add_engine(InstrumentId{2});
  add_engine(InstrumentId{3});

  ASSERT_THAT(repository.size(), Eq(3));
}

TEST_F(TradingSystemTradingEnginesRepository, RejectsToAddNullEngine) {
  ASSERT_THROW(add_engine(nullptr), std::invalid_argument);
}

TEST_F(TradingSystemTradingEnginesRepository,
       RejectsToAddEngineWithExistingInstrumentId) {
  add_engine(InstrumentId{42});
  ASSERT_THAT(repository.size(), Eq(1));

  ASSERT_THROW(add_engine(InstrumentId{42}), std::invalid_argument);
  ASSERT_THAT(repository.size(), Eq(1));
}

TEST_F(TradingSystemTradingEnginesRepository, ProvidesEngineByInstrumentId) {
  auto& engine = add_engine(InstrumentId{1});

  ASSERT_THAT(repository.find_instrument_engine(InstrumentId{1}), Ref(engine));
}

TEST_F(TradingSystemTradingEnginesRepository, ResolvesEngineByInstrumentId) {
  add_engine(InstrumentId{1});
  auto& engine2 = add_engine(InstrumentId{2});
  add_engine(InstrumentId{3});

  ASSERT_THAT(repository.find_instrument_engine(InstrumentId{2}), Ref(engine2));
}

TEST_F(TradingSystemTradingEnginesRepository,
       ThrowsErrorWhenEngineIsNotFoundByInstrumentId) {
  add_engine(InstrumentId{1});
  add_engine(InstrumentId{2});
  add_engine(InstrumentId{3});

  ASSERT_THROW(repository.find_instrument_engine(InstrumentId{4}),
               std::out_of_range);
}

TEST_F(TradingSystemTradingEnginesRepository, IteratesOverAllEngines) {
  auto& engine1 = add_engine(InstrumentId{1});
  auto& engine2 = add_engine(InstrumentId{2});
  auto& engine3 = add_engine(InstrumentId{3});

  std::vector<const TradingEngine*> engines_pointers;
  repository.for_each_engine([&](const TradingEngine& engine) {
    engines_pointers.push_back(&engine);
  });

  ASSERT_THAT(engines_pointers,
              UnorderedElementsAre(&engine1, &engine2, &engine3));
}

TEST_F(TradingSystemTradingEnginesRepository,
       IteratesOverPhaseAwareEnginesOnly) {
  auto phase_aware_engine = std::make_unique<PhaseAwareTradingEngineMock>();
  const PhaseAwareEngine* const expected_engine = phase_aware_engine.get();
  add_engine(InstrumentId{1});
  repository.add_engine(InstrumentId{2}, std::move(phase_aware_engine));

  std::vector<const PhaseAwareEngine*> engines_pointers;
  repository.for_each_phase_aware_engine([&](const PhaseAwareEngine& engine) {
    engines_pointers.push_back(&engine);
  });

  ASSERT_THAT(engines_pointers, ElementsAre(expected_engine));
}

TEST_F(TradingSystemTradingEnginesRepository,
       ProvidesNoOrderModifyingEngineForEngineThatCannotModifyOrders) {
  add_engine(InstrumentId{1});

  ASSERT_THAT(repository.find_order_modifying_engine(InstrumentId{1}),
              IsNull());
}

TEST_F(TradingSystemTradingEnginesRepository,
       ProvidesOrderModifyingEngineByInstrumentId) {
  add_engine(InstrumentId{1});
  const OrderModifyingEngine& engine =
      add_engine<OrderModifyingTradingEngineMock>(InstrumentId{2});

  ASSERT_THAT(repository.find_order_modifying_engine(InstrumentId{2}),
              Eq(&engine));
}

TEST_F(TradingSystemTradingEnginesRepository,
       ProvidesNoStatePersistingEngineForEngineThatPersistsNoState) {
  add_engine(InstrumentId{1});

  ASSERT_THAT(repository.find_state_persisting_engine(InstrumentId{1}),
              IsNull());
}

TEST_F(TradingSystemTradingEnginesRepository,
       ProvidesStatePersistingEngineByInstrumentId) {
  add_engine(InstrumentId{1});
  const StatePersistingEngine& engine =
      add_engine<StatePersistingTradingEngineMock>(InstrumentId{2});

  ASSERT_THAT(repository.find_state_persisting_engine(InstrumentId{2}),
              Eq(&engine));
}

TEST_F(TradingSystemTradingEnginesRepository,
       ProvidesNoQuotePublishingEngineForEngineThatCannotPublishQuotes) {
  add_engine(InstrumentId{1});

  ASSERT_THAT(repository.find_quote_publishing_engine(InstrumentId{1}),
              IsNull());
}

TEST_F(TradingSystemTradingEnginesRepository,
       ProvidesQuotePublishingEngineByInstrumentId) {
  add_engine(InstrumentId{1});
  const QuotePublishingEngine& engine =
      add_engine<QuotePublishingTradingEngineMock>(InstrumentId{2});

  ASSERT_THAT(repository.find_quote_publishing_engine(InstrumentId{2}),
              Eq(&engine));
}

TEST_F(TradingSystemTradingEnginesRepository,
       TriesEveryQuotePublishingEngineWhileNoneHandlesAction) {
  add_engine(InstrumentId{1});
  const QuotePublishingEngine& first_engine =
      add_engine<QuotePublishingTradingEngineMock>(InstrumentId{2});
  const QuotePublishingEngine& second_engine =
      add_engine<QuotePublishingTradingEngineMock>(InstrumentId{3});
  std::vector<const QuotePublishingEngine*> tried;

  std::ignore = repository.try_quote_publishing_until_handled(
      [&tried](QuotePublishingEngine& engine) {
        tried.push_back(&engine);
        return false;
      });

  ASSERT_THAT(tried, UnorderedElementsAre(&first_engine, &second_engine));
}

TEST_F(TradingSystemTradingEnginesRepository,
       StopsTryingQuotePublishingEnginesOnceOneHandlesAction) {
  add_engine<QuotePublishingTradingEngineMock>(InstrumentId{1});
  add_engine<QuotePublishingTradingEngineMock>(InstrumentId{2});
  std::vector<const QuotePublishingEngine*> tried;

  std::ignore = repository.try_quote_publishing_until_handled(
      [&tried](QuotePublishingEngine& engine) {
        tried.push_back(&engine);
        return true;
      });

  ASSERT_THAT(tried, SizeIs(1));
}

TEST_F(TradingSystemTradingEnginesRepository,
       ReportsActionUnhandledByQuotePublishingEngines) {
  add_engine(InstrumentId{1});
  add_engine<QuotePublishingTradingEngineMock>(InstrumentId{2});

  ASSERT_FALSE(repository.try_quote_publishing_until_handled(
      [](QuotePublishingEngine& /*engine*/) { return false; }));
}

TEST_F(TradingSystemTradingEnginesRepository,
       ReportsActionHandledByQuotePublishingEngine) {
  add_engine<QuotePublishingTradingEngineMock>(InstrumentId{1});

  ASSERT_TRUE(repository.try_quote_publishing_until_handled(
      [](QuotePublishingEngine& /*engine*/) { return true; }));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::test