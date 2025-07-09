#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

#include "api/models/setting.hpp"
#include "ih/common/exceptions.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

TEST(DataLayer_Model_Setting, Patch_Set_Key) {
  Setting::Patch patch{};
  ASSERT_FALSE(patch.key().has_value());

  patch.with_key("Name");
  EXPECT_THAT(patch.key(), Optional(Eq("Name")));
}

TEST(DataLayer_Model_Setting, Patch_Set_Value) {
  Setting::Patch patch{};
  ASSERT_FALSE(patch.value().has_value());

  patch.with_value("SiteName");
  EXPECT_THAT(patch.value(), Optional(Eq("SiteName")));
}

TEST(DataLayer_Model_Setting, Create_WithoutKey) {
  Setting::Patch patch{};
  ASSERT_FALSE(patch.key().has_value());

  EXPECT_THROW((void)Setting::create(patch), RequiredAttributeMissing);
}

TEST(DataLayer_Model_Setting, Get_Key) {
  Setting::Patch patch{};
  patch.with_key("MyKey");

  Setting setting = Setting::create(patch);
  EXPECT_EQ(setting.key(), "MyKey");
}

TEST(DataLayer_Model_Setting, Get_Value_Missing) {
  Setting::Patch patch{};
  patch.with_key("MyKey");

  Setting setting = Setting::create(patch);
  EXPECT_EQ(setting.value(), std::nullopt);
}

TEST(DataLayer_Model_Setting, Get_Value_Specified) {
  Setting::Patch patch{};
  patch.with_key("MyKey");
  patch.with_value("MyValue");

  Setting setting = Setting::create(patch);
  EXPECT_THAT(setting.value(), Optional(Eq("MyValue")));
}

}  // namespace
}  // namespace simulator::data_layer::test