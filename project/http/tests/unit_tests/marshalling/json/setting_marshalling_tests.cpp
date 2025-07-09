#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <initializer_list>
#include <string>
#include <string_view>

#include "data_layer/api/models/setting.hpp"
#include "ih/marshalling/json/setting.hpp"

namespace simulator::http::json::test {
namespace {

using namespace ::testing;

class HttpJsonSettingMarshaller : public ::testing::Test {
 public:
  using Setting = data_layer::Setting;
  using Patch = Setting::Patch;

  static auto make_default_patch() -> Patch {
    return Patch{}.with_key("dummy");
  }

  static auto marshall(std::initializer_list<Setting> settings) -> std::string {
    return SettingMarshaller::marshall(settings);
  }
};

TEST_F(HttpJsonSettingMarshaller, MarshallsKey) {
  const auto patch = make_default_patch().with_key("MyKey");
  const auto setting = Setting::create(patch);

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("settings":[)"
      "{"
        R"("key":"MyKey")"
      "}"
    "]"
  "}";
  // clang-format on

  EXPECT_EQ(marshall({setting}), expected_json);
}

TEST_F(HttpJsonSettingMarshaller, MarshallsValue) {
  const auto patch = make_default_patch().with_value("MyValue");
  const auto setting = Setting::create(patch);

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("settings":[)"
      "{"
        R"("key":"dummy",)"
        R"("value":"MyValue")"
      "}"
    "]"
  "}";
  // clang-format on

  EXPECT_EQ(marshall({setting}), expected_json);
}

TEST_F(HttpJsonSettingMarshaller, MarshallsMultipleSettings) {
  const auto setting1 = Setting::create(make_default_patch().with_key("Key1"));
  const auto setting2 = Setting::create(make_default_patch().with_key("Key2"));

  // clang-format off
  constexpr std::string_view expected_json = "{"
    R"("settings":[)"
      "{"
        R"("key":"Key1")"
            "},"
      "{"
        R"("key":"Key2")"
      "}"
    "]"
  "}";
  // clang-format on

  EXPECT_EQ(marshall({setting1, setting2}), expected_json);
}

struct HttpJsonSettingUnmarshaller : public ::testing::Test {
  std::vector<data_layer::Setting::Patch> result;
};

TEST_F(HttpJsonSettingUnmarshaller,
       ThrowsExceptionWhenUnmarshallingInvalidJson) {
  constexpr std::string_view json = R"({"key": "KEY",})";

  ASSERT_THROW(SettingUnmarshaller::unmarshall(json, result),
               std::runtime_error);
}

TEST_F(HttpJsonSettingUnmarshaller, ThrowsExceptionWhenUnmarshallingEmptyJson) {
  constexpr std::string_view json = "{}";

  ASSERT_THROW(SettingUnmarshaller::unmarshall(json, result),
               std::runtime_error);
}

TEST_F(HttpJsonSettingUnmarshaller,
       ThrowsExceptionWhenUnmarshallingUnexpectedJson) {
  constexpr std::string_view json = R"({"settings":"Value"})";

  ASSERT_THROW(SettingUnmarshaller::unmarshall(json, result),
               std::runtime_error);
}

TEST_F(HttpJsonSettingUnmarshaller, UnmarshallsValidJson) {
  // clang-format off
  constexpr std::string_view json = "{"
    R"("settings":[)"
      "{"
        R"("key":"Key1",)"
        R"("value":"Value1")"
      "},"
      "{"
        R"("key":"Key2",)"
        R"("value":"Value2")"
      "}"
    "]"
  "}";
  // clang-format on

  ASSERT_NO_THROW(SettingUnmarshaller::unmarshall(json, result));

  ASSERT_EQ(result.size(), 2);
  ASSERT_THAT(result.at(0).key(), Optional(Eq("Key1")));
  ASSERT_THAT(result.at(0).value(), Optional(Eq("Value1")));
  ASSERT_THAT(result.at(1).key(), Optional(Eq("Key2")));
  ASSERT_THAT(result.at(1).value(), Optional(Eq("Value2")));
}

}  // namespace
}  // namespace simulator::http::json::test
