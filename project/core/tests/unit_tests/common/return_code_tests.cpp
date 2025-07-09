#include <fmt/format.h>
#include <gtest/gtest.h>

#include <sstream>
#include <utility>

#include "core/common/return_code.hpp"
#include "fixtures/given_expected_fixture.hpp"

namespace simulator::core::code::test {
namespace {

using namespace testing;  // NOLINT

struct CoreMarketStateStoreFormatting
    : public core::test::GivenExpectedFixture<StoreMarketState, std::string> {};

TEST_P(CoreMarketStateStoreFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreMarketStateStoreFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreMarketStateStoreFormatting,
    Values(std::make_pair(static_cast<StoreMarketState>(0xFF), "undefined"),
           std::make_pair(StoreMarketState::Stored, "Stored"),
           std::make_pair(StoreMarketState::PersistenceDisabled, "PersistenceDisabled"),
           std::make_pair(StoreMarketState::PersistenceFilePathIsEmpty, "PersistenceFilePathIsEmpty"),
           std::make_pair(StoreMarketState::PersistenceFilePathIsUnreachable, "PersistenceFilePathIsUnreachable"),
           std::make_pair(StoreMarketState::ErrorWhenOpeningPersistenceFile, "ErrorWhenOpeningPersistenceFile"),
           std::make_pair(StoreMarketState::ErrorWhenWritingToPersistenceFile, "ErrorWhenWritingToPersistenceFile")));
// clang-format on

struct CoreMarketStateRecoverFormatting
    : public core::test::GivenExpectedFixture<RecoverMarketState, std::string> {
};

TEST_P(CoreMarketStateRecoverFormatting, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

TEST_P(CoreMarketStateRecoverFormatting, StreamFormatting) {
  std::stringstream stream;

  stream << given();

  ASSERT_EQ(stream.str(), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    CoreMarketStateRecoverFormatting,
    Values(std::make_pair(static_cast<RecoverMarketState>(0xFF), "undefined"),
                  std::make_pair(RecoverMarketState::Recovered, "Recovered"),
                  std::make_pair(RecoverMarketState::PersistenceDisabled, "PersistenceDisabled"),
                  std::make_pair(RecoverMarketState::PersistenceFilePathIsEmpty, "PersistenceFilePathIsEmpty"),
                  std::make_pair(RecoverMarketState::PersistenceFilePathIsUnreachable, "PersistenceFilePathIsUnreachable"),
                  std::make_pair(RecoverMarketState::ErrorWhenOpeningPersistenceFile, "ErrorWhenOpeningPersistenceFile"),
                  std::make_pair(RecoverMarketState::PersistenceFileIsMalformed, "PersistenceFileIsMalformed")));

}  // namespace
}  // namespace simulator::core::code::test