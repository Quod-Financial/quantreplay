#include <fmt/format.h>
#include <gmock/gmock.h>

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "data_layer/api/models/listing.hpp"
#include "ih/adaptation/generated_message.hpp"
#include "ih/historical/processor.hpp"
#include "ih/market_data/order_book_derivation.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "ih/registry/generated_orders_registry_impl.hpp"
#include "middleware/channels/trading_request_channel.hpp"
#include "mocks/context/component_context.hpp"
#include "mocks/context/order_context.hpp"
#include "mocks/trading_request_channel.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

MATCHER_P4(IsOrderPlacement, side, price, quantity, party_id, "") {
  return ExplainMatchResult(
      AllOf(Field(&protocol::OrderPlacementRequest::side, Optional(Eq(side))),
            Field(&protocol::OrderPlacementRequest::order_price,
                  Optional(Eq(OrderPrice{price}))),
            Field(&protocol::OrderPlacementRequest::order_quantity,
                  Optional(Eq(OrderQuantity{quantity}))),
            Field(&protocol::OrderPlacementRequest::parties,
                  ElementsAre(Eq(generated_party(PartyId{party_id}))))),
      arg,
      result_listener);
}

// Feeds market data snapshots through the real historical pipeline
// (ActionProcessor, RecordApplier, orders registry) and observes the trading
// requests derived from them, as the acceptance criteria describe.
struct GeneratorOrderDerivation : public Test {
  const MdRequestId request_id{"MD-1"};
  const PartyId counterparty{"DS42"};

  data_layer::Listing listing = make_listing();
  InstrumentDescriptor descriptor;
  GeneratedOrdersRegistryImpl registry;
  StrictMock<mock::TradingRequestReceiver> receiver;
  std::shared_ptr<NiceMock<mock::OrderInstrumentContext>> instrument_context =
      std::make_shared<NiceMock<mock::OrderInstrumentContext>>();
  std::shared_ptr<NiceMock<mock::ComponentContext>> component_context =
      std::make_shared<NiceMock<mock::ComponentContext>>();
  std::unique_ptr<OrderBookDerivation> derivation;

  GeneratorOrderDerivation() {
    ON_CALL(*component_context, is_component_running())
        .WillByDefault(Return(true));
    ON_CALL(*instrument_context, get_instrument())
        .WillByDefault(ReturnRef(listing));
    ON_CALL(*instrument_context, get_instrument_descriptor())
        .WillByDefault(ReturnRef(descriptor));
    ON_CALL(*instrument_context, take_registry())
        .WillByDefault(ReturnRef(registry));
    ON_CALL(*instrument_context, get_synthetic_identifier())
        .WillByDefault(
            [this] { return fmt::format("GEN-{}", ++generated_identifiers_); });

    derivation = std::make_unique<OrderBookDerivation>(
        std::make_shared<historical::ActionProcessor>(
            historical::ActionProcessor::Contexts{instrument_context}),
        component_context);
    derivation->add(
        request_id,
        OrderDerivationRequirement{
            .session = protocol::Session{protocol::generator::Session{}},
            .symbol = Symbol{"AAPL"},
            .depth = AllDepthLevels,
            .counterparty = counterparty});
  }

  auto derive(std::vector<MarketDataEntry> entries) -> void {
    protocol::MarketDataSnapshot snapshot{
        protocol::Session{protocol::generator::Session{}}};
    snapshot.request_id = request_id;
    snapshot.market_data_entries = std::move(entries);
    derivation->process(snapshot);
  }

  [[nodiscard]]
  static auto bid(double price, double quantity) -> MarketDataEntry {
    return make_entry(MdEntryType::Option::Bid, price, quantity);
  }

  [[nodiscard]]
  static auto offer(double price, double quantity) -> MarketDataEntry {
    return make_entry(MdEntryType::Option::Offer, price, quantity);
  }

 private:
  [[nodiscard]]
  static auto make_listing() -> data_layer::Listing {
    data_layer::Listing::Patch patch;
    patch.with_symbol("AAPL").with_venue_id("NYSE");
    return data_layer::Listing::create(std::move(patch), 1);
  }

  [[nodiscard]]
  static auto make_entry(MdEntryType type,
                         double price,
                         double quantity) -> MarketDataEntry {
    MarketDataEntry entry;
    entry.type = type;
    entry.price = Price{price};
    entry.quantity = Quantity{quantity};
    return entry;
  }

  auto SetUp() -> void override {
    middleware::bind_trading_request_channel(
        std::shared_ptr<mock::TradingRequestReceiver>{
            std::addressof(receiver), [](auto* /*pointer*/) {}});
  }

  auto TearDown() -> void override {
    middleware::release_trading_request_channel();
  }

  std::size_t generated_identifiers_ = 0;
};

TEST_F(GeneratorOrderDerivation,
       PlacesOrderPerBookLevelUnderDatasourceCounterparty) {
  std::vector<protocol::OrderPlacementRequest> placements;
  EXPECT_CALL(receiver, process(An<protocol::OrderPlacementRequest>()))
      .Times(4)
      .WillRepeatedly([&placements](protocol::OrderPlacementRequest request) {
        placements.push_back(std::move(request));
      });

  derive({bid(100.0, 10.0),
          bid(99.0, 20.0),
          offer(101.0, 11.0),
          offer(102.0, 21.0)});

  EXPECT_THAT(placements,
              UnorderedElementsAre(
                  IsOrderPlacement(Side::Option::Buy, 100.0, 10.0, "DS42"),
                  IsOrderPlacement(Side::Option::Sell, 101.0, 11.0, "DS42"),
                  IsOrderPlacement(Side::Option::Buy, 99.0, 20.0, "DS42"),
                  IsOrderPlacement(Side::Option::Sell, 102.0, 21.0, "DS42")));
}

TEST_F(GeneratorOrderDerivation, SendsNothingForRepeatedUnchangedSnapshot) {
  EXPECT_CALL(receiver, process(An<protocol::OrderPlacementRequest>()))
      .Times(2);
  derive({bid(100.0, 10.0), offer(101.0, 11.0)});
  Mock::VerifyAndClearExpectations(&receiver);

  derive({bid(100.0, 10.0), offer(101.0, 11.0)});
}

TEST_F(GeneratorOrderDerivation, AmendsRestingOrderWhenLevelChanges) {
  EXPECT_CALL(receiver, process(An<protocol::OrderPlacementRequest>()))
      .Times(1);
  derive({bid(100.0, 10.0)});
  Mock::VerifyAndClearExpectations(&receiver);

  EXPECT_CALL(receiver,
              process(Matcher<protocol::OrderModificationRequest>(AllOf(
                  Field(&protocol::OrderModificationRequest::order_price,
                        Optional(Eq(OrderPrice{99.0}))),
                  Field(&protocol::OrderModificationRequest::parties,
                        ElementsAre(Eq(generated_party(PartyId{"DS42"}))))))));

  derive({bid(99.0, 10.0)});
}

TEST_F(GeneratorOrderDerivation, PlacesSingleOrderWhenBookGainsBestLevel) {
  EXPECT_CALL(receiver, process(An<protocol::OrderPlacementRequest>()))
      .Times(2);
  derive({bid(100.0, 10.0), bid(99.0, 20.0)});
  Mock::VerifyAndClearExpectations(&receiver);

  EXPECT_CALL(receiver,
              process(Matcher<protocol::OrderPlacementRequest>(
                  IsOrderPlacement(Side::Option::Buy, 101.0, 5.0, "DS42"))));

  derive({bid(101.0, 5.0), bid(100.0, 10.0), bid(99.0, 20.0)});
}

TEST_F(GeneratorOrderDerivation, CancelsSingleOrderWhenBestLevelDisappears) {
  EXPECT_CALL(receiver, process(An<protocol::OrderPlacementRequest>()))
      .Times(2);
  derive({bid(100.0, 10.0), bid(99.0, 20.0)});
  Mock::VerifyAndClearExpectations(&receiver);

  EXPECT_CALL(receiver,
              process(Matcher<protocol::OrderCancellationRequest>(
                  Field(&protocol::OrderCancellationRequest::side,
                        Optional(Eq(Side::Option::Buy))))));

  derive({bid(99.0, 20.0)});
}

TEST_F(GeneratorOrderDerivation, CancelsDerivedOrdersOnEmptySnapshot) {
  EXPECT_CALL(receiver, process(An<protocol::OrderPlacementRequest>()))
      .Times(2);
  derive({bid(100.0, 10.0), offer(101.0, 11.0)});
  Mock::VerifyAndClearExpectations(&receiver);

  EXPECT_CALL(receiver,
              process(Matcher<protocol::OrderCancellationRequest>(
                  Field(&protocol::OrderCancellationRequest::side,
                        Optional(Eq(Side::Option::Buy))))));
  EXPECT_CALL(receiver,
              process(Matcher<protocol::OrderCancellationRequest>(
                  Field(&protocol::OrderCancellationRequest::side,
                        Optional(Eq(Side::Option::Sell))))));

  derive({});
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
