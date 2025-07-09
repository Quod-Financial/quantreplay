#include "ih/historical/data/record.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <cassert>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "core/common/std_formatter.hpp"
#include "ih/historical/data/time.hpp"

namespace simulator::generator::historical {

auto Level::bid_price() const noexcept -> std::optional<double> {
  return bid_price_;
}

auto Level::bid_quantity() const noexcept -> std::optional<double> {
  return bid_quantity_;
}

auto Level::bid_counterparty() const noexcept
    -> const std::optional<std::string>& {
  return bid_counterparty_;
}

auto Level::offer_price() const noexcept -> std::optional<double> {
  return offer_price_;
}

auto Level::offer_quantity() const noexcept -> std::optional<double> {
  return offer_quantity_;
}

auto Level::offer_counterparty() const noexcept
    -> const std::optional<std::string>& {
  return offer_counterparty_;
}

auto Level::Builder::with_bid_price(double px) noexcept -> Level::Builder& {
  bid_price_ = px;
  return *this;
}

auto Level::Builder::with_bid_quantity(double qty) noexcept -> Level::Builder& {
  bid_quantity_ = qty;
  return *this;
}

auto Level::Builder::with_bid_counterparty(std::string cp) noexcept
    -> Level::Builder& {
  bid_counterparty_ = std::make_optional(std::move(cp));
  return *this;
}

auto Level::Builder::with_offer_price(double px) noexcept -> Level::Builder& {
  offer_price_ = std::make_optional(px);
  return *this;
}

auto Level::Builder::with_offer_quantity(double qty) noexcept
    -> Level::Builder& {
  offer_quantity_ = std::make_optional(qty);
  return *this;
}

auto Level::Builder::with_offer_counterparty(std::string cp) noexcept
    -> Level::Builder& {
  offer_counterparty_ = std::make_optional(std::move(cp));
  return *this;
}

auto Level::Builder::empty() const noexcept -> bool {
  return !bid_price_.has_value() && !bid_quantity_.has_value() &&
         !bid_counterparty_.has_value() && !offer_price_.has_value() &&
         !offer_quantity_.has_value() && !offer_counterparty_.has_value();
}

auto Level::Builder::construct(Level::Builder builder) noexcept -> Level {
  Level level;

  level.bid_price_ = builder.bid_price_;
  level.bid_quantity_ = builder.bid_quantity_;
  level.bid_counterparty_ = std::move(builder.bid_counterparty_);

  level.offer_price_ = builder.offer_price_;
  level.offer_quantity_ = builder.offer_quantity_;
  level.offer_counterparty_ = std::move(builder.offer_counterparty_);

  return level;
}

auto Record::instrument() const noexcept -> const std::string& {
  return instrument_;
}

auto Record::receive_time() const noexcept -> historical::Timepoint {
  return received_time_;
}

auto Record::message_time() const noexcept
    -> std::optional<historical::Timepoint> {
  return message_time_;
}

auto Record::source_connection() const noexcept
    -> const std::optional<std::string>& {
  return source_conn_;
}

auto Record::source_name() const noexcept -> const std::optional<std::string>& {
  return source_name_;
}

auto Record::source_row() const noexcept -> std::uint64_t {
  return source_row_;
}

auto Record::has_levels() const noexcept -> bool { return !levels_.empty(); }

auto Record::steal_levels(const LevelStealer& stealer) -> void {
  std::vector<Level> stolen_levels;
  std::swap(levels_, stolen_levels);

  assert(levels_.empty());

  for (std::uint64_t level_idx = 0; level_idx < stolen_levels.size();
       ++level_idx) {
    stealer(level_idx, std::move(stolen_levels[level_idx]));
  }
}

auto Record::visit_levels(const LevelVisitor& visitor) const -> void {
  for (std::uint64_t level_idx = 0; level_idx < levels_.size(); ++level_idx) {
    visitor(level_idx, levels_[level_idx]);
  }
}

Record::Builder::Builder(Record base_record) noexcept
    : instrument_{std::move(base_record.instrument_)},
      source_name_{std::move(base_record.source_name_)},
      source_conn_{std::move(base_record.source_conn_)},
      levels_{std::move(base_record.levels_)},
      message_time_{base_record.message_time_},
      received_time_{base_record.received_time_},
      source_row_{base_record.source_row_} {}

auto Record::Builder::with_instrument(std::string instrument) noexcept
    -> Record::Builder& {
  instrument_ = std::make_optional(std::move(instrument));
  return *this;
}

auto Record::Builder::with_receive_time(
    historical::Timepoint receive_time) noexcept -> Record::Builder& {
  received_time_ = std::make_optional(receive_time);
  return *this;
}

auto Record::Builder::with_message_time(
    historical::Timepoint message_time) noexcept -> Record::Builder& {
  message_time_ = std::make_optional(message_time);
  return *this;
}

auto Record::Builder::with_source_name(std::string source_name) noexcept
    -> Record::Builder& {
  source_name_ = std::make_optional(std::move(source_name));
  return *this;
}

auto Record::Builder::with_source_connection(std::string source_conn) noexcept
    -> Record::Builder& {
  source_conn_ = std::make_optional(std::move(source_conn));
  return *this;
}

auto Record::Builder::with_source_row(std::uint64_t source_row) noexcept
    -> Record::Builder& {
  source_row_ = std::make_optional(source_row);
  return *this;
}

auto Record::Builder::add_level(std::uint64_t index, historical::Level level)
    -> Record::Builder& {
  if (index >= levels_.size()) {
    levels_.resize(index + 1);
  }

  levels_.at(index) = std::move(level);
  return *this;
}

auto Record::Builder::construct(Record::Builder builder) -> Record {
  Record record;

  validate(builder);

  assert(builder.source_row_.has_value());
  record.source_row_ = *builder.source_row_;

  assert(builder.received_time_.has_value());
  record.received_time_ = *builder.received_time_;

  assert(builder.instrument_.has_value());
  record.instrument_ = std::move(*builder.instrument_);

  record.message_time_ = builder.message_time_;
  record.source_name_ = std::move(builder.source_name_);
  record.source_conn_ = std::move(builder.source_conn_);

  record.levels_ = std::move(builder.levels_);

  return record;
}

auto Record::Builder::validate(const Builder& builder) -> void {
  if (!builder.source_row_.has_value()) {
    throw std::invalid_argument{
        "missing mandatory source row number attribute"};
  }

  const auto row = *(builder.source_row_);

  if (!builder.received_time_.has_value()) {
    throw std::invalid_argument{
        fmt::format("missing mandatory received time attribute "
                    "(row: {})",
                    row)};
  }

  if (!builder.instrument_.has_value()) {
    throw std::invalid_argument{
        fmt::format("missing mandatory instrument attribute "
                    "(row: {})",
                    row)};
  }
}

auto Action::action_time() const noexcept -> historical::Timepoint {
  return action_time_;
}

auto Action::steal_records(const RecordStealer& stealer) -> void {
  std::vector<Record> stolen_records;
  std::swap(records_, stolen_records);
  assert(records_.empty());

  for (auto& record : stolen_records) {
    stealer(std::move(record));
  }
}

auto Action::visit_records(const RecordVisitor& visitor) const -> void {
  for (const auto& record : records_) {
    visitor(record);
  }
}

auto Action::update_time(Action base_action, historical::Timepoint action_time)
    -> Action {
  using std::chrono::duration_cast;
  using std::chrono::time_point_cast;

  const auto updated_action_time =
      time_point_cast<historical::Duration>(action_time);
  auto time_offset = duration_cast<historical::Duration>(
      updated_action_time - base_action.action_time());

  Action updated_action{std::move(base_action)};
  updated_action.action_time_ = updated_action_time;

  for (Record& record : updated_action.records_) {
    auto updated_record = update_timepoints(std::move(record), time_offset);
    record = std::move(updated_record);
  }

  return updated_action;
}

auto Action::update_timepoints(Record&& record,
                               historical::Duration time_offset) -> Record {
  const auto base_receive_time = record.receive_time();
  const auto base_message_time = record.message_time();

  Record::Builder update_builder{std::move(record)};
  update_builder.with_receive_time(base_receive_time + time_offset);
  if (base_message_time.has_value()) {
    update_builder.with_message_time(*base_message_time + time_offset);
  }

  return Record::Builder::construct(std::move(update_builder));
}

auto Action::Builder::add(Record record, historical::Duration time_offset)
    -> void {
  auto updated_record = update_timepoints(std::move(record), time_offset);
  validate(updated_record);
  records_.emplace_back(std::move(updated_record));
}

auto Action::Builder::construct(Action::Builder builder) -> Action {
  Action action;

  if (builder.records_.empty()) {
    throw std::invalid_argument{
        "can not construct an action without any record"};
  }

  // Builder checks that all records have same ReceiveTimestamps.
  action.records_ = std::move(builder.records_);
  action.action_time_ = action.records_.front().receive_time();

  return action;
}

auto Action::Builder::validate(const Record& record) const -> void {
  if (records_.empty()) {
    return;
  }

  const auto new_record_time = record.receive_time();
  auto initial_record_time = records_.front().receive_time();
  if (new_record_time != initial_record_time) {
    throw std::logic_error{
        "a new record's receive time does not equal to "
        "an initial record's receive time"};
  }
}

auto operator<<(std::ostream& os, const Level& level) -> std::ostream& {
  return os << fmt::to_string(level);
}

auto operator<<(std::ostream& os, const Record& record) -> std::ostream& {
  return os << fmt::to_string(record);
}

auto operator<<(std::ostream& os, const Action& action) -> std::ostream& {
  return os << fmt::to_string(action);
}

}  // namespace simulator::generator::historical

auto fmt::formatter<simulator::generator::historical::Level>::format(
    const formattable& level, format_context& context) const
    -> decltype(context.out()) {
  return fmt::format_to(context.out(),
                        "{{ Bid={{ Price={} Qty={} Counterparty={} }} "
                        "Offer={{ Price={} Qty={} Counterparty={} }} }}",
                        level.bid_price(),
                        level.bid_quantity(),
                        level.bid_counterparty(),
                        level.offer_price(),
                        level.offer_quantity(),
                        level.offer_counterparty());
}

auto fmt::formatter<simulator::generator::historical::Record>::format(
    const formattable& record, format_context& context) const
    -> decltype(context.out()) {
  fmt::format_to(context.out(),
                 "Record={{ Instrument={} ReceiveTime={} MessageTime={} "
                 "RowNumber={} SourceName={} SourceConnection={}",
                 record.instrument(),
                 record.receive_time(),
                 record.message_time(),
                 record.source_row(),
                 record.source_name(),
                 record.source_connection());

  if (record.has_levels()) {
    format_levels(record, context);
  }

  return fmt::format_to(context.out(), " }}");
}

auto fmt::formatter<simulator::generator::historical::Record>::format_levels(
    const formattable& record, format_context& context) const -> void {
  fmt::format_to(context.out(), " Levels=[");

  bool is_first = true;
  record.visit_levels([&context, &is_first](auto level_idx, const auto& level) {
    fmt::format_to(context.out(),
                   "{}Level={{ Index={} Data={} }}",
                   is_first ? " " : ", ",
                   level_idx,
                   level);
    is_first = false;
  });

  fmt::format_to(context.out(), " ]");
}

auto fmt::formatter<simulator::generator::historical::Action>::format(
    const formattable& action, format_context& context) const
    -> decltype(context.out()) {
  fmt::format_to(context.out(), "Action={{");

  fmt::format_to(context.out(), " ActionTime={}", action.action_time());

  format_records(action, context);

  return fmt::format_to(context.out(), " }}");
}

auto fmt::formatter<simulator::generator::historical::Action>::format_records(
    const formattable& action, format_context& context) const -> void {
  fmt::format_to(context.out(), " Records=[");

  bool is_first = true;
  action.visit_records([&is_first, &context](const auto& record) {
    fmt::format_to(context.out(), "{}{}", is_first ? " " : ", ", record);
    is_first = false;
  });

  fmt::format_to(context.out(), " ]");
}
