#include "ih/historical/parsing/parsing.hpp"

#include <charconv>
#include <string>

#include "ih/historical/data/time.hpp"

namespace simulator::generator::historical {

auto parse_value(std::string_view data, double& result) -> bool {
  double old_value = result;
  const char* end = data.data() + data.size();
  const auto parsing_res = std::from_chars(data.data(), end, result);

  const bool parsing_success =
      parsing_res.ptr == end && parsing_res.ec == std::errc{};
  if (!parsing_success) {
    result = old_value;
  }
  return parsing_success;
}

auto parse_value(std::string_view data, historical::Timepoint& result) -> bool {
  return historical::time::from(data, result);
}

auto parse_value(std::string_view data, std::string& result) -> bool {
  result = data;
  return true;
}

auto parse_bid_level_part(const Row& row,
                          Level::Builder& builder,
                          const MappingParams& mapping,
                          std::uint32_t depth) -> void {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  if (auto column_idx = mapping.column_idx(
          ColumnFrom::create(ColumnFrom::Column::BidPrice, depth).value())) {
    double price = std::numeric_limits<double>::quiet_NaN();
    if (parse_cell(row, *column_idx, price) && !std::isnan(price)) {
      builder.with_bid_price(price);
    }
  }
  if (auto column_idx = mapping.column_idx(
          ColumnFrom::create(ColumnFrom::Column::BidQuantity, depth).value())) {
    double quantity = std::numeric_limits<double>::quiet_NaN();
    if (parse_cell(row, *column_idx, quantity) && !std::isnan(quantity)) {
      builder.with_bid_quantity(quantity);
    }
  }
  if (auto column_idx = mapping.column_idx(
          ColumnFrom::create(ColumnFrom::Column::BidParty, depth).value())) {
    std::string party;
    if (parse_cell(row, *column_idx, party) && !party.empty()) {
      builder.with_bid_counterparty(std::move(party));
    }
  }
}

auto parse_offer_level_part(const Row& row,
                            Level::Builder& builder,
                            const MappingParams& mapping,
                            std::uint32_t depth) -> void {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  if (auto column_idx = mapping.column_idx(
          ColumnFrom::create(ColumnFrom::Column::OfferPrice, depth).value())) {
    double price = std::numeric_limits<double>::quiet_NaN();
    if (parse_cell(row, *column_idx, price) && !std::isnan(price)) {
      builder.with_offer_price(price);
    }
  }
  if (auto column_idx = mapping.column_idx(
          ColumnFrom::create(ColumnFrom::Column::OfferQuantity, depth)
              .value())) {
    double quantity = std::numeric_limits<double>::quiet_NaN();
    if (parse_cell(row, *column_idx, quantity) && !std::isnan(quantity)) {
      builder.with_offer_quantity(quantity);
    }
  }
  if (auto column_idx = mapping.column_idx(
          ColumnFrom::create(ColumnFrom::Column::OfferParty, depth).value())) {
    std::string party;
    if (parse_cell(row, *column_idx, party) && !party.empty()) {
      builder.with_offer_counterparty(std::move(party));
    }
  }
}

auto parse(const Row& row,
           historical::Record::Builder& result_builder,
           const MappingParams& mapping,
           std::uint32_t depth_levels_to_parse) -> void {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  if (auto column_idx = mapping.column_idx(ColumnFrom::ReceivedTimestamp)) {
    std::chrono::system_clock::time_point receive_time;
    if (parse_cell(row, *column_idx, receive_time)) {
      result_builder.with_receive_time(receive_time);
    }
  }

  if (auto column_idx = mapping.column_idx(ColumnFrom::MessageTimestamp)) {
    std::chrono::system_clock::time_point message_time;
    if (parse_cell(row, *column_idx, message_time)) {
      result_builder.with_message_time(message_time);
    }
  }
  if (auto column_idx = mapping.column_idx(ColumnFrom::Instrument)) {
    std::string instrument;
    if (parse_cell(row, *column_idx, instrument) && !instrument.empty()) {
      result_builder.with_instrument(std::move(instrument));
    }
  }

  for (auto depth = 1u; depth <= depth_levels_to_parse; ++depth) {
    historical::Level::Builder level_builder;
    parse_bid_level_part(row, level_builder, mapping, depth);
    parse_offer_level_part(row, level_builder, mapping, depth);

    if (!level_builder.empty()) {
      auto level = Level::Builder::construct(std::move(level_builder));
      result_builder.add_level(depth - 1, std::move(level));
    }
  }
}

}  // namespace simulator::generator::historical
