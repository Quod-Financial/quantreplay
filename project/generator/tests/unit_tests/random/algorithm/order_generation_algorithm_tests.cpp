#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "core/domain/attributes.hpp"
#include "ih/adaptation/generated_message.hpp"
#include "ih/random/algorithm/order_generation_algorithm.hpp"
#include "ih/random/values/event.hpp"
#include "ih/random/values/resting_order_action.hpp"
#include "ih/registry/generated_order_data.hpp"
#include "mocks/context/order_context.hpp"
#include "mocks/random/counterparty_generator.hpp"
#include "mocks/random/price_generator.hpp"
#include "mocks/random/quantity_generator.hpp"
#include "mocks/random/random_event_generator.hpp"
#include "mocks/random/resting_order_action_generator.hpp"
#include "mocks/registry/generated_orders_registry.hpp"

namespace simulator::generator::random::test {
namespace {

using namespace ::testing;

class Generator_Random_OrderGenerationAlgorithm : public testing::Test {
  template <typename T>
  using Captured = std::optional<std::reference_wrapper<T>>;

 public:
  using EventType = random::Event::Type;
  using ActionType = random::RestingOrderAction::Type;

  inline static const std::string symbol{"TestListingSymbol"};
  inline static const std::string venue_id{"TestVenue"};

  constexpr static auto to_integer(random::Event::Type event)
      -> random::Event::RandomInteger {
    return static_cast<random::Event::RandomInteger>(event);
  }

  constexpr static auto to_integer(random::RestingOrderAction::Type action)
      -> random::RestingOrderAction::RandomInteger {
    return static_cast<random::RestingOrderAction::RandomInteger>(action);
  }

  auto get_context_pointer() -> std::shared_ptr<mock::OrderGenerationContext> {
    return context_;
  }

  auto context() -> mock::OrderGenerationContext& { return *context_; }

  auto registry() -> mock::GeneratedOrdersRegistry& { return registry_; }

  auto counterparty_generator() -> mock::CounterpartyGenerator& {
    return cp_generator_->get();
  }

  auto resting_order_action_generator() -> mock::RestingOrderActionGenerator& {
    return action_generator_->get();
  }

  auto price_generator() -> mock::PriceGenerator& {
    return price_generator_->get();
  }

  auto quantity_generator() -> mock::QuantityGenerator& {
    return quantity_generator_->get();
  }

  auto event_generator() -> mock::EventGenerator& {
    return event_generator_->get();
  }

  auto generate() -> std::optional<GeneratedMessage> {
    GeneratedMessage generated_message{};

    bool completed = false;
    EXPECT_NO_THROW(completed = algorithm_->generate(generated_message));

    return completed ? std::make_optional(generated_message) : std::nullopt;
  }

  static auto make_market_state(
      std::optional<double> bid_px = std::nullopt,
      std::optional<std::uint32_t> bid_depth = std::nullopt,
      std::optional<double> offer_px = std::nullopt,
      std::optional<std::uint32_t> offer_depth = std::nullopt) -> MarketState {
    MarketState state{};
    state.best_bid_price = bid_px;
    state.bid_depth_levels = bid_depth;
    state.best_offer_price = offer_px;
    state.offer_depth_levels = offer_depth;
    return state;
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
  static auto create_listing(const std::string& smbl,
                             const std::string& venue,
                             std::optional<std::uint32_t> random_depth_levels =
                                 std::nullopt) -> data_layer::Listing {
    data_layer::Listing::Patch listing_patch;
    listing_patch.with_symbol(smbl).with_venue_id(venue);
    if (random_depth_levels.has_value()) {
      listing_patch.with_random_depth_levels(*random_depth_levels);
    }
    return data_layer::Listing::create(std::move(listing_patch), 1);
  }

  static auto create_venue(const std::string& venue) -> data_layer::Venue {
    data_layer::Venue::Patch venue_patch;
    venue_patch.with_venue_id(venue);
    return data_layer::Venue::create(std::move(venue_patch));
  }

  static auto create_price_seed(const std::string& symbol)
      -> data_layer::PriceSeed {
    data_layer::PriceSeed::Patch price_seed_patch;
    price_seed_patch.with_symbol(symbol);
    return data_layer::PriceSeed::create(std::move(price_seed_patch), 1);
  }

  Generator_Random_OrderGenerationAlgorithm()
      : listing{create_listing(symbol, venue_id)},
        test_venue_{create_venue(venue_id)},
        test_price_seed_{create_price_seed(symbol)} {
    setup_test_entities();
    setup_algorithm();
  }

  auto setup_test_entities() -> void {
    context_ = std::make_shared<mock::OrderGenerationContext>();

    EXPECT_CALL(context(), get_venue).WillRepeatedly(ReturnRef(test_venue_));

    EXPECT_CALL(context(), get_instrument).WillRepeatedly(ReturnRef(listing));

    EXPECT_CALL(context(), get_price_seed)
        .WillRepeatedly(ReturnRef(test_price_seed_));

    EXPECT_CALL(context(), take_registry).WillRepeatedly(ReturnRef(registry_));
  }

  auto setup_algorithm() -> void {
    auto cp_gen = std::make_unique<mock::CounterpartyGenerator>();
    auto action_gen = std::make_unique<mock::RestingOrderActionGenerator>();
    auto price_gen = std::make_unique<mock::PriceGenerator>();
    auto qty_gen = std::make_unique<mock::QuantityGenerator>();
    auto event_gen = std::make_unique<mock::EventGenerator>();

    cp_generator_ = std::ref(*cp_gen);
    action_generator_ = std::ref(*action_gen);
    price_generator_ = std::ref(*price_gen);
    quantity_generator_ = std::ref(*qty_gen);
    event_generator_ = std::ref(*event_gen);

    algorithm_ = OrderGenerationAlgorithm::create(context_,
                                                  std::move(event_gen),
                                                  std::move(cp_gen),
                                                  std::move(action_gen),
                                                  std::move(price_gen),
                                                  std::move(qty_gen));
  }

  data_layer::Listing listing;

 private:
  data_layer::Venue test_venue_;
  data_layer::PriceSeed test_price_seed_;

  mock::GeneratedOrdersRegistry registry_;

  std::shared_ptr<mock::OrderGenerationContext> context_;
  std::unique_ptr<OrderGenerationAlgorithm> algorithm_;

  Captured<mock::CounterpartyGenerator> cp_generator_;
  Captured<mock::RestingOrderActionGenerator> action_generator_;
  Captured<mock::PriceGenerator> price_generator_;
  Captured<mock::QuantityGenerator> quantity_generator_;
  Captured<mock::EventGenerator> event_generator_;
};

TEST_F(Generator_Random_OrderGenerationAlgorithm, Construct_WithContextOnly) {
  EXPECT_NO_THROW(OrderGenerationAlgorithm::create(get_context_pointer()));
}

TEST_F(Generator_Random_OrderGenerationAlgorithm, Generate_NoopEvent) {
  constexpr auto event = EventType::NoOperation;

  EXPECT_CALL(event_generator(), generate_choice_integer)
      .Times(1)
      .WillOnce(Return(to_integer(event)));

  const std::optional<GeneratedMessage> generated = generate();

  EXPECT_FALSE(generated.has_value());
}

TEST_F(Generator_Random_OrderGenerationAlgorithm,
       Generate_Aggressive_OppositeSideEmpty) {
  constexpr auto event = EventType::AggressiveBuy;
  constexpr std::uint32_t cp_number = 3;

  // Sell side has no prices
  const MarketState state = make_market_state(120.1, 2);

  EXPECT_CALL(event_generator(), generate_choice_integer)
      .Times(1)
      .WillOnce(Return(to_integer(event)));

  EXPECT_CALL(counterparty_generator(), generate_counterparty_number)
      .Times(1)
      .WillOnce(Return(cp_number));

  EXPECT_CALL(context(), get_current_market_state)
      .Times(1)
      .WillOnce(Return(state));

  const std::optional<GeneratedMessage> generated = generate();

  EXPECT_FALSE(generated.has_value());
}

TEST_F(Generator_Random_OrderGenerationAlgorithm,
       Generate_Aggressive_OppositeSideHasPrices) {
  constexpr auto event = EventType::AggressiveSell;
  constexpr std::uint32_t cp_number = 3;
  constexpr double price = 149;
  constexpr double quantity = 170;
  const std::string order_id{"AggressiveSellOrderID"};
  const std::string party{"CP3"};  // Depends on cp_number

  // Sell side has no prices
  const MarketState state = make_market_state(150, 12);

  EXPECT_CALL(event_generator(), generate_choice_integer)
      .Times(1)
      .WillOnce(Return(to_integer(event)));

  EXPECT_CALL(counterparty_generator(), generate_counterparty_number)
      .Times(1)
      .WillOnce(Return(cp_number));

  EXPECT_CALL(context(), get_current_market_state)
      .Times(1)
      .WillRepeatedly(Return(state));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(1)
      .WillOnce(Return(order_id));

  EXPECT_CALL(price_generator(), generate_px)
      .Times(1)
      .WillOnce(Return(mock::PriceGenerator::make_output(price)));

  EXPECT_CALL(quantity_generator(), generate_random_qty)
      .Times(1)
      .WillOnce(Return(mock::QuantityGenerator::make_output(quantity)));

  const std::optional<GeneratedMessage> generated = generate();

  ASSERT_TRUE(generated.has_value());
  const auto& message = generated.value();

  EXPECT_EQ(message.message_type, MessageType::NewOrderSingle);
  EXPECT_EQ(message.order_type, OrderType::Option::Limit);
  EXPECT_EQ(message.side, Side::Option::Sell);
  EXPECT_EQ(message.time_in_force, TimeInForce::Option::ImmediateOrCancel);
  EXPECT_DOUBLE_EQ(message.order_price->value(), price);
  EXPECT_DOUBLE_EQ(message.quantity->value(), quantity);
  EXPECT_EQ(message.client_order_id->value(), order_id);
  EXPECT_EQ(message.party_id->value(), party);
}

TEST_F(Generator_Random_OrderGenerationAlgorithm,
       Generate_Resting_PartyHasNoOrder_NoMaxMktDepth) {
  constexpr auto event = EventType::RestingBuy;
  constexpr int cp_number = 5;
  constexpr double price = 34.54;
  constexpr double quantity = 20.05;
  const std::string party{"CP5"};
  const std::string order_id{"NewOrderID"};

  EXPECT_CALL(event_generator(), generate_choice_integer)
      .Times(1)
      .WillOnce(Return(to_integer(event)));

  EXPECT_CALL(counterparty_generator(), generate_counterparty_number)
      .Times(1)
      .WillOnce(Return(cp_number));

  EXPECT_CALL(registry(), find_by_owner(Eq(party)))
      .Times(1)
      .WillOnce(Return(std::nullopt));

  EXPECT_CALL(context(), get_synthetic_identifier)
      .Times(1)
      .WillOnce(Return(order_id));

  EXPECT_CALL(context(), get_current_market_state)
      .Times(1)
      .WillOnce(Return(make_market_state()));

  EXPECT_CALL(price_generator(), generate_px)
      .Times(1)
      .WillOnce(Return(mock::PriceGenerator::make_output(price)));

  EXPECT_CALL(quantity_generator(), generate_random_qty)
      .Times(1)
      .WillOnce(Return(mock::QuantityGenerator::make_output(quantity)));

  EXPECT_CALL(registry(), add).Times(1).WillOnce(Return(true));

  const std::optional<GeneratedMessage> generated = generate();

  ASSERT_TRUE(generated.has_value());
  const auto& message = generated.value();

  EXPECT_EQ(message.message_type, MessageType::NewOrderSingle);
  EXPECT_EQ(message.order_type, OrderType::Option::Limit);
  EXPECT_EQ(message.side, Side::Option::Buy);
  EXPECT_EQ(message.time_in_force, TimeInForce::Option::Day);
  EXPECT_DOUBLE_EQ(message.order_price->value(), price);
  EXPECT_DOUBLE_EQ(message.quantity->value(), quantity);
  EXPECT_EQ(message.client_order_id->value(), order_id);
  EXPECT_EQ(message.party_id->value(), party);
}

TEST_F(Generator_Random_OrderGenerationAlgorithm,
       Generate_Resting_PartyHasNoOrder_MaxMktDepthReached) {
  constexpr auto event = EventType::RestingBuy;
  constexpr int cp_number = 5;
  constexpr int max_mkt_depth = 14;
  constexpr std::size_t current_mkt_depth = 14;
  const std::string party{"CP5"};
  const std::string order_id{"NewOrderID"};

  listing = create_listing(symbol, venue_id, max_mkt_depth);

  EXPECT_CALL(event_generator(), generate_choice_integer)
      .Times(1)
      .WillOnce(Return(to_integer(event)));

  EXPECT_CALL(counterparty_generator(), generate_counterparty_number)
      .Times(1)
      .WillOnce(Return(cp_number));

  EXPECT_CALL(registry(), find_by_owner(Eq(party)))
      .Times(1)
      .WillOnce(Return(std::nullopt));

  EXPECT_CALL(context(), get_current_market_state)
      .Times(1)
      .WillOnce(Return(make_market_state(std::nullopt, current_mkt_depth)));

  const std::optional<GeneratedMessage> generated = generate();

  EXPECT_FALSE(generated.has_value());
}

TEST_F(Generator_Random_OrderGenerationAlgorithm,
       Generate_Resting_PartyHasOrder_PriceModificationAction) {
  constexpr auto event = EventType::RestingSell;
  constexpr auto action = ActionType::PriceModification;
  constexpr int cp_number = 5;
  constexpr OrderPrice old_price{120.3};
  constexpr double new_price = 121.4;
  constexpr Quantity quantity{121.4};
  const PartyId party{"CP5"};

  const ClientOrderId order_id{"RegisteredOrderID"};

  // Note: registered order side is SELL, we have a resting sell event
  const GeneratedOrderData registered_order = make_registered_order(
      order_id, old_price, Side::Option::Buy, quantity, party);

  EXPECT_CALL(event_generator(), generate_choice_integer)
      .Times(1)
      .WillOnce(Return(to_integer(event)));

  EXPECT_CALL(counterparty_generator(), generate_counterparty_number)
      .Times(1)
      .WillOnce(Return(cp_number));

  EXPECT_CALL(registry(), find_by_owner(Eq(party.value())))
      .Times(1)
      .WillOnce(Return(registered_order));

  EXPECT_CALL(resting_order_action_generator(), generate_integer)
      .Times(1)
      .WillOnce(Return(to_integer(action)));

  EXPECT_CALL(context(), get_current_market_state)
      .Times(1)
      .WillOnce(Return(make_market_state()));

  EXPECT_CALL(price_generator(), generate_px)
      .Times(1)
      .WillOnce(Return(mock::PriceGenerator::make_output(new_price)));

  EXPECT_CALL(quantity_generator(), generate_random_qty).Times(0);

  EXPECT_CALL(registry(), update_by_owner(Eq(party.value()), testing::_))
      .Times(1)
      .WillOnce(Return(true));

  const std::optional<GeneratedMessage> generated = generate();

  ASSERT_TRUE(generated.has_value());
  const auto& message = generated.value();

  EXPECT_EQ(message.message_type, MessageType::OrderCancelReplaceRequest);
  EXPECT_EQ(message.order_type, OrderType::Option::Limit);
  EXPECT_EQ(message.side,
            Side::Option::Buy);  // Side is same as in stored order
  EXPECT_EQ(message.time_in_force, TimeInForce::Option::Day);
  EXPECT_DOUBLE_EQ(message.order_price->value(), new_price);
  EXPECT_DOUBLE_EQ(message.quantity->value(), quantity.value());
  EXPECT_EQ(message.client_order_id, order_id);
  EXPECT_EQ(message.orig_client_order_id->value(), order_id.value());
  EXPECT_EQ(message.party_id, party);
}

TEST_F(Generator_Random_OrderGenerationAlgorithm,
       Generate_Resting_PartyHasOrder_QtyModificationAction) {
  constexpr auto event = EventType::RestingSell;
  constexpr auto action = ActionType::QuantityModification;
  constexpr int cp_number = 5;
  constexpr OrderPrice price{121.4};
  constexpr Quantity old_quantity{121.4};
  constexpr double new_quantity = 121.5;
  const PartyId party{"CP5"};

  const ClientOrderId order_id{"RegisteredOrderID"};

  const GeneratedOrderData registered_order = make_registered_order(
      order_id, price, Side::Option::Sell, old_quantity, party);

  EXPECT_CALL(event_generator(), generate_choice_integer)
      .Times(1)
      .WillOnce(Return(to_integer(event)));

  EXPECT_CALL(counterparty_generator(), generate_counterparty_number)
      .Times(1)
      .WillOnce(Return(cp_number));

  EXPECT_CALL(registry(), find_by_owner(Eq(party.value())))
      .Times(1)
      .WillOnce(Return(registered_order));

  EXPECT_CALL(resting_order_action_generator(), generate_integer)
      .Times(1)
      .WillOnce(Return(to_integer(action)));

  EXPECT_CALL(context(), get_current_market_state)
      .Times(1)
      .WillOnce(Return(make_market_state()));

  EXPECT_CALL(price_generator(), generate_px).Times(0);

  EXPECT_CALL(quantity_generator(), generate_random_qty)
      .Times(1)
      .WillOnce(Return(mock::QuantityGenerator::make_output(new_quantity)));

  EXPECT_CALL(registry(), update_by_owner(Eq(party.value()), testing::_))
      .Times(1)
      .WillOnce(Return(true));

  const std::optional<GeneratedMessage> generated = generate();

  ASSERT_TRUE(generated.has_value());
  const auto& message = generated.value();

  EXPECT_EQ(message.message_type, MessageType::OrderCancelReplaceRequest);
  EXPECT_EQ(message.order_type, OrderType::Option::Limit);
  EXPECT_EQ(message.side, Side::Option::Sell);
  EXPECT_EQ(message.time_in_force, TimeInForce::Option::Day);
  EXPECT_DOUBLE_EQ(message.order_price->value(), price.value());
  EXPECT_DOUBLE_EQ(message.quantity->value(), new_quantity);
  EXPECT_EQ(message.client_order_id, order_id);
  EXPECT_EQ(message.orig_client_order_id->value(), order_id.value());
  EXPECT_EQ(message.party_id, party);
}

TEST_F(Generator_Random_OrderGenerationAlgorithm,
       Generate_Resting_PartyHasOrder_Cancellation) {
  constexpr auto event = EventType::RestingBuy;
  constexpr auto action = ActionType::Cancellation;
  constexpr int cp_number = 5;
  constexpr OrderPrice price{121.4};
  constexpr Quantity quantity{121.5};
  const PartyId party{"CP5"};

  const ClientOrderId order_id{"RegisteredOrderID"};

  const GeneratedOrderData registered_order = make_registered_order(
      order_id, price, Side::Option::Buy, quantity, party);

  EXPECT_CALL(event_generator(), generate_choice_integer)
      .Times(1)
      .WillOnce(Return(to_integer(event)));

  EXPECT_CALL(counterparty_generator(), generate_counterparty_number)
      .Times(1)
      .WillOnce(Return(cp_number));

  EXPECT_CALL(registry(), find_by_owner(Eq(party.value())))
      .Times(1)
      .WillOnce(Return(registered_order));

  EXPECT_CALL(resting_order_action_generator(), generate_integer)
      .Times(1)
      .WillOnce(Return(to_integer(action)));

  EXPECT_CALL(context(), get_current_market_state)
      .Times(1)
      .WillOnce(Return(make_market_state()));

  EXPECT_CALL(price_generator(), generate_px).Times(0);

  EXPECT_CALL(quantity_generator(), generate_random_qty).Times(0);

  EXPECT_CALL(registry(), remove_by_owner(Eq(party.value())))
      .Times(1)
      .WillOnce(Return(true));

  const std::optional<GeneratedMessage> generated = generate();

  ASSERT_TRUE(generated.has_value());
  const auto& message = generated.value();

  EXPECT_EQ(message.message_type, MessageType::OrderCancelRequest);
  EXPECT_EQ(message.order_type, OrderType::Option::Limit);
  EXPECT_EQ(message.side, Side::Option::Buy);
  EXPECT_EQ(message.time_in_force, TimeInForce::Option::Day);
  EXPECT_EQ(message.client_order_id, order_id);
  EXPECT_EQ(message.orig_client_order_id->value(), order_id.value());
  EXPECT_EQ(message.party_id, party);
}

}  // namespace
}  // namespace simulator::generator::random::test