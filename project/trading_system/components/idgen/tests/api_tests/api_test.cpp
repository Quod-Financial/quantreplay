#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "common/attributes.hpp"
#include "idgen/execution_id.hpp"
#include "idgen/instrument_id.hpp"
#include "idgen/market_entry_id.hpp"
#include "idgen/order_id.hpp"
#include "ih/contexts/execution_id_context.hpp"
#include "ih/contexts/instrument_id_context.hpp"
#include "ih/contexts/market_entry_id_context.hpp"
#include "ih/contexts/order_id_context.hpp"

namespace simulator::trading_system::idgen::test {

namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*-magic-numbers)

TEST(Idgen, CreateInstrumentIdGenerationContext) {
  EXPECT_NO_THROW((void)make_instrument_id_generation_ctx());
}

TEST(Idgen, RetrieveBadInstrumentIdGenerationContextImplementation) {
  InstrumentIdContext bad_ctx{
      std::unique_ptr<InstrumentIdContext::Implementation>{nullptr}};

  EXPECT_DEATH_IF_SUPPORTED((void)bad_ctx.implementation(), "");
}

TEST(Idgen, GenerateInstrumentIdentifier) {
  auto context = make_instrument_id_generation_ctx();

  const ExpectedId<InstrumentId> identifier = generate_new_id(context);
  ASSERT_TRUE(identifier.has_value());

  EXPECT_EQ(*identifier, InstrumentId{1});
}

TEST(Idgen, GenerateSeveralInstrumentIdentifiers) {
  auto context = make_instrument_id_generation_ctx();

  const ExpectedId<InstrumentId> first_identifier = generate_new_id(context);
  ASSERT_TRUE(first_identifier.has_value());
  const ExpectedId<InstrumentId> second_identifier = generate_new_id(context);
  ASSERT_TRUE(second_identifier.has_value());
  const ExpectedId<InstrumentId> third_identifier = generate_new_id(context);
  ASSERT_TRUE(third_identifier.has_value());

  EXPECT_EQ(*first_identifier, InstrumentId{1});
  EXPECT_EQ(*second_identifier, InstrumentId{2});
  EXPECT_EQ(*third_identifier, InstrumentId{3});
}

TEST(Idgen, ResetInstrumentIdGenerationContext) {
  auto context = make_instrument_id_generation_ctx();

  (void)generate_new_id(context);  // move context from initial state
  const ExpectedId<InstrumentId> initial_id = generate_new_id(context);
  ASSERT_TRUE(initial_id.has_value());

  reset_generation_ctx(context);
  const ExpectedId<InstrumentId> refreshed_id = generate_new_id(context);
  ASSERT_TRUE(refreshed_id.has_value());

  EXPECT_EQ(*initial_id, InstrumentId{2});
  EXPECT_EQ(*refreshed_id, InstrumentId{1});
}

TEST(Idgen, CreateOrderIdGenerationContext) {
  EXPECT_NO_THROW((void)make_order_id_generation_ctx());
}

TEST(Idgen, RetrieveBadOrderIdGenerationContextImplementation) {
  OrderIdContext bad_ctx{
      std::unique_ptr<OrderIdContext::Implementation>{nullptr}};

  EXPECT_DEATH_IF_SUPPORTED((void)bad_ctx.implementation(), "");
}

TEST(Idgen, GenerateSeveralOrderIdentifiers) {
  auto context = make_order_id_generation_ctx();

  const OrderId first_identifier = generate_new_id(context);
  const OrderId second_identifier = generate_new_id(context);

  ASSERT_THAT(first_identifier, Ne(second_identifier));
}

TEST(Idgen, CreateExecutionIdGenerationContext) {
  EXPECT_NO_THROW((void)make_execution_id_generation_ctx(OrderId{123}));
}

TEST(Idgen, RetrieveBadExecutionIdGenerationContextImplementation) {
  ExecutionIdContext bad_ctx{
      std::unique_ptr<ExecutionIdContext::Implementation>{nullptr}};

  EXPECT_DEATH_IF_SUPPORTED((void)bad_ctx.implementation(), "");
}

TEST(Idgen, GenerateExecutionIdentifier) {
  auto context = make_execution_id_generation_ctx(OrderId{123});

  const auto identifier = generate_new_id(context);

  ASSERT_THAT(identifier.has_value(), IsTrue());
  ASSERT_EQ(identifier->value(), "123-1");
}

TEST(Idgen, GenerateSeveralExecutionIdentifiers) {
  auto context = make_execution_id_generation_ctx(OrderId{123});

  const auto first_identifier = generate_new_id(context);
  const auto second_identifier = generate_new_id(context);

  ASSERT_THAT(first_identifier.has_value(), IsTrue());
  ASSERT_THAT(second_identifier.has_value(), IsTrue());
  ASSERT_THAT(first_identifier->value(), Ne(second_identifier->value()));
}

TEST(Idgen, RetrieveBadMarketEntryIdGenerationContextImplementation) {
  MarketEntryIdContext bad_ctx{
      std::unique_ptr<MarketEntryIdContext::Implementation>{nullptr}};

  EXPECT_DEATH_IF_SUPPORTED((void)bad_ctx.implementation(), "");
}

TEST(Idgen, GenerateMarketEntryIdentifier) {
  auto context = make_market_entry_id_generation_ctx();

  ASSERT_THAT(static_cast<std::string>(generate_new_id(context)),
              MatchesRegex("[0-9]+:1"));
}

TEST(Idgen, GenerateSeveralMarketEntryIdentifiers) {
  auto context = make_market_entry_id_generation_ctx();

  EXPECT_THAT(static_cast<std::string>(generate_new_id(context)),
              MatchesRegex("[0-9]+:1"));
  EXPECT_THAT(static_cast<std::string>(generate_new_id(context)),
              MatchesRegex("[0-9]+:2"));
  EXPECT_THAT(static_cast<std::string>(generate_new_id(context)),
              MatchesRegex("[0-9]+:3"));
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::trading_system::idgen::test