#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <csv.hpp>
#include <string>

#include "ih/historical/adapters/csv_reader.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/mapping/params.hpp"
#include "ih/historical/parsing/params.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;

class Generator_Historical_CsvReader : public testing::Test {
 public:
  static auto make_default_mapping() -> MappingParams {
    return MappingParams{MappingParams::ColumnMappings{}};
  }

  static auto make_parsing_params(std::uint64_t header_row = 0,
                                  std::uint64_t data_row = 1,
                                  char delimiter = ',') -> CsvParsingParams {
    std::string mock_path{"/a/path/to/nonexistent/csv-file.csv"};
    std::string mock_name{"Mock datasource name"};

    GeneralDatasourceParams general_params{};
    general_params.set_name(mock_name);
    general_params.set_connection(mock_path);

    CsvParsingParams params{general_params};
    params.set_header_row(header_row);
    params.set_data_row(data_row);
    params.set_delimiter(delimiter);

    return params;
  }

  static auto parse_csv(std::string_view content, CsvParsingParams params)
      -> csv::CSVReader {
    const csv::CSVFormat csv_format = CsvReader::make_format(params);
    return csv::parse(content, csv_format);
  }
};

TEST_F(Generator_Historical_CsvReader, MakeFormat_WithHeader) {
  const CsvParsingParams params = make_parsing_params(1, 2, ',');
  const csv::CSVFormat format = CsvReader::make_format(params);

  EXPECT_EQ(format.get_header(), 0);
  EXPECT_EQ(format.get_delim(), ',');
}

TEST_F(Generator_Historical_CsvReader, MakeFormat_WithoutHeader) {
  const CsvParsingParams params = make_parsing_params(0, 1, ',');
  const csv::CSVFormat format = CsvReader::make_format(params);

  EXPECT_EQ(format.get_header(), -1);
  EXPECT_EQ(format.get_delim(), ',');
}

TEST_F(Generator_Historical_CsvReader, MakeFormat_WithCustomDelimiter) {
  const CsvParsingParams params = make_parsing_params(0, 1, ';');
  const csv::CSVFormat format = CsvReader::make_format(params);

  EXPECT_EQ(format.get_header(), -1);
  EXPECT_EQ(format.get_delim(), ';');
}

TEST_F(Generator_Historical_CsvReader, ThrowsExceptionWhenCsvIsEmpty) {
  constexpr std::string_view csv_content = "";  // NOLINT

  const MappingParams mapping_params = make_default_mapping();
  const CsvParsingParams parsing_params = make_parsing_params(0, 1, ',');
  csv::CSVReader csv = parse_csv(csv_content, parsing_params);

  ASSERT_THROW(
      historical::CsvReader reader(parsing_params, mapping_params, csv),
      std::invalid_argument);
}

TEST_F(Generator_Historical_CsvReader, ReadCsv_WithHeader) {
  // clang-format off
  constexpr std::string_view csv_content =
    "ReceivedTimeStamp,MessageTimeStamp,Instrument,BidParty,BidQuantity,BidPrice,AskPrice,AskQuantity,AskParty\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP1,120,12,22,220,AP1\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP2,320,32,42,420,AP2\n";
  // clang-format on

  const MappingParams mapping_params = make_default_mapping();
  const CsvParsingParams parsing_params = make_parsing_params(1, 2, ',');
  csv::CSVReader csv = parse_csv(csv_content, parsing_params);

  EXPECT_THAT(csv.get_col_names(),
              ElementsAre("ReceivedTimeStamp",
                          "MessageTimeStamp",
                          "Instrument",
                          "BidParty",
                          "BidQuantity",
                          "BidPrice",
                          "AskPrice",
                          "AskQuantity",
                          "AskParty"));

  MockFunction<void(Record)> record_visitor;
  EXPECT_CALL(record_visitor, Call).Times(2);

  historical::CsvReader reader{parsing_params, mapping_params, csv};
  reader.accept(record_visitor.AsStdFunction());
}

TEST_F(Generator_Historical_CsvReader, ReadCsv_WithCommentedHeader) {
  // clang-format off
  constexpr std::string_view csv_content =
    "This is an ugly comment in the beginning of the file,\n"
    "ReceivedTimeStamp,MessageTimeStamp,Instrument,BidParty,BidQuantity,BidPrice,AskPrice,AskQuantity,AskParty\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP1,120,12,22,220,AP1\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP2,320,32,42,420,AP2\n";
  // clang-format on

  const MappingParams mapping_params = make_default_mapping();
  const CsvParsingParams parsing_params = make_parsing_params(2, 3);
  csv::CSVReader csv = parse_csv(csv_content, parsing_params);

  EXPECT_THAT(csv.get_col_names(),
              ElementsAre("ReceivedTimeStamp",
                          "MessageTimeStamp",
                          "Instrument",
                          "BidParty",
                          "BidQuantity",
                          "BidPrice",
                          "AskPrice",
                          "AskQuantity",
                          "AskParty"));

  MockFunction<void(Record)> record_visitor;
  EXPECT_CALL(record_visitor, Call).Times(2);

  historical::CsvReader reader{parsing_params, mapping_params, csv};
  reader.accept(record_visitor.AsStdFunction());
}

TEST_F(Generator_Historical_CsvReader, ReadCsv_WithMultiHeader) {
  // clang-format off
  constexpr std::string_view csv_content =
    "rec_time,msg_time,instr,bid_party,bid_px,bid_qty,ask_qty,ask_px,ask_party,\n"
    "ReceivedTimeStamp,MessageTimeStamp,Instrument,BidParty,BidQuantity,BidPrice,AskPrice,AskQuantity,AskParty\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP1,120,12,22,220,AP1\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP2,320,32,42,420,AP2\n";
  // clang-format on

  const MappingParams mapping_params = make_default_mapping();
  const CsvParsingParams parsing_params = make_parsing_params(2, 3);
  csv::CSVReader csv = parse_csv(csv_content, parsing_params);

  EXPECT_THAT(csv.get_col_names(),
              ElementsAre("ReceivedTimeStamp",
                          "MessageTimeStamp",
                          "Instrument",
                          "BidParty",
                          "BidQuantity",
                          "BidPrice",
                          "AskPrice",
                          "AskQuantity",
                          "AskParty"));

  MockFunction<void(Record)> record_visitor;
  EXPECT_CALL(record_visitor, Call).Times(2);

  historical::CsvReader reader{parsing_params, mapping_params, csv};
  reader.accept(record_visitor.AsStdFunction());
}

TEST_F(Generator_Historical_CsvReader, ReadCsv_WithoutHeader) {
  // clang-format off
  constexpr std::string_view csv_content =
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP1,120,12,22,220,AP1\r\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP2,320,32,42,420,AP2\r\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP3,520,52,62,620,AP3\r\n";
  // clang-format on

  const MappingParams mapping_params = make_default_mapping();
  const CsvParsingParams parsing_params = make_parsing_params(0, 1, ',');
  csv::CSVReader csv = parse_csv(csv_content, parsing_params);

  MockFunction<void(Record)> record_visitor;
  EXPECT_CALL(record_visitor, Call).Times(3);

  historical::CsvReader reader{parsing_params, mapping_params, csv};
  reader.accept(record_visitor.AsStdFunction());
}

TEST_F(Generator_Historical_CsvReader, ReadCsv_FromSpecificRow) {
  // clang-format off
  constexpr std::string_view csv_content =
    "ReceivedTimeStamp,MessageTimeStamp,Instrument,BidParty,BidQuantity,BidPrice,AskPrice,AskQuantity,AskParty\r\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP1,120,12,22,220,AP1\r\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP2,320,32,42,420,AP2\r\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP3,520,52,62,620,AP3\r\n";
  // clang-format on

  const MappingParams mapping_params = make_default_mapping();
  const CsvParsingParams parsing_params = make_parsing_params(1, 3, ',');
  csv::CSVReader csv = parse_csv(csv_content, parsing_params);

  MockFunction<void(Record)> record_visitor;
  EXPECT_CALL(record_visitor, Call).Times(2);

  historical::CsvReader reader{parsing_params, mapping_params, csv};
  reader.accept(record_visitor.AsStdFunction());
}

TEST_F(Generator_Historical_CsvReader, ReadCsv_FromNonexistentFile) {
  EXPECT_THROW(CsvReader::create(make_parsing_params(), make_default_mapping()),
               std::runtime_error);
}

TEST_F(Generator_Historical_CsvReader, ParsesOneLevelIfHeaderIsNotPresent) {
  // clang-format off
  constexpr std::string_view csv_content =
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP1,120,12,22,220,AP1\r\n";
  // clang-format on

  const MappingParams mapping_params = make_default_mapping();
  const CsvParsingParams parsing_params = make_parsing_params(0, 1, ',');
  csv::CSVReader csv = parse_csv(csv_content, parsing_params);

  historical::CsvReader reader{parsing_params, mapping_params, csv};

  MockFunction<void(std::uint64_t, const Level&)> level_visitor;
  EXPECT_CALL(level_visitor, Call).Times(1);

  reader.accept([&](const Record& record) {
    record.visit_levels(level_visitor.AsStdFunction());
  });
}

TEST_F(Generator_Historical_CsvReader, ParsesOneLevelIfHeaderIsPresent) {
  // clang-format off
  constexpr std::string_view csv_content =
    "ReceivedTimeStamp,MessageTimeStamp,Instrument,BidParty,BidQuantity,BidPrice,AskPrice,AskQuantity,AskParty\r\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP1,120,12,22,220,AP1\r\n";
  // clang-format on

  const MappingParams mapping_params = make_default_mapping();
  const CsvParsingParams parsing_params = make_parsing_params(1, 2, ',');
  csv::CSVReader csv = parse_csv(csv_content, parsing_params);

  historical::CsvReader reader{parsing_params, mapping_params, csv};

  MockFunction<void(std::uint64_t, const Level&)> level_visitor;
  EXPECT_CALL(level_visitor, Call).Times(1);

  reader.accept([&](const Record& record) {
    record.visit_levels(level_visitor.AsStdFunction());
  });
}

TEST_F(Generator_Historical_CsvReader,
       ParsesOneLevelIfHeaderIsNotPresentAndMappingIsNotConfigured) {
  // clang-format off
  constexpr std::string_view csv_content =
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP1,120,12,22,220,AP1,BP2,320,32,42,420,AP2\r\n";
  // clang-format on

  const MappingParams mapping_params = make_default_mapping();
  const CsvParsingParams parsing_params = make_parsing_params(0, 1, ',');
  csv::CSVReader csv = parse_csv(csv_content, parsing_params);

  historical::CsvReader reader{parsing_params, mapping_params, csv};

  MockFunction<void(std::uint64_t, const Level&)> level_visitor;
  EXPECT_CALL(level_visitor, Call).Times(1);

  reader.accept([&](const Record& record) {
    record.visit_levels(level_visitor.AsStdFunction());
  });
}

TEST_F(Generator_Historical_CsvReader,
       ParsesOneLevelIfHeaderIsPresentAndMappingIsNotConfigured) {
  // clang-format off
  constexpr std::string_view csv_content =
    "ReceivedTimeStamp,MessageTimeStamp,Instrument,BidParty,BidQuantity,BidPrice,AskPrice,AskQuantity,AskParty,BidParty2,BidQuantity2,BidPrice2,AskPrice2,AskQuantity2,AskParty2\r\n"
    "2023-01-20 12:00:32.345,2023-01-20 12:00:33.006,AUD/CAD,BP1,120,12,22,220,AP1,BP2,320,32,42,420,AP2\r\n";
  // clang-format on

  const MappingParams mapping_params = make_default_mapping();
  const CsvParsingParams parsing_params = make_parsing_params(1, 2, ',');
  csv::CSVReader csv = parse_csv(csv_content, parsing_params);

  historical::CsvReader reader{parsing_params, mapping_params, csv};

  MockFunction<void(std::uint64_t, const Level&)> level_visitor;
  EXPECT_CALL(level_visitor, Call).Times(1);

  reader.accept([&](const Record& record) {
    record.visit_levels(level_visitor.AsStdFunction());
  });
}

}  // namespace
}  // namespace simulator::generator::historical::test