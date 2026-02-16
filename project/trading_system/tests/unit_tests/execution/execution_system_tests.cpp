#include <gmock/gmock.h>

#include <tl/expected.hpp>

#include "common/instrument_state.hpp"
#include "ih/execution/execution_system.hpp"
#include "instruments/lookup_error.hpp"
#include "instruments/view.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "mocks/instrument_resolver_mock.hpp"
#include "mocks/repository_accessor_mock.hpp"
#include "mocks/trading_reply_receiver_mock.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::test {
namespace {

// NOLINTBEGIN(*magic-numbers*)

using namespace testing;  // NOLINT

struct TradingSystemExecutionSystem : public Test {
  Instrument instrument;
  InstrumentDescriptor requested_instrument;

  NiceMock<RepositoryAccessorMock> repository_accessor;
  NiceMock<InstrumentResolverMock> instrument_resolver;
  NiceMock<TradingReplyReceiverMock> trading_reply_receiver;

  ExecutionSystem execution_system{instrument_resolver, repository_accessor};

  static auto make_session() -> protocol::Session {
    return protocol::Session{protocol::generator::Session{}};
  }

  template <typename RequestType>
  static auto make_external_request() -> RequestType {
    return RequestType{make_session()};
  }

  static auto make_store_vector(
      const std::vector<Instrument>& instruments,
      std::vector<market_state::InstrumentState>& state)
      -> std::vector<std::pair<InstrumentId, market_state::InstrumentState&>> {
    std::vector<std::pair<InstrumentId, market_state::InstrumentState&>>
        store_vector;
    store_vector.reserve(instruments.size());
    for (std::size_t i = 0; i < instruments.size(); ++i) {
      store_vector.emplace_back(instruments[i].identifier, state[i]);
    }
    return store_vector;
  }

 private:
  auto SetUp() -> void override {
    instrument.identifier = InstrumentId{42};
    requested_instrument.symbol = Symbol{"AAPL"};

    // Bind trading reply receiver mock to middleware
    middleware::bind_trading_reply_channel(
        std::shared_ptr<TradingReplyReceiverMock>{
            std::addressof(trading_reply_receiver), [](auto* /*pointer*/) {}});

    // Resolve test instrument by default
    ON_CALL(instrument_resolver,
            resolve_instrument(A<const InstrumentDescriptor&>()))
        .WillByDefault(Return(instrument::View{instrument}));
  }

  auto TearDown() -> void override {
    middleware::release_trading_reply_channel();
  }
};

TEST_F(TradingSystemExecutionSystem, PerformsInstrumentResolution) {
  auto request = make_external_request<protocol::OrderPlacementRequest>();

  EXPECT_CALL(instrument_resolver,
              resolve_instrument(A<const InstrumentDescriptor&>()));

  execution_system.execute_request(request);
}

TEST_F(TradingSystemExecutionSystem, DispatchesOperationToCorrectExecutor) {
  const auto request = make_external_request<protocol::OrderPlacementRequest>();

  EXPECT_CALL(repository_accessor, unicast_impl(Eq(instrument.identifier), _));
  execution_system.execute_request(request);
}

TEST_F(TradingSystemExecutionSystem,
       HandlesInstrumentResolutionFailureOnOrderPlacementRequest) {
  const auto request = make_external_request<protocol::OrderPlacementRequest>();

  ON_CALL(instrument_resolver,
          resolve_instrument(A<const InstrumentDescriptor&>()))
      .WillByDefault(Return(
          tl::make_unexpected(instrument::LookupError::InstrumentNotFound)));

  EXPECT_CALL(trading_reply_receiver,
              process(A<protocol::OrderPlacementReject>()));

  execution_system.execute_request(request);
}

TEST_F(TradingSystemExecutionSystem,
       HandlesInstrumentResolutionFailureOnOrderModificationRequest) {
  const auto request =
      make_external_request<protocol::OrderModificationRequest>();

  ON_CALL(instrument_resolver,
          resolve_instrument(A<const InstrumentDescriptor&>()))
      .WillByDefault(Return(tl::make_unexpected(
          instrument::LookupError::AmbiguousInstrumentDescriptor)));

  EXPECT_CALL(trading_reply_receiver,
              process(A<protocol::OrderModificationReject>()));

  execution_system.execute_request(request);
}

TEST_F(TradingSystemExecutionSystem,
       HandlesInstrumentResolutionFailureOnOrderCancellationRequest) {
  const auto request =
      make_external_request<protocol::OrderCancellationRequest>();

  ON_CALL(instrument_resolver,
          resolve_instrument(A<const InstrumentDescriptor&>()))
      .WillByDefault(Return(tl::make_unexpected(
          instrument::LookupError::MalformedInstrumentDescriptor)));

  EXPECT_CALL(trading_reply_receiver,
              process(A<protocol::OrderCancellationReject>()));

  execution_system.execute_request(request);
}

TEST_F(TradingSystemExecutionSystem,
       RejectesMarketDataRequestWithNoInstruments) {
  auto request = make_external_request<protocol::MarketDataRequest>();
  ASSERT_THAT(request.instruments, IsEmpty());

  EXPECT_CALL(trading_reply_receiver, process(A<protocol::MarketDataReject>()));

  execution_system.execute_request(request);
}

TEST_F(TradingSystemExecutionSystem,
       RejectesMarketDataRequestWithMultipleInstruments) {
  auto request = make_external_request<protocol::MarketDataRequest>();
  request.instruments.emplace_back(requested_instrument);
  request.instruments.emplace_back(requested_instrument);

  EXPECT_CALL(trading_reply_receiver, process(A<protocol::MarketDataReject>()));

  execution_system.execute_request(request);
}

TEST_F(TradingSystemExecutionSystem,
       HandlesInstrumentResolutionFailureOnMarketDataRequest) {
  auto request = make_external_request<protocol::MarketDataRequest>();
  request.instruments.emplace_back(requested_instrument);

  ON_CALL(instrument_resolver,
          resolve_instrument(A<const InstrumentDescriptor&>()))
      .WillByDefault(Return(tl::make_unexpected(
          instrument::LookupError::MalformedInstrumentDescriptor)));

  EXPECT_CALL(trading_reply_receiver, process(A<protocol::MarketDataReject>()));

  execution_system.execute_request(request);
}

TEST_F(TradingSystemExecutionSystem, StoresStateForTwoInstruments) {
  std::vector<market_state::InstrumentState> states(
      2, market_state::InstrumentState{});
  std::vector<Instrument> instruments;
  instrument.identifier = InstrumentId{1};
  instruments.push_back(instrument);
  instrument.identifier = InstrumentId{2};
  instruments.push_back(instrument);

  EXPECT_CALL(repository_accessor, unicast_impl(_, _)).Times(2);

  execution_system.store_state_request(make_store_vector(instruments, states));
}

TEST_F(TradingSystemExecutionSystem, RecoverStateResolvesEachInstrument) {
  market_state::InstrumentData data1;
  data1.specification.symbol = Symbol{"AAPL"};
  market_state::InstrumentData data2;
  data2.specification.symbol = Symbol{"TSLA"};

  const std::vector<market_state::InstrumentData> data{data1, data2};

  ON_CALL(instrument_resolver,
          resolve_instrument(
              A<const instrument::Cache::InstrumentSpecification&>()))
      .WillByDefault(Return(
          tl::make_unexpected(instrument::LookupError::InstrumentNotFound)));

  EXPECT_CALL(instrument_resolver, resolve_instrument(data1.specification));
  EXPECT_CALL(instrument_resolver, resolve_instrument(data2.specification));

  execution_system.recover_state_request(data);
}

TEST_F(TradingSystemExecutionSystem,
       RecoverStatePassesLambdaToCorrespondingEngine) {
  market_state::InstrumentData data1;
  data1.specification.symbol = Symbol{"AAPL"};
  market_state::InstrumentData data2;
  data2.specification.symbol = Symbol{"TSLA"};

  const std::vector<market_state::InstrumentData> data{data1, data2};

  Instrument instrument1;
  instrument1.symbol = Symbol{"AAPL"};
  instrument1.identifier = InstrumentId{3};
  Instrument instrument2;
  instrument2.symbol = Symbol{"TSLA"};
  instrument2.identifier = InstrumentId{4};

  ON_CALL(instrument_resolver, resolve_instrument(data1.specification))
      .WillByDefault(Return(instrument::View{instrument1}));
  ON_CALL(instrument_resolver, resolve_instrument(data2.specification))
      .WillByDefault(Return(instrument::View{instrument2}));

  EXPECT_CALL(repository_accessor, unicast_impl(instrument1.identifier, _));
  EXPECT_CALL(repository_accessor, unicast_impl(instrument2.identifier, _));

  execution_system.recover_state_request(data);
}

TEST_F(TradingSystemExecutionSystem, BroadcastSessionTerminatedEvent) {
  EXPECT_CALL(repository_accessor, broadcast_impl(_));
  execution_system.handle(protocol::SessionTerminatedEvent{make_session()});
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::test
