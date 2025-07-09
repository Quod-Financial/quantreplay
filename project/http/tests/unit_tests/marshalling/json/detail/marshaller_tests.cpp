#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <rapidjson/document.h>

#include <stdexcept>
#include <string>

#include "data_layer/api/models/venue.hpp"
#include "ih/marshalling/json/detail/marshaller.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::http::json::test {
namespace {

struct HttpJsonMarshaller : public ::testing::Test {
  using Venue = data_layer::Venue;

  rapidjson::Document document;
};

TEST_F(HttpJsonMarshaller, CreatesWithNonObjectDocument) {
  ASSERT_FALSE(document.IsObject());

  const Marshaller marshaller{document};
  EXPECT_TRUE(document.IsObject());
}

TEST_F(HttpJsonMarshaller, CreatesWithObjectDocument) {
  document.SetObject();
  ASSERT_TRUE(document.IsObject());

  const Marshaller marshaller{document};
  EXPECT_TRUE(document.IsObject());
}

TEST_F(HttpJsonMarshaller, ThrowExceptionOnMarshallingBadAttributeValue) {
  constexpr auto bad_attribute = static_cast<Venue::Attribute>(-1);
  Marshaller marshaller{document};

  // The exception is being thrown by an Attribute key resolver
  EXPECT_THROW(marshaller(bad_attribute, true), std::logic_error);
}

TEST_F(HttpJsonMarshaller, MarshallsBoolean) {
  constexpr auto attribute = Venue::Attribute::VenueId;
  constexpr const char* key = "id";

  Marshaller marshaller{document};

  ASSERT_NO_THROW(marshaller(attribute, true));

  ASSERT_THAT(document, HasBool(key, true));
}

TEST_F(HttpJsonMarshaller, MarshallsCharacter) {
  constexpr auto attribute = Venue::Attribute::VenueId;
  constexpr const char* key = "id";

  Marshaller marshaller{document};

  constexpr char character = ';';
  ASSERT_NO_THROW(marshaller(attribute, character));

  ASSERT_THAT(document, HasString(key, ";"));
}

TEST_F(HttpJsonMarshaller, MarshallsDouble) {
  constexpr auto attribute = Venue::Attribute::VenueId;
  constexpr const char* key = "id";

  Marshaller marshaller{document};

  ASSERT_NO_THROW(marshaller(attribute, 42.42));  // NOLINT

  ASSERT_THAT(document, HasDouble(key, 42.42));
}

TEST_F(HttpJsonMarshaller, MarshallsSignedInteger) {
  constexpr auto attribute = Venue::Attribute::VenueId;
  constexpr const char* key = "id";

  Marshaller marshaller{document};

  ASSERT_NO_THROW(marshaller(attribute, -42));  // NOLINT

  ASSERT_THAT(document, HasInt64(key, -42));
}

TEST_F(HttpJsonMarshaller, MarshallsUnsignedInteger) {
  constexpr auto attribute = Venue::Attribute::VenueId;
  constexpr const char* key = "id";

  Marshaller marshaller{document};

  ASSERT_NO_THROW(marshaller(attribute, 42U));  // NOLINT

  ASSERT_THAT(document, HasUInt64(key, 42U));
}

TEST_F(HttpJsonMarshaller, MarshallsEnumerationBadValue) {
  constexpr auto attribute = Venue::Attribute::VenueId;
  constexpr auto bad_enum = static_cast<Venue::EngineType>(-1);

  Marshaller marshaller{document};

  // The exception is being thrown by an Enumeration resolver
  EXPECT_THROW(marshaller(attribute, bad_enum), std::logic_error);
}

TEST_F(HttpJsonMarshaller, MarshallsEnumerationCorrectValue) {
  constexpr auto attribute = Venue::Attribute::VenueId;
  constexpr const char* key = "id";
  constexpr auto enum_value = Venue::EngineType::Matching;

  Marshaller marshaller{document};

  ASSERT_NO_THROW(marshaller(attribute, enum_value));  // NOLINT

  ASSERT_THAT(document, HasString(key, "Matching"));
}

}  // namespace
}  // namespace simulator::http::json::test
