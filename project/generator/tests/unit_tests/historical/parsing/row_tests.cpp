#include <gtest/gtest.h>

#include <csv.hpp>
#include <pqxx/row>

#include "ih/historical/parsing/row.hpp"

namespace simulator::generator::historical::test {
namespace {

struct GeneratorHistoricalParsingRow : public ::testing::Test {
  Row row;
};

TEST_F(GeneratorHistoricalParsingRow, EmptyRowStoresZeroColumns) {
  EXPECT_EQ(row.columns(), 0);
}

TEST_F(GeneratorHistoricalParsingRow, StoresOneColumnForOneCell) {
  row.emplace_next("AAA");
  EXPECT_EQ(row.columns(), 1);
}

TEST_F(GeneratorHistoricalParsingRow, ThrowsExceptionOnAccessOutOfRange) {
  row.emplace_next("AAA");
  EXPECT_THROW((void)row[1], std::out_of_range);
}

TEST_F(GeneratorHistoricalParsingRow, GivesAccessToCell) {
  row.emplace_next("AAA");
  EXPECT_EQ(row[0], "AAA");
}

TEST_F(GeneratorHistoricalParsingRow, IsCreatedFromCsvRow) {
  using ::csv::operator""_csv_no_header;

  csv::CSVReader csv_reader = "CsvCell1,CsvCell2,CsvCell3"_csv_no_header;
  csv::CSVRow csv_row;
  csv_reader.read_row(csv_row);

  row = Row::from(csv_row);
  ASSERT_EQ(row.columns(), 3);
  EXPECT_EQ(row[0], "CsvCell1");
  EXPECT_EQ(row[1], "CsvCell2");
  EXPECT_EQ(row[2], "CsvCell3");
}

TEST_F(GeneratorHistoricalParsingRow, Create_FromPqxxRow) {
  // We can not fill or mock a pqxx row, so TEST_Fing with an empty row
  const pqxx::row pq_row;
  row = Row::from(pq_row);
  EXPECT_EQ(row.columns(), 0);
}

}  // namespace
}  // namespace simulator::generator::historical::test