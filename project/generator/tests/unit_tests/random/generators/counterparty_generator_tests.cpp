#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string_view>

#include "ih/random/generators/counterparty_generator.hpp"
#include "mocks/random/value_generator.hpp"

namespace simulator::generator::random::test {
namespace {

using namespace ::testing;

class GeneratorRandomCounterpartyGenerator : public testing::Test {
 public:
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

TEST_F(GeneratorRandomCounterpartyGenerator, CheckRequestedLimit) {
  using RandomInt = std::uint64_t;

  constexpr RandomInt min_counterparty_number = 1;
  constexpr RandomInt max_counterparties = 121;

  auto generator = random::CounterpartyGeneratorImpl::create(
      max_counterparties, value_generator_ptr());

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<RandomInt>(Eq(min_counterparty_number)),
                               Matcher<RandomInt>(Eq(max_counterparties))))
      .Times(1)
      .WillOnce(Return(1));

  [[maybe_unused]] const auto party = generator->generate_counterparty();
}

TEST_F(GeneratorRandomCounterpartyGenerator, GeneratesPartyId) {
  using RandomInt = std::uint64_t;

  constexpr RandomInt max_counterparties = 121;
  constexpr RandomInt random_integer = 21;
  const PartyId expected_party_id{"CP21"};

  auto generator = random::CounterpartyGeneratorImpl::create(
      max_counterparties, value_generator_ptr());

  EXPECT_CALL(value_generator(),
              generate_uniform(A<RandomInt>(), A<RandomInt>()))
      .Times(1)
      .WillOnce(Return(random_integer));

  const auto party = generator->generate_counterparty();
  ASSERT_EQ(party.party_id(), expected_party_id);
}

TEST_F(GeneratorRandomCounterpartyGenerator, GeneratesPartyRoleExecutingFirm) {
  using RandomInt = std::uint64_t;

  constexpr RandomInt max_counterparties = 121;

  auto generator = random::CounterpartyGeneratorImpl::create(
      max_counterparties, value_generator_ptr());

  EXPECT_CALL(value_generator(),
              generate_uniform(A<RandomInt>(), A<RandomInt>()))
      .Times(1)
      .WillOnce(Return(42));

  const auto party = generator->generate_counterparty();
  ASSERT_EQ(party.role(), PartyRole::Option::ExecutingFirm);
}

TEST_F(GeneratorRandomCounterpartyGenerator,
       GeneratesPartyIdSourceProprietary) {
  using RandomInt = std::uint64_t;

  constexpr RandomInt max_counterparties = 121;

  auto generator = random::CounterpartyGeneratorImpl::create(
      max_counterparties, value_generator_ptr());

  EXPECT_CALL(value_generator(),
              generate_uniform(A<RandomInt>(), A<RandomInt>()))
      .Times(1)
      .WillOnce(Return(42));

  const auto party = generator->generate_counterparty();
  ASSERT_EQ(party.source(), PartyIdSource::Option::Proprietary);
}

}  // namespace
}  // namespace simulator::generator::random::test