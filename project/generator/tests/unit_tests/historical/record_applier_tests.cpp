#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "core/common/attribute.hpp"
#include "ih/adaptation/generated_message.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/record_applier.hpp"
#include "ih/registry/generated_order_data.hpp"
#include "ih/registry/generated_orders_registry_impl.hpp"
#include "mocks/context/order_context.hpp"
#include "mocks/registry/generated_orders_registry.hpp"
#include "tests/test_utils/generated_message_utils.hpp"
#include "tests/test_utils/historical_data_utils.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;

class GeneratorHistoricalRecordApplierFixture : public testing::Test {
 public:
  auto listing() -> data_layer::Listing& { return listing_; }

  auto venue() -> data_layer::Venue& { return venue_; }

  auto context() -> mock::OrderInstrumentContext& { return *context_; }

  auto apply(historical::Record record) -> std::vector<GeneratedMessage> {
    return RecordApplier::apply(std::move(record), context_);
  }

  auto make_record(std::initializer_list<historical::Level> levels)
      -> historical::Record {
    constexpr std::uint64_t source_row = 1;
    const auto receive_time = std::chrono::system_clock::now();

    Record::Builder builder;
    builder.with_receive_time(receive_time)
        .with_instrument(*listing_.symbol())
        .with_source_row(source_row);

    std::uint64_t index = 0;
    for (const historical::Level& level : levels) {
      builder.add_level(index++, level);
    }

    return historical::Record::Builder::construct(std::move(builder));
  }

  static auto make_registered_order(const ClientOrderId& order_id,
                                    OrderPrice price,
                                    Side side,
                                    Quantity quantity,
                                    const PartyId& counterparty)
      -> GeneratedOrderData {
    GeneratedOrderData::Builder builder{counterparty, order_id, side};
    builder.set_price(price).set_quantity(quantity);
    return GeneratedOrderData{std::move(builder)};
  }

 protected:
  inline static const std::string Symbol{"AAPL"};
  inline static const std::string VenueId{"NYSE"};

  static auto create_listing(const std::string& smbl,
                             const std::string& venue_id)
      -> data_layer::Listing {
    data_layer::Listing::Patch listing_patch;
    listing_patch.with_symbol(smbl).with_venue_id(venue_id);
    return data_layer::Listing::create(std::move(listing_patch), 1);
  }

  static auto create_venue(const std::string& venue_id) -> data_layer::Venue {
    data_layer::Venue::Patch venue_patch;
    venue_patch.with_venue_id(venue_id);
    return data_layer::Venue::create(std::move(venue_patch));
  }

  GeneratorHistoricalRecordApplierFixture()
      : listing_{create_listing(Symbol, VenueId)},
        venue_{create_venue(VenueId)} {}

  data_layer::Listing listing_;
  data_layer::Venue venue_;

  std::shared_ptr<mock::OrderInstrumentContext> context_;
};

class GeneratorHistoricalRecordApplierMockedRegistry
    : public GeneratorHistoricalRecordApplierFixture {
 public:
  auto registry() -> mock::GeneratedOrdersRegistry& { return registry_; }

 protected:
  GeneratorHistoricalRecordApplierMockedRegistry()
      : GeneratorHistoricalRecordApplierFixture() {
    context_ = std::make_shared<mock::OrderInstrumentContext>();

    EXPECT_CALL(context(), get_venue).WillRepeatedly(ReturnRef(venue()));

    EXPECT_CALL(context(), get_instrument).WillRepeatedly(ReturnRef(listing()));

    EXPECT_CALL(context(), take_registry).WillRepeatedly(ReturnRef(registry()));
  }

 private:
  mock::GeneratedOrdersRegistry registry_;
};

ACTION_P(CaptureGeneratedOrder, OptStorage) {
  GeneratedOrderData& captured_order = arg0;
  OptStorage.get() = std::move(captured_order);
}

ACTION_P(AppendGeneratedMessage, storage) {
  GeneratedOrderData& captured_order = arg0;
  storage.get().push_back(std::move(captured_order));
}

MATCHER_P5(IsNewOrderRequest, side, order_id, price, quantity, party, "") {
  return ExplainMatchResult(Eq(MessageType::NewOrderSingle),
                            arg.message_type,
                            result_listener) &&
         ExplainMatchResult(Optional(Eq(OrderType::Option::Limit)),
                            arg.order_type,
                            result_listener) &&
         ExplainMatchResult(Optional(Eq(TimeInForce::Option::Day)),
                            arg.time_in_force,
                            result_listener) &&
         ExplainMatchResult(Optional(Eq(side)), arg.side, result_listener) &&
         ExplainMatchResult(
             DoubleEq(price), arg.order_price->value(), result_listener) &&
         ExplainMatchResult(
             DoubleEq(quantity), arg.quantity->value(), result_listener) &&
         ExplainMatchResult(
             Optional(Eq(order_id)), arg.client_order_id, result_listener) &&
         ExplainMatchResult(
             Eq(std::nullopt), arg.orig_client_order_id, result_listener) &&
         ExplainMatchResult(
             Optional(GeneratedParty(party)), arg.party, result_listener);
}

MATCHER_P5(IsModificationRequest, side, order_id, price, quantity, party, "") {
  return ExplainMatchResult(Eq(MessageType::OrderCancelReplaceRequest),
                            arg.message_type,
                            result_listener) &&
         ExplainMatchResult(Optional(Eq(OrderType::Option::Limit)),
                            arg.order_type,
                            result_listener) &&
         ExplainMatchResult(Optional(Eq(TimeInForce::Option::Day)),
                            arg.time_in_force,
                            result_listener) &&
         ExplainMatchResult(Optional(Eq(side)), arg.side, result_listener) &&
         ExplainMatchResult(
             DoubleEq(price), arg.order_price->value(), result_listener) &&
         ExplainMatchResult(
             DoubleEq(quantity), arg.quantity->value(), result_listener) &&
         ExplainMatchResult(
             Optional(Eq(order_id)), arg.client_order_id, result_listener) &&
         ExplainMatchResult(Eq(order_id.value()),
                            arg.orig_client_order_id->value(),
                            result_listener) &&
         ExplainMatchResult(
             Optional(GeneratedParty(party)), arg.party, result_listener);
}

MATCHER_P5(IsCancelRequest, side, order_id, price, quantity, party, "") {
  return ExplainMatchResult(Eq(MessageType::OrderCancelRequest),
                            arg.message_type,
                            result_listener) &&
         ExplainMatchResult(Optional(Eq(OrderType::Option::Limit)),
                            arg.order_type,
                            result_listener) &&
         ExplainMatchResult(Optional(Eq(TimeInForce::Option::Day)),
                            arg.time_in_force,
                            result_listener) &&
         ExplainMatchResult(Optional(Eq(side)), arg.side, result_listener) &&
         ExplainMatchResult(
             DoubleEq(price), arg.order_price->value(), result_listener) &&
         ExplainMatchResult(
             DoubleEq(quantity), arg.quantity->value(), result_listener) &&
         ExplainMatchResult(
             Optional(Eq(order_id)), arg.client_order_id, result_listener) &&
         ExplainMatchResult(Eq(order_id.value()),
                            arg.orig_client_order_id->value(),
                            result_listener) &&
         ExplainMatchResult(
             Optional(GeneratedParty(party)), arg.party, result_listener);
}

MATCHER_P5(IsGeneratedOrder, side, order_id, price, quantity, party, "") {
  return ExplainMatchResult(Eq(side), arg.get_order_side(), result_listener) &&
         ExplainMatchResult(
             Eq(order_id), arg.get_order_id(), result_listener) &&
         ExplainMatchResult(
             DoubleEq(price), arg.get_order_px().value(), result_listener) &&
         ExplainMatchResult(DoubleEq(quantity),
                            arg.get_order_qty().value(),
                            result_listener) &&
         ExplainMatchResult(Eq(party), arg.get_owner_id(), result_listener);
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       DoesNotGenerateMessagesFromNotProcessableLevel) {
  constexpr double bid_px = 101.1;
  constexpr double offer_qty = 202.2;

  const auto level = make_level(bid_px,
                                std::nullopt,
                                std::nullopt,
                                std::nullopt,
                                offer_qty,
                                std::nullopt);
  ASSERT_FALSE(historical::RecordApplier::RecordChecker::is_processable(level));

  EXPECT_CALL(registry(), select_by)
      .Times(1)
      .WillOnce(Return(std::vector<GeneratedOrderData>{}));

  const auto messages = apply(make_record({level}));

  EXPECT_TRUE(messages.empty());
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       DoesNotGenerateMessagesFromEmptyRecord) {
  const auto record = make_record({});
  ASSERT_FALSE(record.has_levels());

  EXPECT_CALL(registry(), select_by)
      .Times(2)
      .WillRepeatedly(Return(std::vector<GeneratedOrderData>{}));

  const auto messages = apply(record);
  EXPECT_TRUE(messages.empty());
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       AppliesTopBidNewOrderIfPartyIsAbsent) {
  // Default counterparty assigned by historical applier
  const PartyId counterparty{"CP1"};
  const ClientOrderId order_id{"NewOrderID"};
  constexpr double price = 32.4;
  constexpr double quantity = 23.44;

  const auto level = make_level(
      price, quantity, std::nullopt, std::nullopt, std::nullopt, std::nullopt);

  const auto record = make_record({level});

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(1)
      .WillOnce(Return(order_id.value()));

  EXPECT_CALL(registry(), select_by)
      .Times(1)
      .WillOnce(Return(std::vector<GeneratedOrderData>{}));

  EXPECT_CALL(registry(), find_by_owner(Eq(counterparty.value())))
      .Times(1)
      .WillOnce(Return(std::nullopt));

  // A storage of our new order which was generated from the level
  std::optional<GeneratedOrderData> generated_order;
  EXPECT_CALL(registry(), add(testing::_))
      .Times(1)
      .WillOnce(DoAll(CaptureGeneratedOrder(std::ref(generated_order)),
                      Return(true)));

  std::vector<GeneratedMessage> messages = apply(record);

  ASSERT_EQ(messages.size(), 1);
  const auto& new_order_request = messages.front();
  ASSERT_THAT(new_order_request,
              IsNewOrderRequest(
                  Side::Option::Buy, order_id, price, quantity, counterparty));

  ASSERT_THAT(generated_order,
              Optional(IsGeneratedOrder(
                  Side::Option::Buy, order_id, price, quantity, counterparty)));
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       AppliesTopBidNewOrderWithParty) {
  const std::string counterparty{"Custom-Counterparty"};
  const std::string order_id{"NewOrderID"};
  constexpr double price = 32.4;
  constexpr double quantity = 23.44;

  const auto level = make_level(
      price, quantity, counterparty, std::nullopt, std::nullopt, std::nullopt);

  const auto record = make_record({level});

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(1)
      .WillOnce(Return(order_id));

  EXPECT_CALL(registry(), select_by)
      .Times(1)
      .WillOnce(Return(std::vector<GeneratedOrderData>{}));

  EXPECT_CALL(registry(), find_by_owner(Eq(counterparty)))
      .Times(1)
      .WillOnce(Return(std::nullopt));

  // A storage of our new order which was generated from the level
  std::optional<GeneratedOrderData> generated_order;
  EXPECT_CALL(registry(), add(testing::_))
      .Times(1)
      .WillOnce(DoAll(CaptureGeneratedOrder(std::ref(generated_order)),
                      Return(true)));

  std::vector<GeneratedMessage> messages = apply(record);

  ASSERT_EQ(messages.size(), 1);
  const auto& new_order_request = messages.front();
  EXPECT_THAT(new_order_request.party,
              Optional(GeneratedParty(PartyId{counterparty})));

  ASSERT_TRUE(generated_order.has_value());
  EXPECT_EQ(generated_order->get_owner_id().value(), counterparty);
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       ModifiesTopBidWithSameParty) {
  const PartyId counterparty{"Custom-Counterparty"};
  const std::string order_id{"NewOrderID"};
  constexpr double price = 32.4;
  constexpr double quantity = 23.44;

  const ClientOrderId stored_order_id{"BetterOrderID"};
  constexpr OrderPrice stored_ord_px{33.4};
  constexpr Quantity stored_ord_qty{33.5};

  const auto level = make_level(price,
                                quantity,
                                counterparty.value(),
                                std::nullopt,
                                std::nullopt,
                                std::nullopt);

  const auto record = make_record({level});

  const auto stored_order = make_registered_order(stored_order_id,
                                                  stored_ord_px,
                                                  Side::Option::Buy,
                                                  stored_ord_qty,
                                                  counterparty);

  EXPECT_CALL(registry(), select_by)
      .Times(1)
      .WillOnce(Return(std::vector<GeneratedOrderData>{}));

  EXPECT_CALL(registry(), find_by_owner(Eq(counterparty.value())))
      .Times(1)
      .WillOnce(Return(stored_order));

  EXPECT_CALL(registry(), update_by_owner(Eq(counterparty.value()), testing::_))
      .Times(1)
      .WillOnce(Return(true));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 1);
  ASSERT_THAT(
      messages[0],
      IsModificationRequest(
          Side::Option::Buy, stored_order_id, price, quantity, counterparty));
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       AppliesTopOfferNewOrderIfPartyIsAbsent) {
  // Default counterparty assigned by historical applier
  const PartyId counterparty{"CP1"};
  const ClientOrderId order_id{"NewOrderID"};
  constexpr double price = 32.4;
  constexpr double quantity = 23.44;

  const auto level = make_level(
      std::nullopt, std::nullopt, std::nullopt, price, quantity, std::nullopt);

  const auto record = make_record({level});

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(1)
      .WillOnce(Return(order_id.value()));

  EXPECT_CALL(registry(), select_by)
      .Times(1)
      .WillOnce(Return(std::vector<GeneratedOrderData>{}));

  EXPECT_CALL(registry(), find_by_owner(Eq(counterparty.value())))
      .Times(1)
      .WillOnce(Return(std::nullopt));

  std::optional<GeneratedOrderData> generated_order;
  EXPECT_CALL(registry(), add(testing::_))
      .Times(1)
      .WillOnce(DoAll(CaptureGeneratedOrder(std::ref(generated_order)),
                      Return(true)));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 1);

  const auto& new_order_request = messages.front();
  ASSERT_THAT(new_order_request,
              IsNewOrderRequest(
                  Side::Option::Sell, order_id, price, quantity, counterparty));

  ASSERT_THAT(
      generated_order,
      Optional(IsGeneratedOrder(
          Side::Option::Sell, order_id, price, quantity, counterparty)));
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       AppliesTopOfferNewOrderWithParty) {
  const std::string counterparty{"Custom-Counterparty"};
  const std::string order_id{"NewOrderID"};
  constexpr double price = 32.4;
  constexpr double quantity = 23.44;

  const auto level = make_level(
      std::nullopt, std::nullopt, std::nullopt, price, quantity, counterparty);

  const auto record = make_record({level});

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(1)
      .WillOnce(Return(order_id));

  EXPECT_CALL(registry(), select_by)
      .Times(1)
      .WillOnce(Return(std::vector<GeneratedOrderData>{}));

  EXPECT_CALL(registry(), find_by_owner(Eq(counterparty)))
      .Times(1)
      .WillOnce(Return(std::nullopt));

  // A storage of our new order which was generated from the level
  std::optional<GeneratedOrderData> generated_order;
  EXPECT_CALL(registry(), add(testing::_))
      .Times(1)
      .WillOnce(DoAll(CaptureGeneratedOrder(std::ref(generated_order)),
                      Return(true)));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 1);

  const auto& new_order_request = messages.front();
  EXPECT_THAT(new_order_request.party,
              Optional(GeneratedParty(PartyId{counterparty})));

  ASSERT_TRUE(generated_order.has_value());
  EXPECT_EQ(generated_order->get_owner_id().value(), counterparty);
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       ModifiesTopOfferWithSameParty) {
  const PartyId counterparty{"Custom-Counterparty"};
  const std::string order_id{"NewOrderID"};
  constexpr double price = 32.4;
  constexpr double quantity = 23.44;

  const ClientOrderId stored_order_id{"BetterOrderID"};
  constexpr OrderPrice stored_ord_px{33.4};
  constexpr Quantity stored_ord_qty{33.5};

  const auto level = make_level(std::nullopt,
                                std::nullopt,
                                std::nullopt,
                                price,
                                quantity,
                                counterparty.value());

  const auto record = make_record({level});

  const auto stored_order = make_registered_order(stored_order_id,
                                                  stored_ord_px,
                                                  Side::Option::Sell,
                                                  stored_ord_qty,
                                                  counterparty);

  EXPECT_CALL(registry(), select_by)
      .Times(1)
      .WillOnce(Return(std::vector<GeneratedOrderData>{}));

  EXPECT_CALL(registry(), find_by_owner(Eq(counterparty.value())))
      .Times(1)
      .WillOnce(Return(stored_order));

  EXPECT_CALL(registry(), update_by_owner(Eq(counterparty.value()), testing::_))
      .Times(1)
      .WillOnce(Return(true));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 1);
  ASSERT_THAT(
      messages[0],
      IsModificationRequest(
          Side::Option::Sell, stored_order_id, price, quantity, counterparty));
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       SetsPartiesForOneLevelWithoutParties) {
  const auto level = make_level(1., 1., std::nullopt, 2., 2., std::nullopt);
  const auto record = make_record({level});

  EXPECT_CALL(registry(), select_by)
      .Times(1)
      .WillOnce(Return(std::vector<GeneratedOrderData>{}));

  EXPECT_CALL(registry(), find_by_owner(Eq("CP1")))
      .WillOnce(Return(std::nullopt));
  EXPECT_CALL(registry(), find_by_owner(Eq("CP2")))
      .WillOnce(Return(std::nullopt));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(2)
      .WillOnce(Return("Order1"))
      .WillOnce(Return("Order2"));

  std::array<std::optional<GeneratedOrderData>, 2> generated_orders;
  EXPECT_CALL(registry(), add)
      .Times(2)
      .WillOnce(DoAll(CaptureGeneratedOrder(std::ref(generated_orders[0])),
                      Return(true)))
      .WillOnce(DoAll(CaptureGeneratedOrder(std::ref(generated_orders[1])),
                      Return(true)));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 2);
  ASSERT_THAT(messages[0].party, Optional(GeneratedParty(PartyId{"CP1"})));
  ASSERT_THAT(messages[1].party, Optional(GeneratedParty(PartyId{"CP2"})));

  ASSERT_THAT(generated_orders[0]->get_owner_id(), Eq(PartyId{"CP1"}));
  ASSERT_THAT(generated_orders[1]->get_owner_id(), Eq(PartyId{"CP2"}));
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       ResetsPartyCounterOnNewRecord) {
  const auto level1 = make_level(1., 1., std::nullopt, 2., 2., std::nullopt);
  const auto level2 = make_level(5., 5., std::nullopt, 6., 6., std::nullopt);

  const auto record1 = make_record({level1});
  const auto record2 = make_record({level2});

  EXPECT_CALL(registry(), select_by)
      .Times(2)
      .WillRepeatedly(Return(std::vector<GeneratedOrderData>{}));

  EXPECT_CALL(registry(), find_by_owner("CP1"))
      .Times(2)
      .WillRepeatedly(Return(std::nullopt));
  EXPECT_CALL(registry(), find_by_owner("CP2"))
      .Times(2)
      .WillRepeatedly(Return(std::nullopt));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(4)
      .WillRepeatedly(
          ReturnRoundRobin({"Order1", "Order2", "Order3", "Order4"}));

  std::vector<GeneratedOrderData> generated_orders;
  EXPECT_CALL(registry(), add)
      .Times(4)
      .WillRepeatedly(DoAll(AppendGeneratedMessage(std::ref(generated_orders)),
                            Return(true)));

  std::vector<GeneratedMessage> messages1 = apply(record1);
  std::vector<GeneratedMessage> messages2 = apply(record2);
  ASSERT_EQ(messages1.size(), 2);
  ASSERT_THAT(messages1[0].party, Optional(GeneratedParty(PartyId{"CP1"})));
  ASSERT_THAT(messages1[1].party, Optional(GeneratedParty(PartyId{"CP2"})));

  ASSERT_EQ(messages2.size(), 2);
  ASSERT_THAT(messages2[0].party, Optional(GeneratedParty(PartyId{"CP1"})));
  ASSERT_THAT(messages2[1].party, Optional(GeneratedParty(PartyId{"CP2"})));

  ASSERT_EQ(generated_orders.size(), 4);
  ASSERT_THAT(generated_orders[0].get_owner_id(), Eq(PartyId{"CP1"}));
  ASSERT_THAT(generated_orders[1].get_owner_id(), Eq(PartyId{"CP2"}));
  ASSERT_THAT(generated_orders[2].get_owner_id(), Eq(PartyId{"CP1"}));
  ASSERT_THAT(generated_orders[3].get_owner_id(), Eq(PartyId{"CP2"}));
}

TEST_F(GeneratorHistoricalRecordApplierMockedRegistry,
       SetsPartyForThreeLevelsWithoutParty) {
  const auto level1 = make_level(1., 1., std::nullopt, 2., 2., "Counterparty1");
  const auto level2 = make_level(3, 3., "Counterparty2", 4., 4., std::nullopt);
  const auto level3 = make_level(5., 5., std::nullopt, 6., 6., std::nullopt);

  const auto record = make_record({level1, level2, level3});

  EXPECT_CALL(registry(), select_by)
      .Times(1)
      .WillOnce(Return(std::vector<GeneratedOrderData>{}));

  EXPECT_CALL(registry(), find_by_owner("CP1")).WillOnce(Return(std::nullopt));
  EXPECT_CALL(registry(), find_by_owner("Counterparty1"))
      .WillOnce(Return(std::nullopt));
  EXPECT_CALL(registry(), find_by_owner("Counterparty2"))
      .WillOnce(Return(std::nullopt));
  EXPECT_CALL(registry(), find_by_owner("CP2")).WillOnce(Return(std::nullopt));
  EXPECT_CALL(registry(), find_by_owner("CP3")).WillOnce(Return(std::nullopt));
  EXPECT_CALL(registry(), find_by_owner("CP4")).WillOnce(Return(std::nullopt));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(6)
      .WillRepeatedly(ReturnRoundRobin(
          {"Order1", "Order2", "Order3", "Order4", "Order5", "Order6"}));

  std::vector<GeneratedOrderData> generated_orders;
  EXPECT_CALL(registry(), add)
      .Times(6)
      .WillRepeatedly(DoAll(AppendGeneratedMessage(std::ref(generated_orders)),
                            Return(true)));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 6);
  ASSERT_THAT(messages[0].party, Optional(GeneratedParty(PartyId{"CP1"})));
  ASSERT_THAT(messages[1].party,
              Optional(GeneratedParty(PartyId{"Counterparty1"})));
  ASSERT_THAT(messages[2].party,
              Optional(GeneratedParty(PartyId{"Counterparty2"})));
  ASSERT_THAT(messages[3].party, Optional(GeneratedParty(PartyId{"CP2"})));
  ASSERT_THAT(messages[4].party, Optional(GeneratedParty(PartyId{"CP3"})));
  ASSERT_THAT(messages[5].party, Optional(GeneratedParty(PartyId{"CP4"})));

  ASSERT_EQ(generated_orders.size(), 6);
  ASSERT_THAT(generated_orders[0].get_owner_id(), Eq(PartyId{"CP1"}));
  ASSERT_THAT(generated_orders[1].get_owner_id(), Eq(PartyId{"Counterparty1"}));
  ASSERT_THAT(generated_orders[2].get_owner_id(), Eq(PartyId{"Counterparty2"}));
  ASSERT_THAT(generated_orders[3].get_owner_id(), Eq(PartyId{"CP2"}));
  ASSERT_THAT(generated_orders[4].get_owner_id(), Eq(PartyId{"CP3"}));
  ASSERT_THAT(generated_orders[5].get_owner_id(), Eq(PartyId{"CP4"}));
}

class GeneratorHistoricalRecordApplier
    : public GeneratorHistoricalRecordApplierFixture {
 public:
  auto registry() -> generator::GeneratedOrdersRegistryImpl& {
    return registry_;
  }

  auto registry_select_by_party_id(const PartyId& party_id) const
      -> std::vector<GeneratedOrderData> {
    return registry_.select_by([&party_id](const GeneratedOrderData& order) {
      return order.get_owner_id() == party_id;
    });
  }

 protected:
  GeneratorHistoricalRecordApplier() {
    context_ = std::make_shared<mock::OrderInstrumentContext>();

    EXPECT_CALL(context(), get_venue).WillRepeatedly(ReturnRef(venue()));

    EXPECT_CALL(context(), get_instrument).WillRepeatedly(ReturnRef(listing()));

    EXPECT_CALL(context(), take_registry).WillRepeatedly(ReturnRef(registry()));
  }

 private:
  generator::GeneratedOrdersRegistryImpl registry_;
};

TEST_F(GeneratorHistoricalRecordApplier, CancelsOldBuyOrderIfNewRecordIsEmpty) {
  constexpr OrderPrice price{644.4};
  constexpr Quantity quantity{113.4};
  const ClientOrderId order_id{"Registered-BID"};
  const PartyId counterparty{"Owner-BID"};

  auto registered_order = make_registered_order(
      order_id, price, Side::Option::Buy, quantity, counterparty);
  registry().add(std::move(registered_order));

  const auto record = make_record({});
  ASSERT_FALSE(record.has_levels());

  const auto messages = apply(record);
  ASSERT_EQ(messages.size(), 1);

  const auto& cancel_request = messages.front();
  ASSERT_THAT(cancel_request,
              IsCancelRequest(Side::Option::Buy,
                              order_id,
                              price.value(),
                              quantity.value(),
                              counterparty));

  ASSERT_TRUE(registry_select_by_party_id(counterparty).empty());
}

TEST_F(GeneratorHistoricalRecordApplier,
       CancelsOldSellOrderIfNewRecordIsEmpty) {
  constexpr OrderPrice price{644.4};
  constexpr Quantity quantity{113.4};
  const ClientOrderId order_id{"Registered-OFFER"};
  const PartyId counterparty{"Owner-OFFER"};

  auto registered_order = make_registered_order(
      order_id, price, Side::Option::Sell, quantity, counterparty);
  registry().add(std::move(registered_order));

  const auto record = make_record({});
  ASSERT_FALSE(record.has_levels());

  const auto messages = apply(record);

  ASSERT_FALSE(messages.empty());
  const auto& cancel_request = messages.front();

  ASSERT_THAT(cancel_request,
              IsCancelRequest(Side::Option::Sell,
                              order_id,
                              price.value(),
                              quantity.value(),
                              counterparty));
  ASSERT_TRUE(registry_select_by_party_id(counterparty).empty());
}

TEST_F(
    GeneratorHistoricalRecordApplier,
    CancelsBetterOldTopBidOrderAndPlacesNewWorseBidOrderIfTheyHaveDifferentCounterparties) {
  const PartyId counterparty{"Custom-Counterparty"};
  const ClientOrderId order_id{"NewOrderID"};
  constexpr double price = 32.4;
  constexpr double quantity = 23.44;

  const ClientOrderId better_order_id{"BetterOrderID"};
  const PartyId better_order_owner{"BetterOrderOwner"};
  constexpr OrderPrice better_ord_px{33.4};
  constexpr Quantity better_ord_qty{33.5};

  const auto level = make_level(price,
                                quantity,
                                counterparty.value(),
                                std::nullopt,
                                std::nullopt,
                                std::nullopt);

  const auto record = make_record({level});

  auto better_order = make_registered_order(better_order_id,
                                            better_ord_px,
                                            Side::Option::Buy,
                                            better_ord_qty,
                                            better_order_owner);
  registry().add(std::move(better_order));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(1)
      .WillOnce(Return(order_id.value()));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 2);

  ASSERT_THAT(messages[0],
              IsCancelRequest(Side::Option::Buy,
                              better_order_id,
                              better_ord_px.value(),
                              better_ord_qty.value(),
                              better_order_owner));

  ASSERT_THAT(messages[1],
              IsNewOrderRequest(
                  Side::Option::Buy, order_id, price, quantity, counterparty));

  ASSERT_TRUE(registry_select_by_party_id(better_order_owner).empty());

  const auto generated_orders = registry_select_by_party_id(counterparty);
  ASSERT_EQ(generated_orders.size(), 1);
  ASSERT_THAT(generated_orders[0],
              IsGeneratedOrder(
                  Side::Option::Buy, order_id, price, quantity, counterparty));
}

TEST_F(GeneratorHistoricalRecordApplier,
       CancelsSellOrderAndPlacesBuyOrderIfTheirPartiesAreSame) {
  const PartyId counterparty{"Custom-Counterparty"};
  const ClientOrderId order_id{"NewOrderID"};
  constexpr double price = 32.4;
  constexpr double quantity = 23.44;

  const ClientOrderId stored_order_id{"StoredOrderID"};
  constexpr OrderPrice stored_ord_px{33.4};
  constexpr Quantity stored_ord_qty{33.5};

  const auto level = make_level(price,
                                quantity,
                                counterparty.value(),
                                std::nullopt,
                                std::nullopt,
                                std::nullopt);

  const auto record = make_record({level});

  auto stored_order = make_registered_order(stored_order_id,
                                            stored_ord_px,
                                            Side::Option::Sell,
                                            stored_ord_qty,
                                            counterparty);
  registry().add(std::move(stored_order));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(1)
      .WillOnce(Return(order_id.value()));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 2);
  ASSERT_THAT(messages[0],
              IsCancelRequest(Side::Option::Sell,
                              stored_order_id,
                              stored_ord_px.value(),
                              stored_ord_qty.value(),
                              counterparty));
  ASSERT_THAT(messages[1],
              IsNewOrderRequest(
                  Side::Option::Buy, order_id, price, quantity, counterparty));

  const auto generated_orders = registry_select_by_party_id(counterparty);
  ASSERT_EQ(generated_orders.size(), 1);
  ASSERT_THAT(generated_orders[0],
              IsGeneratedOrder(
                  Side::Option::Buy, order_id, price, quantity, counterparty));
}

TEST_F(GeneratorHistoricalRecordApplier,
       CancelsSellOrdersAndPlacesBuyOrdersIfTheirPartiesAreSame) {
  const PartyId counterparty1{"Counterparty1"};
  const PartyId counterparty2{"Counterparty2"};

  const auto level1 = make_level(33.4,
                                 24.44,
                                 counterparty1.value(),
                                 std::nullopt,
                                 std::nullopt,
                                 std::nullopt);
  const auto level2 = make_level(32.4,
                                 23.44,
                                 counterparty2.value(),
                                 std::nullopt,
                                 std::nullopt,
                                 std::nullopt);
  const auto record = make_record({level1, level2});

  auto stored_order1 = make_registered_order(ClientOrderId{"StoredOrderID1"},
                                             OrderPrice{33.4},
                                             Side::Option::Sell,
                                             Quantity{33.5},
                                             counterparty2);
  auto stored_order2 = make_registered_order(ClientOrderId{"StoredOrderID2"},
                                             OrderPrice{35.4},
                                             Side::Option::Sell,
                                             Quantity{50.5},
                                             counterparty1);
  registry().add(std::move(stored_order1));
  registry().add(std::move(stored_order2));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(2)
      .WillRepeatedly(ReturnRoundRobin({"Order1", "Order2"}));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 4);
  ASSERT_THAT(messages[0],
              IsCancelRequest(Side::Option::Sell,
                              ClientOrderId{"StoredOrderID2"},
                              35.4,
                              50.5,
                              counterparty1));

  ASSERT_THAT(messages[1],
              IsNewOrderRequest(Side::Option::Buy,
                                ClientOrderId{"Order1"},
                                33.4,
                                24.44,
                                counterparty1));
  ASSERT_THAT(messages[2],
              IsCancelRequest(Side::Option::Sell,
                              ClientOrderId{"StoredOrderID1"},
                              33.4,
                              33.5,
                              counterparty2));
  ASSERT_THAT(messages[3],
              IsNewOrderRequest(Side::Option::Buy,
                                ClientOrderId{"Order2"},
                                32.4,
                                23.44,
                                counterparty2));

  const auto generated_orders1 = registry_select_by_party_id(counterparty1);
  ASSERT_EQ(generated_orders1.size(), 1);
  ASSERT_THAT(generated_orders1[0],
              IsGeneratedOrder(Side::Option::Buy,
                               ClientOrderId{"Order1"},
                               33.4,
                               24.44,
                               counterparty1));
  const auto generated_orders2 = registry_select_by_party_id(counterparty2);
  ASSERT_THAT(generated_orders2[0],
              IsGeneratedOrder(Side::Option::Buy,
                               ClientOrderId{"Order2"},
                               32.4,
                               23.44,
                               counterparty2));
}

TEST_F(
    GeneratorHistoricalRecordApplier,
    CancelsBetterOldTopOfferOrderAndPlacesNewWorseOfferOrderIfTheyHaveDifferentCounterparties) {
  const PartyId counterparty{"Custom-Counterparty"};
  const ClientOrderId order_id{"NewOrderID"};
  constexpr double price = 32.4;
  constexpr double quantity = 23.44;

  const ClientOrderId better_order_id{"BetterOrderID"};
  const PartyId better_order_owner{"BetterOrderOwner"};
  constexpr OrderPrice better_ord_px{30.4};
  constexpr Quantity better_ord_qty{30.5};

  const auto level = make_level(std::nullopt,
                                std::nullopt,
                                std::nullopt,
                                price,
                                quantity,
                                counterparty.value());

  const auto record = make_record({level});

  auto better_order = make_registered_order(better_order_id,
                                            better_ord_px,
                                            Side::Option::Sell,
                                            better_ord_qty,
                                            better_order_owner);
  registry().add(std::move(better_order));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(1)
      .WillOnce(Return(order_id.value()));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 2);

  ASSERT_THAT(messages[0],
              IsCancelRequest(Side::Option::Sell,
                              better_order_id,
                              better_ord_px.value(),
                              better_ord_qty.value(),
                              better_order_owner));
  ASSERT_THAT(messages[1],
              IsNewOrderRequest(
                  Side::Option::Sell, order_id, price, quantity, counterparty));

  ASSERT_TRUE(registry_select_by_party_id(better_order_owner).empty());

  const auto generated_orders = registry_select_by_party_id(counterparty);
  ASSERT_THAT(generated_orders[0],
              IsGeneratedOrder(
                  Side::Option::Sell, order_id, price, quantity, counterparty));
}

TEST_F(GeneratorHistoricalRecordApplier,
       CancelsBuyOrderAndPlacesSellOrderIfTheirPartiesAreSame) {
  const PartyId counterparty{"Custom-Counterparty"};
  const ClientOrderId order_id{"NewOrderID"};
  constexpr double price = 32.4;
  constexpr double quantity = 23.44;

  const ClientOrderId stored_order_id{"StoredOrderID"};
  constexpr OrderPrice stored_ord_px{33.4};
  constexpr Quantity stored_ord_qty{33.5};

  const auto level = make_level(std::nullopt,
                                std::nullopt,
                                std::nullopt,
                                price,
                                quantity,
                                counterparty.value());

  const auto record = make_record({level});

  auto stored_order = make_registered_order(stored_order_id,
                                            stored_ord_px,
                                            Side::Option::Buy,
                                            stored_ord_qty,
                                            counterparty);
  registry().add(std::move(stored_order));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(1)
      .WillOnce(Return(order_id.value()));

  const std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 2);
  ASSERT_THAT(messages[0],
              IsCancelRequest(Side::Option::Buy,
                              stored_order_id,
                              stored_ord_px.value(),
                              stored_ord_qty.value(),
                              counterparty));
  ASSERT_THAT(messages[1],
              IsNewOrderRequest(
                  Side::Option::Sell, order_id, price, quantity, counterparty));

  const auto generated_orders = registry_select_by_party_id(counterparty);
  ASSERT_EQ(generated_orders.size(), 1);
  ASSERT_THAT(generated_orders[0],
              IsGeneratedOrder(
                  Side::Option::Sell, order_id, price, quantity, counterparty));
}

TEST_F(GeneratorHistoricalRecordApplier,
       CancelsBuyOrdersAndPlacesSellOrdersIfTheirPartiesAreSame) {
  const PartyId counterparty1{"Counterparty1"};
  const PartyId counterparty2{"Counterparty2"};

  const auto level1 = make_level(std::nullopt,
                                 std::nullopt,
                                 std::nullopt,
                                 32.4,
                                 23.44,
                                 counterparty1.value());
  const auto level2 = make_level(std::nullopt,
                                 std::nullopt,
                                 std::nullopt,
                                 33.4,
                                 24.44,
                                 counterparty2.value());
  const auto record = make_record({level1, level2});

  auto stored_order1 = make_registered_order(ClientOrderId{"StoredOrderID1"},
                                             OrderPrice{35.4},
                                             Side::Option::Buy,
                                             Quantity{50.5},
                                             counterparty2);
  auto stored_order2 = make_registered_order(ClientOrderId{"StoredOrderID2"},
                                             OrderPrice{33.4},
                                             Side::Option::Buy,
                                             Quantity{33.5},
                                             counterparty1);
  registry().add(std::move(stored_order1));
  registry().add(std::move(stored_order2));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(2)
      .WillRepeatedly(ReturnRoundRobin({"Order1", "Order2"}));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 4);
  ASSERT_THAT(messages[0],
              IsCancelRequest(Side::Option::Buy,
                              ClientOrderId{"StoredOrderID2"},
                              33.4,
                              33.5,
                              counterparty1));

  ASSERT_THAT(messages[1],
              IsNewOrderRequest(Side::Option::Sell,
                                ClientOrderId{"Order1"},
                                32.4,
                                23.44,
                                counterparty1));
  ASSERT_THAT(messages[2],
              IsCancelRequest(Side::Option::Buy,
                              ClientOrderId{"StoredOrderID1"},
                              35.4,
                              50.5,
                              counterparty2));
  ASSERT_THAT(messages[3],
              IsNewOrderRequest(Side::Option::Sell,
                                ClientOrderId{"Order2"},
                                33.4,
                                24.44,
                                counterparty2));

  const auto generated_orders1 = registry_select_by_party_id(counterparty1);
  ASSERT_EQ(generated_orders1.size(), 1);
  ASSERT_THAT(generated_orders1[0],
              IsGeneratedOrder(Side::Option::Sell,
                               ClientOrderId{"Order1"},
                               32.4,
                               23.44,
                               counterparty1));
  const auto generated_orders2 = registry_select_by_party_id(counterparty2);
  ASSERT_THAT(generated_orders2[0],
              IsGeneratedOrder(Side::Option::Sell,
                               ClientOrderId{"Order2"},
                               33.4,
                               24.44,
                               counterparty2));
}

TEST_F(GeneratorHistoricalRecordApplier,
       CancelsOldBidOrdersIfTheirPartiesAreNotPresentInCurrentRecord) {
  const auto level1 = make_level(
      12., 12., "Counterparty1", std::nullopt, std::nullopt, std::nullopt);
  const auto level2 = make_level(
      10, 10., "Counterparty2", std::nullopt, std::nullopt, std::nullopt);

  const auto record = make_record({level1, level2});

  auto old_buy_order_level1 = make_registered_order(ClientOrderId{"OldOrder1"},
                                                    OrderPrice{20.},
                                                    Side::Option::Buy,
                                                    Quantity{20.},
                                                    PartyId{"CP1"});
  auto old_buy_order_level2 = make_registered_order(ClientOrderId{"OldOrder2"},
                                                    OrderPrice{2.},
                                                    Side::Option::Buy,
                                                    Quantity{3.},
                                                    PartyId{"CP2"});
  registry().add(std::move(old_buy_order_level1));
  registry().add(std::move(old_buy_order_level2));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(2)
      .WillRepeatedly(ReturnRoundRobin({"Order1", "Order2"}));

  std::vector<GeneratedMessage> messages = apply(record);

  ASSERT_EQ(messages.size(), 4);
  ASSERT_THAT(messages[0],
              IsCancelRequest(Side::Option::Buy,
                              ClientOrderId{"OldOrder1"},
                              20.,
                              20.,
                              PartyId{"CP1"}));
  ASSERT_THAT(messages[1],
              IsCancelRequest(Side::Option::Buy,
                              ClientOrderId{"OldOrder2"},
                              2.,
                              3.,
                              PartyId{"CP2"}));

  ASSERT_TRUE(registry_select_by_party_id(PartyId{"CP1"}).empty());
  ASSERT_TRUE(registry_select_by_party_id(PartyId{"CP2"}).empty());
}

TEST_F(GeneratorHistoricalRecordApplier,
       CancelsOldOfferOrdersIfTheirPartiesAreNotPresentInCurrentRecord) {
  const auto level1 = make_level(
      std::nullopt, std::nullopt, std::nullopt, 10., 10., "Counterparty1");
  const auto level2 = make_level(
      std::nullopt, std::nullopt, std::nullopt, 12., 12., "Counterparty2");

  const auto record = make_record({level1, level2});

  auto old_sell_order_level1 = make_registered_order(ClientOrderId{"OldOrder1"},
                                                     OrderPrice{2.},
                                                     Side::Option::Sell,
                                                     Quantity{3.},
                                                     PartyId{"CP1"});
  auto old_sell_order_level2 = make_registered_order(ClientOrderId{"OldOrder2"},
                                                     OrderPrice{20.},
                                                     Side::Option::Sell,
                                                     Quantity{20.},
                                                     PartyId{"CP2"});
  registry().add(std::move(old_sell_order_level1));
  registry().add(std::move(old_sell_order_level2));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(2)
      .WillRepeatedly(ReturnRoundRobin({"Order1", "Order2"}));

  std::vector<GeneratedMessage> messages = apply(record);

  ASSERT_EQ(messages.size(), 4);
  ASSERT_THAT(messages[0],
              IsCancelRequest(Side::Option::Sell,
                              ClientOrderId{"OldOrder1"},
                              2.,
                              3.,
                              PartyId{"CP1"}));
  ASSERT_THAT(messages[1],
              IsCancelRequest(Side::Option::Sell,
                              ClientOrderId{"OldOrder2"},
                              20.,
                              20.,
                              PartyId{"CP2"}));

  ASSERT_TRUE(registry_select_by_party_id(PartyId{"CP1"}).empty());
  ASSERT_TRUE(registry_select_by_party_id(PartyId{"CP2"}).empty());
}

TEST_F(GeneratorHistoricalRecordApplier, ModifiesTwoBidLevelsWithSameParties) {
  const auto level1 = make_level(
      12., 12., "Counterparty1", std::nullopt, std::nullopt, std::nullopt);
  const auto level2 = make_level(
      10, 10., "Counterparty2", std::nullopt, std::nullopt, std::nullopt);

  const auto record = make_record({level1, level2});

  auto old_buy_order_level1 = make_registered_order(ClientOrderId{"OldOrder1"},
                                                    OrderPrice{20.},
                                                    Side::Option::Buy,
                                                    Quantity{20.},
                                                    PartyId{"Counterparty2"});
  auto old_buy_order_level2 = make_registered_order(ClientOrderId{"OldOrder2"},
                                                    OrderPrice{2.},
                                                    Side::Option::Buy,
                                                    Quantity{3.},
                                                    PartyId{"Counterparty1"});
  registry().add(std::move(old_buy_order_level1));
  registry().add(std::move(old_buy_order_level2));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 2);
  ASSERT_THAT(messages[0],
              IsModificationRequest(Side::Option::Buy,
                                    ClientOrderId{"OldOrder2"},
                                    12.,
                                    12.,
                                    PartyId{"Counterparty1"}));
  ASSERT_THAT(messages[1],
              IsModificationRequest(Side::Option::Buy,
                                    ClientOrderId{"OldOrder1"},
                                    10.,
                                    10.,
                                    PartyId{"Counterparty2"}));

  const auto generated_orders1 =
      registry_select_by_party_id(PartyId{"Counterparty1"});
  ASSERT_EQ(generated_orders1.size(), 1);
  ASSERT_THAT(generated_orders1[0],
              IsGeneratedOrder(Side::Option::Buy,
                               ClientOrderId{"OldOrder2"},
                               12.,
                               12.,
                               PartyId{"Counterparty1"}));

  const auto generated_orders2 =
      registry_select_by_party_id(PartyId{"Counterparty2"});
  ASSERT_EQ(generated_orders2.size(), 1);
  ASSERT_THAT(generated_orders2[0],
              IsGeneratedOrder(Side::Option::Buy,
                               ClientOrderId{"OldOrder1"},
                               10.,
                               10.,
                               PartyId{"Counterparty2"}));
}

TEST_F(GeneratorHistoricalRecordApplier,
       ModifiesTwoOfferLevelsWithSameParties) {
  const auto level1 = make_level(
      std::nullopt, std::nullopt, std::nullopt, 10., 10., "Counterparty1");
  const auto level2 = make_level(
      std::nullopt, std::nullopt, std::nullopt, 12., 12., "Counterparty2");

  const auto record = make_record({level1, level2});

  auto old_sell_order_level1 = make_registered_order(ClientOrderId{"OldOrder1"},
                                                     OrderPrice{2.},
                                                     Side::Option::Sell,
                                                     Quantity{3.},
                                                     PartyId{"Counterparty2"});
  auto old_sell_order_level2 = make_registered_order(ClientOrderId{"OldOrder2"},
                                                     OrderPrice{20.},
                                                     Side::Option::Sell,
                                                     Quantity{20.},
                                                     PartyId{"Counterparty1"});
  registry().add(std::move(old_sell_order_level1));
  registry().add(std::move(old_sell_order_level2));

  std::vector<GeneratedMessage> messages = apply(record);
  ASSERT_EQ(messages.size(), 2);
  ASSERT_THAT(messages[0],
              IsModificationRequest(Side::Option::Sell,
                                    ClientOrderId{"OldOrder2"},
                                    10.,
                                    10.,
                                    PartyId{"Counterparty1"}));
  ASSERT_THAT(messages[1],
              IsModificationRequest(Side::Option::Sell,
                                    ClientOrderId{"OldOrder1"},
                                    12.,
                                    12.,
                                    PartyId{"Counterparty2"}));

  const auto generated_orders1 =
      registry_select_by_party_id(PartyId{"Counterparty1"});
  ASSERT_EQ(generated_orders1.size(), 1);
  ASSERT_THAT(generated_orders1[0],
              IsGeneratedOrder(Side::Option::Sell,
                               ClientOrderId{"OldOrder2"},
                               10.,
                               10.,
                               PartyId{"Counterparty1"}));

  const auto generated_orders2 =
      registry_select_by_party_id(PartyId{"Counterparty2"});
  ASSERT_EQ(generated_orders2.size(), 1);
  ASSERT_THAT(generated_orders2[0],
              IsGeneratedOrder(Side::Option::Sell,
                               ClientOrderId{"OldOrder1"},
                               12.,
                               12.,
                               PartyId{"Counterparty2"}));
}

}  // namespace
}  // namespace simulator::generator::historical::test