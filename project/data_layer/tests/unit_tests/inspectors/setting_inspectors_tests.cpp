#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/inspectors/setting.hpp"
#include "api/models/setting.hpp"
#include "common/marshaller.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;  // NOLINT

class DataLayer_Inspectors_SettingReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<Setting>;
  using ReaderType = SettingReader<MarshallerType>;

  using Attribute = Setting::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

  static auto make_default_patch() -> Setting::Patch {
    return Setting::Patch{}.with_key("dummy");
  }

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(marshaller(), string(Eq(Attribute::Key), _)).Times(AnyNumber());
  }

 private:
  MarshallerType marshaller_;
};

class DataLayer_Inspectors_SettingPatchReader : public ::testing::Test {
 public:
  using MarshallerType = Marshaller<Setting>;
  using ReaderType = SettingPatchReader<MarshallerType>;

  using Attribute = Setting::Attribute;

  auto marshaller() -> MarshallerType& { return marshaller_; }

  auto make_reader() -> ReaderType { return ReaderType{marshaller_}; }

 private:
  MarshallerType marshaller_;
};

class DataLayer_Inspectors_SettingPatchWriter : public ::testing::Test {
 public:
  using UnmarshallerType = Unmarshaller<Setting>;
  using WriterType = SettingPatchWriter<UnmarshallerType>;

  using Attribute = Setting::Attribute;

  auto unmarshaller() -> UnmarshallerType& { return unmarshaller_; }

  auto make_writer() -> WriterType { return WriterType{unmarshaller_}; }

 protected:
  auto SetUp() -> void override {
    EXPECT_CALL(unmarshaller(), string)
        .Times(AnyNumber())
        .WillRepeatedly(Return(false));
  }

 private:
  UnmarshallerType unmarshaller_;
};

TEST_F(DataLayer_Inspectors_SettingReader, Read_Key) {
  const auto patch = make_default_patch().with_key("MyKey");
  const auto setting = Setting::create(patch);
  ASSERT_EQ(setting.key(), "MyKey");

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Key), Eq("MyKey"))).Times(1);
  make_reader().read(setting);
}

TEST_F(DataLayer_Inspectors_SettingReader, Read_Value) {
  const auto patch = make_default_patch().with_value("MyValue");
  const auto setting = Setting::create(patch);
  ASSERT_THAT(setting.value(), Optional(Eq("MyValue")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Value), Eq("MyValue")))
      .Times(1);
  make_reader().read(setting);
}

TEST_F(DataLayer_Inspectors_SettingPatchReader, Read_Key) {
  Setting::Patch patch{};
  patch.with_key("Name");
  ASSERT_THAT(patch.key(), Optional(Eq("Name")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Key), Eq("Name"))).Times(1);
  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_SettingPatchReader, Read_Value) {
  Setting::Patch patch{};
  patch.with_value("Val");
  ASSERT_THAT(patch.value(), Optional(Eq("Val")));

  EXPECT_CALL(marshaller(), string(Eq(Attribute::Value), Eq("Val"))).Times(1);
  make_reader().read(patch);
}

TEST_F(DataLayer_Inspectors_SettingPatchWriter, Write_Key) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Key), _))
      .WillOnce(DoAll(SetArgReferee<1>("Name"), Return(true)));

  Setting::Patch patch{};
  make_writer().write(patch);
  EXPECT_THAT(patch.key(), Optional(Eq("Name")));
}

TEST_F(DataLayer_Inspectors_SettingPatchWriter, Write_Value) {
  EXPECT_CALL(unmarshaller(), string(Eq(Attribute::Value), _))
      .WillOnce(DoAll(SetArgReferee<1>("Val"), Return(true)));

  Setting::Patch patch{};
  make_writer().write(patch);
  EXPECT_THAT(patch.value(), Optional(Eq("Val")));
}

}  // namespace
}  // namespace simulator::data_layer::test