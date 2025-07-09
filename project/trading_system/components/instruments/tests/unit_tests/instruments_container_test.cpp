#include "ih/instruments_container.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "test_utils/utils.hpp"

namespace simulator::trading_system::instrument::test {
namespace {

using namespace ::testing;

struct InstrumentsInstrumentsContainer : public Test {
  Container container;
};

// NOLINTBEGIN(*-magic-numbers)

TEST_F(InstrumentsInstrumentsContainer, CreateContainer) {
  EXPECT_EQ(container.size(), 0);
  EXPECT_EQ(container.begin(), container.end());
}

TEST_F(InstrumentsInstrumentsContainer, EmplaceInstrumentIntoEmptyContainer) {
  const auto inserted_it = container.emplace(make_instrument(InstrumentId{42}));

  EXPECT_NE(inserted_it, container.end());
  EXPECT_THAT(
      container,
      ElementsAre(Field(&Instrument::identifier, Eq(InstrumentId{42}))));
}

TEST_F(InstrumentsInstrumentsContainer,
       EmplaceInstrumentIntoNonEmptyContainer) {
  container.emplace(make_instrument(InstrumentId{41}));
  container.emplace(make_instrument(InstrumentId{43}));
  ASSERT_EQ(container.size(), 2);

  const auto inserted_it = container.emplace(make_instrument(InstrumentId{42}));

  EXPECT_NE(inserted_it, container.end());
  EXPECT_THAT(
      container,
      ElementsAre(Field(&Instrument::identifier, Eq(InstrumentId{41})),
                  Field(&Instrument::identifier, Eq(InstrumentId{42})),
                  Field(&Instrument::identifier, Eq(InstrumentId{43}))));
}

TEST_F(InstrumentsInstrumentsContainer,
       DoesNotEmplaceInstrumentWithDuplicatedId) {
  container.emplace(make_instrument(InstrumentId{41}));
  container.emplace(make_instrument(InstrumentId{42}));
  container.emplace(make_instrument(InstrumentId{43}));
  ASSERT_EQ(container.size(), 3);

  const auto inserted_it = container.emplace(make_instrument(InstrumentId{42}));

  EXPECT_EQ(inserted_it, container.end());
  EXPECT_THAT(
      container,
      ElementsAre(Field(&Instrument::identifier, Eq(InstrumentId{41})),
                  Field(&Instrument::identifier, Eq(InstrumentId{42})),
                  Field(&Instrument::identifier, Eq(InstrumentId{43}))));
}

TEST_F(InstrumentsInstrumentsContainer, FindExistingInstrumentByIdentifier) {
  container.emplace(make_instrument(InstrumentId{41}));
  container.emplace(make_instrument(InstrumentId{42}));
  container.emplace(make_instrument(InstrumentId{43}));
  ASSERT_EQ(container.size(), 3);

  const auto found_it = container.find_by_identifier(InstrumentId{42});

  ASSERT_NE(found_it, container.end());
  EXPECT_EQ(found_it->identifier, InstrumentId{42});
}

TEST_F(InstrumentsInstrumentsContainer,
       DoesNotFindNonexistentInstrumentByIdentifier) {
  container.emplace(make_instrument(InstrumentId{41}));
  container.emplace(make_instrument(InstrumentId{43}));
  ASSERT_EQ(container.size(), 2);

  const auto found_it = container.find_by_identifier(InstrumentId{42});

  EXPECT_EQ(found_it, container.end());
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::trading_system::instrument::test