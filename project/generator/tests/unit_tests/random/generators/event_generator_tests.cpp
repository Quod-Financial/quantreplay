#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "ih/random/generators/event_generator.hpp"
#include "ih/random/values/event.hpp"
#include "mocks/random/value_generator.hpp"

namespace simulator::generator::random::test {
namespace {

using namespace ::testing;

class Generator_Random_EventGenerator : public testing::Test {
 public:
  static constexpr auto to_integer(random::Event::Type event) -> std::uint64_t {
    return static_cast<std::uint64_t>(event);
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

TEST_F(Generator_Random_EventGenerator, CheckRequestedLimit) {
  constexpr std::uint64_t expected_min = 0;
  constexpr std::uint64_t expected_max = 29;

  auto generator = random::EventGeneratorImpl::create(value_generator_ptr());
  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::uint64_t>(Eq(expected_min)),
                               Matcher<std::uint64_t>(Eq(expected_max))))
      .Times(1)
      .WillOnce(Return(0));

  [[maybe_unused]] const auto event = generator->generate_event();
}

TEST_F(Generator_Random_EventGenerator, Generate_RestingBuy) {
  constexpr auto random = to_integer(random::Event::Type::RestingBuy);

  auto generator = random::EventGeneratorImpl::create(value_generator_ptr());
  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::uint64_t>(), A<std::uint64_t>()))
      .Times(1)
      .WillOnce(Return(random));

  const random::Event event = generator->generate_event();
  EXPECT_EQ(event.get_type(), random::Event::Type::RestingBuy);
}

TEST_F(Generator_Random_EventGenerator, Generate_RestingSell) {
  constexpr auto random = to_integer(random::Event::Type::RestingSell);

  auto generator = random::EventGeneratorImpl::create(value_generator_ptr());
  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::uint64_t>(), A<std::uint64_t>()))
      .Times(1)
      .WillOnce(Return(random));

  const random::Event event = generator->generate_event();
  EXPECT_EQ(event.get_type(), random::Event::Type::RestingSell);
}

TEST_F(Generator_Random_EventGenerator, Generate_AggressiveBuy) {
  constexpr auto random = to_integer(random::Event::Type::AggressiveBuy);

  auto generator = random::EventGeneratorImpl::create(value_generator_ptr());
  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::uint64_t>(), A<std::uint64_t>()))
      .Times(1)
      .WillOnce(Return(random));

  const random::Event event = generator->generate_event();
  EXPECT_EQ(event.get_type(), random::Event::Type::AggressiveBuy);
}

TEST_F(Generator_Random_EventGenerator, Generate_AggressiveSell) {
  constexpr auto random = to_integer(random::Event::Type::AggressiveSell);

  auto generator = random::EventGeneratorImpl::create(value_generator_ptr());
  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::uint64_t>(), A<std::uint64_t>()))
      .Times(1)
      .WillOnce(Return(random));

  const random::Event event = generator->generate_event();
  EXPECT_EQ(event.get_type(), random::Event::Type::AggressiveSell);
}

TEST_F(Generator_Random_EventGenerator, Generate_NoOperation) {
  constexpr auto random = to_integer(random::Event::Type::NoOperation);

  auto generator = random::EventGeneratorImpl::create(value_generator_ptr());
  EXPECT_CALL(value_generator(),
              generate_uniform(A<std::uint64_t>(), A<std::uint64_t>()))
      .Times(1)
      .WillOnce(Return(random));

  const random::Event event = generator->generate_event();
  EXPECT_EQ(event.get_type(), random::Event::Type::NoOperation);
}

}  // namespace
}  // namespace simulator::generator::random::test