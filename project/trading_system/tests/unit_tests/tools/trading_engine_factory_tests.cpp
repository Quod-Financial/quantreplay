#include <gmock/gmock.h>

#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>

#include "common/instrument.hpp"
#include "common/phase_aware_engine.hpp"
#include "common/quote_publishing_engine.hpp"
#include "common/trading_engine.hpp"
#include "ih/config/config.hpp"
#include "ih/repository/trading_engines_repository.hpp"
#include "ih/tools/trading_engine_factory.hpp"
#include "matching_engine/matching_engine.hpp"
#include "mocks/service_mock.hpp"
#include "protocol/admin/quote.hpp"
#include "quoting_engine/quoting_engine.hpp"
#include "test_utils/utils.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace ::testing;  // NOLINT

constexpr auto UnknownEngineType = invalid_enum_value<EngineType>();

struct TradingSystemTradingEngineFactory : public Test {
  auto create_engine(EngineType engine_type) -> TradingEngine& {
    return create_engine(engine_type, InstrumentId{1});
  }

  auto create_engine(EngineType engine_type, InstrumentId identifier)
      -> TradingEngine& {
    Instrument instrument;
    instrument.identifier = identifier;
    instrument.engine_type = engine_type;
    factory->create_trading_engine(instrument, repository);
    return repository.find_instrument_engine(identifier);
  }

  [[nodiscard]]
  auto count_phase_aware_engines() const -> std::size_t {
    std::size_t count = 0;
    repository.for_each_phase_aware_engine(
        [&count](PhaseAwareEngine& /*engine*/) { ++count; });
    return count;
  }

  Config config;
  NiceMock<ServiceMock> executor;
  TradingEnginesRepository repository;
  std::unique_ptr<TradingEngineFactory> factory =
      create_trading_engine_factory(config, executor);
};

TEST_F(TradingSystemTradingEngineFactory,
       ThrowsForUnknownEngineTypeWithoutRegisteringEngine) {
  ASSERT_THROW(create_engine(UnknownEngineType), std::invalid_argument);
  ASSERT_THAT(repository.size(), Eq(0));
}

TEST_F(TradingSystemTradingEngineFactory,
       CreatesMatchingEngineForMatchingListing) {
  auto& engine = create_engine(EngineType::Matching);

  ASSERT_THAT(dynamic_cast<matching_engine::MatchingEngine*>(&engine),
              NotNull());
}

TEST_F(TradingSystemTradingEngineFactory,
       RegistersMatchingEngineAsPhaseAwareEngine) {
  create_engine(EngineType::Matching);

  ASSERT_THAT(count_phase_aware_engines(), Eq(1));
}

TEST_F(TradingSystemTradingEngineFactory,
       RegistersMatchingEngineAsOrderModifyingEngine) {
  create_engine(EngineType::Matching, InstrumentId{1});

  ASSERT_THAT(repository.find_order_modifying_engine(InstrumentId{1}),
              NotNull());
}

TEST_F(TradingSystemTradingEngineFactory,
       RegistersMatchingEngineAsStatePersistingEngine) {
  create_engine(EngineType::Matching, InstrumentId{1});

  ASSERT_THAT(repository.find_state_persisting_engine(InstrumentId{1}),
              NotNull());
}

TEST_F(TradingSystemTradingEngineFactory,
       DoesNotRegisterMatchingEngineAsQuotePublishingEngine) {
  create_engine(EngineType::Matching, InstrumentId{1});

  ASSERT_THAT(repository.find_quote_publishing_engine(InstrumentId{1}),
              IsNull());
}

TEST_F(TradingSystemTradingEngineFactory,
       CreatesQuotingEngineForQuotingListing) {
  auto& engine = create_engine(EngineType::Quoting);

  ASSERT_THAT(dynamic_cast<quoting_engine::QuotingEngine*>(&engine), NotNull());
}

TEST_F(TradingSystemTradingEngineFactory,
       CreatesEngineReceivingNoPhaseEventsForQuotingListing) {
  create_engine(EngineType::Quoting);

  ASSERT_THAT(count_phase_aware_engines(), Eq(0));
}

TEST_F(TradingSystemTradingEngineFactory,
       DoesNotRegisterQuotingEngineAsOrderModifyingEngine) {
  create_engine(EngineType::Quoting, InstrumentId{1});

  ASSERT_THAT(repository.find_order_modifying_engine(InstrumentId{1}),
              IsNull());
}

TEST_F(TradingSystemTradingEngineFactory,
       DoesNotRegisterQuotingEngineAsStatePersistingEngine) {
  create_engine(EngineType::Quoting, InstrumentId{1});

  ASSERT_THAT(repository.find_state_persisting_engine(InstrumentId{1}),
              IsNull());
}

TEST_F(TradingSystemTradingEngineFactory,
       RegistersQuotingEngineAsQuotePublishingEngine) {
  create_engine(EngineType::Quoting, InstrumentId{1});

  ASSERT_THAT(repository.find_quote_publishing_engine(InstrumentId{1}),
              NotNull());
}

struct TradingSystemTradingEngineFactoryQuoting
    : public TradingSystemTradingEngineFactory {
  TradingSystemTradingEngineFactoryQuoting() {
    ON_CALL(executor, execute)
        .WillByDefault([](const std::function<void()>& task) { task(); });
  }

  [[nodiscard]]
  auto publish_quote(InstrumentId identifier) const -> std::optional<QuoteId> {
    protocol::QuotePublicationRequest request;
    request.bids.push_back(
        protocol::QuoteEntry{.price = Price{1.0}, .quantity = Quantity{1.0}});
    protocol::QuotePublicationReply reply;
    repository.find_quote_publishing_engine(identifier)
        ->execute(request, reply);
    return reply.quote_id;
  }
};

TEST_F(TradingSystemTradingEngineFactoryQuoting,
       CreatesQuotingEnginesThatNeverAssignSameQuoteId) {
  create_engine(EngineType::Quoting, InstrumentId{1});
  create_engine(EngineType::Quoting, InstrumentId{2});

  const auto first_quote_id = publish_quote(InstrumentId{1});
  const auto second_quote_id = publish_quote(InstrumentId{2});

  ASSERT_THAT(second_quote_id, Ne(std::nullopt));
  ASSERT_THAT(first_quote_id, AllOf(Ne(std::nullopt), Ne(second_quote_id)));
}

}  // namespace
}  // namespace simulator::trading_system::test
