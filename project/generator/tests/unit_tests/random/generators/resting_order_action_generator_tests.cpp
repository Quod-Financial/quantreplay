#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "ih/random/generators/resting_order_action_generator.hpp"
#include "ih/random/values/resting_order_action.hpp"
#include "mocks/random/value_generator.hpp"

namespace simulator::generator::random::test {
namespace {

using namespace ::testing;

class Generator_Random_RestingOrderActionGenerator : public testing::Test {
 public:
  using Action = random::RestingOrderAction::Type;

  static constexpr auto to_integer(random::RestingOrderAction::Type action)
      -> std::uint64_t {
    return static_cast<std::uint64_t>(action);
  }

  auto value_generator_ptr() -> std::shared_ptr<mock::ValueGenerator> {
    return value_generator_;
  }

  auto value_generator() -> mock::ValueGenerator& { return *value_generator_; }

 protected:
  auto SetUp() -> void override {
    value_generator_ = std::make_shared<mock::ValueGenerator>();
  }

 private:
  std::shared_ptr<mock::ValueGenerator> value_generator_;
};

TEST_F(Generator_Random_RestingOrderActionGenerator, CheckRequestedLimit) {
  constexpr std::uint64_t expected_min = 0;
  constexpr std::uint64_t expected_max = 19;

  auto generator =
      random::RestingOrderActionGeneratorImpl::create(value_generator_ptr());

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::uint64_t>(Eq(expected_min)),
                               Matcher<std::uint64_t>(Eq(expected_max))))
      .Times(1)
      .WillOnce(Return(0));

  [[maybe_unused]] const auto event = generator->generate_action();
}

TEST_F(Generator_Random_RestingOrderActionGenerator,
       Generate_PriceModification) {
  constexpr auto random = to_integer(Action::PriceModification);

  auto generator =
      random::RestingOrderActionGeneratorImpl::create(value_generator_ptr());

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::uint64_t>(), A<std::uint64_t>()))
      .Times(1)
      .WillOnce(Return(random));

  const random::RestingOrderAction event = generator->generate_action();
  EXPECT_EQ(event.get_action_type(), Action::PriceModification);
}

TEST_F(Generator_Random_RestingOrderActionGenerator,
       Generate_QuantityModification) {
  constexpr auto random = to_integer(Action::QuantityModification);

  auto generator =
      random::RestingOrderActionGeneratorImpl::create(value_generator_ptr());

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::uint64_t>(), A<std::uint64_t>()))
      .Times(1)
      .WillOnce(Return(random));

  const random::RestingOrderAction event = generator->generate_action();
  EXPECT_EQ(event.get_action_type(), Action::QuantityModification);
}

TEST_F(Generator_Random_RestingOrderActionGenerator, Generate_Cancellation) {
  constexpr auto random = to_integer(Action::Cancellation);

  auto generator =
      random::RestingOrderActionGeneratorImpl::create(value_generator_ptr());

  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::uint64_t>(), A<std::uint64_t>()))
      .Times(1)
      .WillOnce(Return(random));

  const random::RestingOrderAction event = generator->generate_action();
  EXPECT_EQ(event.get_action_type(), Action::Cancellation);
}

}  // namespace
}  // namespace simulator::generator::random::test