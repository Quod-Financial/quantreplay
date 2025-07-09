#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "common/instrument.hpp"
#include "ih/repository/trading_engines_repository.hpp"
#include "mocks/trading_engine_mock.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace testing;  // NOLINT

struct TradingSystemTradingEnginesRepository : public Test {
  auto add_engine(InstrumentId instrument_id) -> TradingEngine& {
    return repository.add_engine(instrument_id,
                                 std::make_unique<TradingEngineMock>());
  }

  auto add_engine(std::nullptr_t) -> TradingEngine& {
    return repository.add_engine(InstrumentId{42}, nullptr);
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

}  // namespace
}  // namespace simulator::trading_system::test