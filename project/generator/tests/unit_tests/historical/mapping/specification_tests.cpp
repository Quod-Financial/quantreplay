#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/historical/mapping/specification.hpp"

namespace simulator::generator::historical::mapping::test {
namespace {

using namespace ::testing;

MATCHER_P(HasIndex, index, "") {
  return arg.has_value() && arg->index() == index;
}

struct GeneratorHistoricalMappingSpecification : public testing::Test {
  Specification specification;
};

TEST_F(GeneratorHistoricalMappingSpecification, DoesNotResolveMissingSource) {
  EXPECT_FALSE(
      specification.resolve_by(data_layer::converter::ColumnFrom::Instrument)
          .has_value());
}

TEST_F(GeneratorHistoricalMappingSpecification, ResolvesAssociatedSource) {
  const auto column_from = data_layer::converter::ColumnFrom::Instrument;
  constexpr SourceColumn source{10};

  specification.associate(column_from, source);

  const auto resolved_source = specification.resolve_by(column_from);
  ASSERT_THAT(resolved_source, HasIndex(source.index()));
}

TEST_F(GeneratorHistoricalMappingSpecification, ResolvesReassociatedSource) {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  constexpr SourceColumn initial_source{1};
  constexpr SourceColumn updated_source{2};
  const auto column_from =
      ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value();

  specification.associate(column_from, initial_source);
  specification.associate(column_from, updated_source);

  const auto source = specification.resolve_by(column_from);
  ASSERT_THAT(source, HasIndex(updated_source.index()));
}

TEST_F(GeneratorHistoricalMappingSpecification, StoresTwoAssociations) {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  constexpr SourceColumn source1{1};
  constexpr SourceColumn source2{2};
  const auto column_from1 =
      ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value();
  const auto column_from2 =
      ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value();

  specification.associate(column_from1, source1);
  specification.associate(column_from2, source2);

  const auto resolved_source1 = specification.resolve_by(column_from1);
  ASSERT_THAT(resolved_source1, HasIndex(source1.index()));

  const auto resolved_source2 = specification.resolve_by(column_from2);
  ASSERT_THAT(resolved_source2, HasIndex(source2.index()));
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test
