#include <gmock/gmock.h>
#include <quickfix/Exceptions.h>
#include <quickfix/FieldMap.h>
#include <quickfix/Fields.h>

#include "core/domain/attributes.hpp"
#include "ih/mapping/from_fix_mapping.hpp"

namespace simulator::fix::generator_initiator::test {
namespace {

using namespace testing;  // NOLINT

struct InitiatorFromFixMapping : Test {
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

TEST_F(InitiatorFromFixMapping, ConvertsFixFieldToInternalFieldType) {
  const auto fix_field = make_fix_field<FIX::Price>(42.0);

  const auto internal_field = convert_from_fix_field<OrderPrice>(fix_field);

  ASSERT_THAT(internal_field.value(), DoubleEq(42.0));
}

TEST_F(InitiatorFromFixMapping, ReportsConversionError) {
  const auto fix_field = make_fix_field<FIX::MDEntryType>('x');

  ASSERT_THROW(convert_from_fix_field<MdEntryType>(fix_field),
               FIX::IncorrectTagValue);
}

TEST_F(InitiatorFromFixMapping, RetrievesFixFieldFromFieldMap) {
  const auto fix_field = make_fix_field<FIX::Price>(42.0);
  set_fix_field(fix_field);

  const auto retrieved_field = get_fix_field<FIX::Price>(field_map);

  ASSERT_THAT(retrieved_field, Eq(fix_field));
}

TEST_F(InitiatorFromFixMapping, ReportsErrorWhenFieldIsNotFound) {
  ASSERT_THROW(get_fix_field<FIX::Price>(field_map), FIX::FieldNotFound);
}

TEST_F(InitiatorFromFixMapping, MapsFixFieldToInternalField) {
  set_fix_field(make_fix_field<FIX::Price>(42.0));
  std::optional<OrderPrice> internal_field;

  ASSERT_NO_THROW(map_fix_field<FIX::Price>(field_map, internal_field));

  ASSERT_THAT(internal_field,
              Optional(Property(&OrderPrice::value, DoubleEq(42.0))));
}

TEST_F(InitiatorFromFixMapping, ReportsErrorWhenFixFieldCanNotBeMapped) {
  set_fix_field(make_fix_field<FIX::MDEntryType>('x'));
  std::optional<MdEntryType> internal_field;

  ASSERT_THROW(map_fix_field<FIX::MDEntryType>(field_map, internal_field),
               FIX::IncorrectTagValue);
}

TEST_F(InitiatorFromFixMapping, DoesNothingWhenFixFieldIsNotPresent) {
  std::optional<OrderPrice> internal_field;

  ASSERT_NO_THROW(map_fix_field<FIX::Price>(field_map, internal_field));

  ASSERT_THAT(internal_field, Eq(std::nullopt));
}

}  // namespace
}  // namespace simulator::fix::generator_initiator::test