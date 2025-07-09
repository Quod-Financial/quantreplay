#include <gmock/gmock.h>
#include <quickfix/Exceptions.h>
#include <quickfix/FieldMap.h>
#include <quickfix/Fields.h>

#include "common/mapping/from_fix_mapping.hpp"
#include "core/domain/attributes.hpp"

namespace simulator::fix::test {
namespace {

using namespace testing;  // NOLINT

struct FromFixMapping : Test {
  template <typename FixFieldType, typename ValueType>
  static auto make_fix_field(const ValueType& value) -> FixFieldType {
    return FixFieldType{value};
  }

  template <typename FixFieldType>
  auto set_fix_field(const FixFieldType& field) -> void {
    field_map.setField(field);
  }

  FIX::FieldMap field_map;
};

TEST_F(FromFixMapping, ConvertsFixFieldToInternalFieldType) {
  const auto fix_field = make_fix_field<FIX::Price>(42.0);

  const auto internal_field = convert_from_fix_field<OrderPrice>(fix_field);

  ASSERT_THAT(internal_field.value(), DoubleEq(42.0));
}

TEST_F(FromFixMapping, ReportsConversionError) {
  // Such order type FIX value is not supported
  const auto fix_field = make_fix_field<FIX::OrdType>('x');

  ASSERT_THROW(convert_from_fix_field<OrderType>(fix_field),
               FIX::IncorrectTagValue);
}

TEST_F(FromFixMapping, RetrievesFixFieldFromFieldMap) {
  const auto fix_field = make_fix_field<FIX::Price>(42.0);
  set_fix_field(fix_field);

  const auto retrieved_field = get_fix_field<FIX::Price>(field_map);

  ASSERT_THAT(retrieved_field, Eq(fix_field));
}

TEST_F(FromFixMapping, ReportsErrorWhenFieldIsNotFound) {
  ASSERT_THROW(get_fix_field<FIX::Price>(field_map), FIX::FieldNotFound);
}

TEST_F(FromFixMapping, MapsFixFieldToInternalField) {
  set_fix_field(make_fix_field<FIX::Price>(42.0));
  std::optional<OrderPrice> internal_field;

  ASSERT_NO_THROW(map_fix_field<FIX::Price>(field_map, internal_field));

  ASSERT_THAT(internal_field,
              Optional(Property(&OrderPrice::value, DoubleEq(42.0))));
}

TEST_F(FromFixMapping, ReportsErrorWhenFixFieldCanNotBeMapped) {
  // such order type FIX value is not supported
  set_fix_field(make_fix_field<FIX::OrdType>('x'));
  std::optional<OrderType> internal_field;

  ASSERT_THROW(map_fix_field<FIX::OrdType>(field_map, internal_field),
               FIX::IncorrectTagValue);
}

TEST_F(FromFixMapping, DoesNothingWhenFixFieldIsNotPresent) {
  std::optional<OrderPrice> internal_field;

  ASSERT_NO_THROW(map_fix_field<FIX::Price>(field_map, internal_field));

  ASSERT_THAT(internal_field, Eq(std::nullopt));
}

}  // namespace
}  // namespace simulator::fix::test