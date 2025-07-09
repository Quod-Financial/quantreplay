#include <gtest/gtest.h>
#include <rapidjson/document.h>

#include <limits>
#include <stdexcept>
#include <string>

#include "ih/marshalling/json/detail/unmarshaller.hpp"

namespace simulator::http::json::test {
namespace {

class HttpJsonUnmarshaller : public ::testing::Test {
 public:
  using Venue = data_layer::Venue;
  constexpr inline static auto Attribute = Venue::Attribute::VenueId;
  constexpr inline static const char* Key = "id";

  template <typename T>
  auto add_member(const char* key, T value, rapidjson::Document& doc) -> void {
    if (!doc.IsObject()) {
      doc.SetObject();
    }

    rapidjson::Value json_key{key, doc.GetAllocator()};
    rapidjson::Value json_value{std::forward<T>(value)};
    doc.AddMember(json_key, json_value, doc.GetAllocator());
  }

  auto add_member(const char* key,
                  const std::string& value,
                  rapidjson::Document& doc) -> void {
    if (!doc.IsObject()) {
      doc.SetObject();
    }

    rapidjson::Value json_key{key, doc.GetAllocator()};
    rapidjson::Value json_value{value.data(), doc.GetAllocator()};
    doc.AddMember(json_key, json_value, doc.GetAllocator());
  }

  rapidjson::Document document;
};

TEST_F(HttpJsonUnmarshaller,
       ThrowsExceptionWhenUnmarshallingWithNonObjectValue) {
  ASSERT_FALSE(document.IsObject());

  ASSERT_THROW(Unmarshaller{document}, std::logic_error);
}

TEST_F(HttpJsonUnmarshaller, CreatesWithObjectValue) {
  document.SetObject();
  ASSERT_TRUE(document.IsObject());

  EXPECT_NO_THROW(Unmarshaller{document});
}

TEST_F(HttpJsonUnmarshaller,
       ThrowsExceptionWhenUnmarshallingBadAttributeValue) {
  constexpr auto bad_attribute = static_cast<Venue::Attribute>(-1);

  add_member(Key, true, document);

  Unmarshaller unmarshaller{document};
  bool decoded{};

  // The exception is being thrown by an Attribute resolver
  ASSERT_THROW(unmarshaller(bad_attribute, decoded), std::logic_error);
}

TEST_F(HttpJsonUnmarshaller,
       ThrowsExceptionWhenUnmarshallingBooleanNotBoolEncoded) {
  add_member(Key, 1, document);

  Unmarshaller unmarshaller{document};
  bool decoded{};

  ASSERT_THROW(unmarshaller(Attribute, decoded), std::runtime_error);
}

TEST_F(HttpJsonUnmarshaller, UnmarshallsBooleanByKey) {
  add_member(Key, true, document);

  Unmarshaller unmarshaller{document};
  bool decoded{};

  ASSERT_TRUE(unmarshaller(Key, decoded));
  ASSERT_EQ(decoded, true);
}

TEST_F(HttpJsonUnmarshaller, UnmarshallsBooleanByAttribute) {
  add_member(Key, true, document);

  Unmarshaller unmarshaller{document};
  bool decoded{};

  ASSERT_TRUE(unmarshaller(Attribute, decoded));
  ASSERT_EQ(decoded, true);
}

TEST_F(HttpJsonUnmarshaller,
       ThrowsExceptionWhenUnmarshallsCharacterFromEmptyString) {
  add_member(Key, std::string{}, document);

  Unmarshaller unmarshaller{document};
  char decoded{};

  ASSERT_THROW(unmarshaller(Attribute, decoded), std::runtime_error);
}

TEST_F(HttpJsonUnmarshaller,
       ThrowsExceptionWhenUnmarshallsTwoCharactersStringAsChar) {
  const std::string value = "AB";
  add_member(Key, value, document);

  Unmarshaller unmarshaller{document};
  char decoded{};

  ASSERT_THROW(unmarshaller(Attribute, decoded), std::runtime_error);
}

TEST_F(HttpJsonUnmarshaller, UnmarshallsSingleCharacter) {
  const std::string value = ";";
  add_member(Key, value, document);

  Unmarshaller unmarshaller{document};
  char decoded{};

  ASSERT_TRUE(unmarshaller(Attribute, decoded));
  ASSERT_EQ(decoded, ';');
}

TEST_F(HttpJsonUnmarshaller, ThrowsExceptionWhenUnmarshallingStringAsDouble) {
  const std::string value = "bad_type";
  add_member(Key, value, document);

  Unmarshaller unmarshaller{document};
  double decoded{};

  ASSERT_THROW(unmarshaller(Attribute, decoded), std::runtime_error);
}

TEST_F(HttpJsonUnmarshaller, UnmarshallsIntegerAsDouble) {
  add_member(Key, 42, document);  // NOLINT
  ASSERT_TRUE(document[Key].IsInt64());

  Unmarshaller unmarshaller{document};
  double decoded{};

  ASSERT_TRUE(unmarshaller(Attribute, decoded));
  ASSERT_DOUBLE_EQ(decoded, 42.0);
}

TEST_F(HttpJsonUnmarshaller, UnmarshallsUnsignedIntegerAsDouble) {
  add_member(Key, 42U, document);  // NOLINT

  Unmarshaller unmarshaller{document};
  double decoded{};

  ASSERT_TRUE(unmarshaller(Attribute, decoded));
  ASSERT_DOUBLE_EQ(decoded, 42.0);
}

TEST_F(HttpJsonUnmarshaller, UnmarshallsDouble) {
  add_member(Key, 42.42, document);  // NOLINT

  Unmarshaller unmarshaller{document};
  double decoded{};

  ASSERT_TRUE(unmarshaller(Attribute, decoded));
  ASSERT_DOUBLE_EQ(decoded, 42.42);
}

TEST_F(HttpJsonUnmarshaller, ThrowsExceptionWhenUnmarshallingBoolAsString) {
  add_member(Key, false, document);

  Unmarshaller unmarshaller{document};
  std::string decoded{};

  ASSERT_THROW(unmarshaller(Attribute, decoded), std::runtime_error);
}

TEST_F(HttpJsonUnmarshaller, UnmarshallsString) {
  const std::string value = "string_value";
  add_member(Key, value, document);

  Unmarshaller unmarshaller{document};
  std::string decoded{};

  ASSERT_TRUE(unmarshaller(Attribute, decoded));
  ASSERT_EQ(decoded, value);
}

TEST_F(
    HttpJsonUnmarshaller,
    ThrowsExceptionWhenUnmarshalling64BitUnsingedIntegerAs32BitSignedInteger) {
  constexpr auto value = std::numeric_limits<std::uint64_t>::max();
  add_member(Key, value, document);

  Unmarshaller unmarshaller{document};
  std::int32_t decoded{};

  ASSERT_THROW(unmarshaller(Attribute, decoded), std::runtime_error);
}

TEST_F(HttpJsonUnmarshaller, Unmarshalls32BitSingedInteger) {
  add_member(Key, -42, document);  // NOLINT
  ASSERT_TRUE(document[Key].IsInt64());

  Unmarshaller unmarshaller{document};
  std::int32_t decoded{};

  ASSERT_TRUE(unmarshaller(Attribute, decoded));
  ASSERT_EQ(decoded, -42);
}

TEST_F(HttpJsonUnmarshaller,
       ThrowsExceptionWhenUnmarshallingSingedIntegerAsUnsigned) {
  add_member(Key, -1, document);
  ASSERT_FALSE(document[Key].IsUint64());

  Unmarshaller unmarshaller{document};
  std::uint32_t decoded{};

  ASSERT_THROW(unmarshaller(Attribute, decoded), std::runtime_error);
}

TEST_F(HttpJsonUnmarshaller, Unmarshalls64BitUnsignedInteger) {
  constexpr auto value = std::numeric_limits<std::uint64_t>::max();
  add_member(Key, value, document);  // NOLINT

  Unmarshaller unmarshaller{document};
  std::uint64_t decoded{};

  ASSERT_TRUE(unmarshaller(Attribute, decoded));
  ASSERT_EQ(decoded, value);
}

TEST_F(HttpJsonUnmarshaller, ThrowsExceptionWhenUnmarshallingBoolAsEnum) {
  add_member(Key, false, document);

  Unmarshaller unmarshaller{document};
  Venue::EngineType decoded{};

  ASSERT_THROW(unmarshaller(Attribute, decoded), std::runtime_error);
}

TEST_F(HttpJsonUnmarshaller,
       ThrowsExceptionWhenUnmarshallingUnknownEnumStringRepresentation) {
  const std::string value = "bad_value";
  add_member(Key, value, document);

  Unmarshaller unmarshaller{document};
  Venue::EngineType decoded{};

  ASSERT_THROW(unmarshaller(Attribute, decoded), std::runtime_error);
}

TEST_F(HttpJsonUnmarshaller, UnmarshallsEnumFromString) {
  const std::string value = "Quoting";
  add_member(Key, value, document);

  Unmarshaller unmarshaller{document};
  Venue::EngineType decoded{};

  ASSERT_TRUE(unmarshaller(Attribute, decoded));
  ASSERT_EQ(decoded, Venue::EngineType::Quoting);
}

}  // namespace
}  // namespace simulator::http::json::test
