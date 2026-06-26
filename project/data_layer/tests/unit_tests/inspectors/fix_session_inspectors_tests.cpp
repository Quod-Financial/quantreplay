#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>

#include "api/inspectors/fix_session.hpp"
#include "api/models/fix_session.hpp"
#include "common/marshaller.hpp"
#include "core/tools/time.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;  // NOLINT

[[nodiscard]]
auto sample_last_connected_time() -> core::sys_us {
  using namespace std::chrono;
  return sys_days{2026y / May / 6} + 11h + 31min + 40s + 904900us;
}

class DataLayerFixSessionReader : public Test {
 public:
  using MarshallerType = Marshaller<FixSession>;
  using ReaderType = FixSessionReader<MarshallerType>;

  using Attribute = FixSession::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  static auto make_default_patch() -> FixSession::Patch {
    return FixSession::Patch{}
        .with_venue_id("dummy_venue")
        .with_session_id("dummy_session");
  }

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), _))
        .Times(AnyNumber());
    EXPECT_CALL(marshaller(), string(Eq(Attribute::SessionId), _))
        .Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerFixSessionReader, ReadsVenueId) {
  const auto patch = make_default_patch().with_venue_id("VenueA");
  const auto fix_session = FixSession::create(patch);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("VenueA")))
      .Times(1);
  make_reader().read(fix_session);
}

TEST_F(DataLayerFixSessionReader, ReadsSessionId) {
  const auto patch = make_default_patch().with_session_id("SessionA");
  const auto fix_session = FixSession::create(patch);

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SessionId), Eq("SessionA")))
      .Times(1);
  make_reader().read(fix_session);
}

TEST_F(DataLayerFixSessionReader, ReadsLastConnectedTime) {
  const auto patch = make_default_patch().with_last_connected_time(
      sample_last_connected_time());
  const auto fix_session = FixSession::create(patch);

  EXPECT_CALL(marshaller(),
              timestamp(Eq(Attribute::LastConnectedTime),
                        Eq(sample_last_connected_time())))
      .Times(1);
  make_reader().read(fix_session);
}

class DataLayerFixSessionPatchReader : public Test {
 public:
  using MarshallerType = Marshaller<FixSession>;
  using ReaderType = FixSessionPatchReader<MarshallerType>;

  using Attribute = FixSession::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

 protected:
  FixSession::Patch patch;

 private:
  MarshallerType marshaller_;
};

TEST_F(DataLayerFixSessionPatchReader, ReadsVenueId) {
  patch.with_venue_id("VenueA");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::VenueId), Eq("VenueA")))
      .Times(1);
  make_reader().read(patch);
}

TEST_F(DataLayerFixSessionPatchReader, ReadsSessionId) {
  patch.with_session_id("SessionA");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::SessionId), Eq("SessionA")))
      .Times(1);
  make_reader().read(patch);
}

TEST_F(DataLayerFixSessionPatchReader, ReadsLastConnectedTime) {
  patch.with_last_connected_time(sample_last_connected_time());

  EXPECT_CALL(marshaller(),
              timestamp(Eq(Attribute::LastConnectedTime),
                        Eq(sample_last_connected_time())))
      .Times(1);
  make_reader().read(patch);
}

class DataLayerFixSessionPatchWriter : public Test {
 public:
  using UnmarshallerType = Unmarshaller<FixSession>;
  using WriterType = FixSessionPatchWriter<UnmarshallerType>;

  using Attribute = FixSession::Attribute;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(unmarshaller(), string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
    EXPECT_CALL(unmarshaller(), timestamp)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

  FixSession::Patch patch;

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayerFixSessionPatchWriter, WritesVenueId) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::VenueId), _))
      .WillOnce(DoAll(SetArgReferee<1>("VenueA"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.venue_id(), Optional(Eq("VenueA")));
}

TEST_F(DataLayerFixSessionPatchWriter, WritesSessionId) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::SessionId), _))
      .WillOnce(DoAll(SetArgReferee<1>("SessionA"), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.session_id(), Optional(Eq("SessionA")));
}

TEST_F(DataLayerFixSessionPatchWriter, WritesLastConnectedTime) {
  EXPECT_CALL(unmarshaller(), timestamp(Eq(Attribute::LastConnectedTime), _))
      .WillOnce(
          DoAll(SetArgReferee<1>(sample_last_connected_time()), Return(true)));

  make_writer().write(patch);
  EXPECT_THAT(patch.last_connected_time(),
              Optional(Eq(sample_last_connected_time())));
}

}  // namespace
}  // namespace simulator::data_layer::test
