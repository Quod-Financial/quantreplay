#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <quickfix/Session.h>
#include <quickfix/SessionID.h>

#include "common/mapping/setting/mapping_settings.hpp"

namespace simulator::fix::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,
// cppcoreguidelines-non-private-member-variables-in-classes)

class MappingSettings : public Test {
 public:
  const FIX::SessionID SessionID{"FIX.4.4", "Sender", "Target"};
  std::vector<FIX::Session*> sessions;

  auto create_session(FIX::SessionID session_id)
      -> std::unique_ptr<FIX::Session> {
    return std::make_unique<FIX::Session>(application_,
                                          memory_store_factory_,
                                          session_id,
                                          data_dictionary_provider_,
                                          time_range_,
                                          0,
                                          nullptr);
  }

 protected:
  FIX::NullApplication application_;
  FIX::MemoryStoreFactory memory_store_factory_;
  FIX::DataDictionaryProvider data_dictionary_provider_;
  FIX::TimeRange time_range_{FIX::UtcTimeOnly{0, 0, 0},
                             FIX::UtcTimeOnly{0, 0, 0}};
};

TEST_F(MappingSettings, ReturnsMillisecondsPrecisionIfNoSessionIDFound) {
  const simulator::fix::MappingSettings mapping_settings{sessions};

  const auto setting = mapping_settings.get_setting(SessionID);
  ASSERT_THAT(setting.timestamp_precision,
              Eq(TimestampPrecision::Milliseconds));
}

TEST_F(MappingSettings, ReturnsTimeprecisionFromSession) {
  auto session = create_session(SessionID);
  session->setTimestampPrecision(6);
  sessions.push_back(session.get());
  const simulator::fix::MappingSettings mapping_settings{sessions};

  const auto setting = mapping_settings.get_setting(SessionID);
  ASSERT_THAT(setting.timestamp_precision,
              Eq(TimestampPrecision::Microseconds));
}

// NOLINTEND(*magic-numbers*,
// cppcoreguidelines-non-private-member-variables-in-classes)

}  // namespace
}  // namespace simulator::fix::test