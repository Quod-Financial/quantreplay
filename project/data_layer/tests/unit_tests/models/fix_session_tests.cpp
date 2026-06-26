#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <optional>
#include <string>

#include "api/models/fix_session.hpp"
#include "core/tools/time.hpp"
#include "ih/common/exceptions.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

[[nodiscard]]
auto sample_last_connected_time() -> core::sys_us {
  using namespace std::chrono_literals;
  return std::chrono::sys_days{2026y / std::chrono::May / 6} + 11h + 31min + 40s + 904900us;
}

struct DataLayerModelsFixSessionPatch : public Test {
  FixSession::Patch patch;
};

TEST_F(DataLayerModelsFixSessionPatch, DoesNotContainDefaultValues) {
  ASSERT_FALSE(patch.venue_id().has_value());
  ASSERT_FALSE(patch.session_id().has_value());
  ASSERT_FALSE(patch.last_connected_time().has_value());
}

TEST_F(DataLayerModelsFixSessionPatch, SetsVenueId) {
  patch.with_venue_id("LSE");
  EXPECT_THAT(patch.venue_id(), Optional(Eq("LSE")));
}

TEST_F(DataLayerModelsFixSessionPatch, SetsSessionId) {
  patch.with_session_id("FIX.4.4:CLIENT->SERVER");
  EXPECT_THAT(patch.session_id(), Optional(Eq("FIX.4.4:CLIENT->SERVER")));
}

TEST_F(DataLayerModelsFixSessionPatch, SetsLastConnectedTime) {
  patch.with_last_connected_time(sample_last_connected_time());
  EXPECT_THAT(patch.last_connected_time(),
              Optional(Eq(sample_last_connected_time())));
}

struct DataLayerModelsFixSession : public Test {
  FixSession::Patch patch;

  static auto fill_required_fields(FixSession::Patch& patch) -> void {
    patch.with_venue_id("dummy").with_session_id("dummy");
  }
};

TEST_F(DataLayerModelsFixSession, ThrowsExceptionIfVenueIdIsNotSet) {
  patch.with_session_id("dummy");

  ASSERT_FALSE(patch.venue_id().has_value());

  EXPECT_THROW((void)FixSession::create(patch), RequiredAttributeMissing);
}

TEST_F(DataLayerModelsFixSession, ThrowsExceptionIfSessionIdIsNotSet) {
  patch.with_venue_id("dummy");

  ASSERT_FALSE(patch.session_id().has_value());

  EXPECT_THROW((void)FixSession::create(patch), RequiredAttributeMissing);
}

TEST_F(DataLayerModelsFixSession, StoresVenueId) {
  fill_required_fields(patch);
  patch.with_venue_id("LSE");

  const auto fix_session = FixSession::create(patch);
  EXPECT_EQ(fix_session.venue_id(), "LSE");
}

TEST_F(DataLayerModelsFixSession, StoresSessionId) {
  fill_required_fields(patch);
  patch.with_session_id("FIX.4.4:CLIENT->SERVER");

  const auto fix_session = FixSession::create(patch);
  EXPECT_EQ(fix_session.session_id(), "FIX.4.4:CLIENT->SERVER");
}

TEST_F(DataLayerModelsFixSession, StoresNullOptionalLastConnectedTimeIfNotSet) {
  fill_required_fields(patch);

  const auto fix_session = FixSession::create(patch);
  EXPECT_EQ(fix_session.last_connected_time(), std::nullopt);
}

TEST_F(DataLayerModelsFixSession, StoresLastConnectedTime) {
  fill_required_fields(patch);
  patch.with_last_connected_time(sample_last_connected_time());

  const auto fix_session = FixSession::create(patch);
  EXPECT_THAT(fix_session.last_connected_time(),
              Optional(Eq(sample_last_connected_time())));
}

}  // namespace
}  // namespace simulator::data_layer::test
