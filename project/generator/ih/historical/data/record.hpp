#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_RECORD_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_RECORD_HPP_

#include <fmt/format.h>

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "ih/historical/data/time.hpp"

namespace simulator::generator::historical {

class Level {
 public:
  class Builder;

  [[nodiscard]]
  auto bid_price() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto bid_quantity() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto bid_counterparty() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto offer_price() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto offer_quantity() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto offer_counterparty() const noexcept -> const std::optional<std::string>&;

 private:
  std::optional<std::string> bid_counterparty_;
  std::optional<std::string> offer_counterparty_;

  std::optional<double> bid_price_;
  std::optional<double> offer_price_;

  std::optional<double> bid_quantity_;
  std::optional<double> offer_quantity_;
};

class Level::Builder {
 public:
  auto with_bid_price(double px) noexcept -> Builder&;

  auto with_bid_quantity(double qty) noexcept -> Builder&;

  auto with_bid_counterparty(std::string cp) noexcept -> Builder&;

  auto with_offer_price(double px) noexcept -> Builder&;

  auto with_offer_quantity(double qty) noexcept -> Builder&;

  auto with_offer_counterparty(std::string cp) noexcept -> Builder&;

  [[nodiscard]]
  auto empty() const noexcept -> bool;

  [[nodiscard]]
  static auto construct(Builder builder) noexcept -> Level;

 private:
  std::optional<std::string> bid_counterparty_;
  std::optional<std::string> offer_counterparty_;

  std::optional<double> bid_price_;
  std::optional<double> offer_price_;

  std::optional<double> bid_quantity_;
  std::optional<double> offer_quantity_;
};

class Record {
 public:
  class Builder;

  using LevelStealer = std::function<void(std::uint64_t, Level)>;
  using LevelVisitor = std::function<void(std::uint64_t, const Level&)>;

  [[nodiscard]]
  auto instrument() const noexcept -> const std::string&;

  [[nodiscard]]
  auto receive_time() const noexcept -> historical::Timepoint;

  [[nodiscard]]
  auto message_time() const noexcept -> std::optional<historical::Timepoint>;

  [[nodiscard]]
  auto source_connection() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto source_name() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto source_row() const noexcept -> std::uint64_t;

  [[nodiscard]]
  auto has_levels() const noexcept -> bool;

  auto steal_levels(const LevelStealer& stealer) -> void;

  auto visit_levels(const LevelVisitor& visitor) const -> void;

 private:
  Record() = default;

  std::vector<Level> levels_;

  std::optional<std::string> source_name_;
  std::optional<std::string> source_conn_;
  std::string instrument_;

  std::optional<historical::Timepoint> message_time_;
  historical::Timepoint received_time_;

  std::uint64_t source_row_{0};
};

class Record::Builder {
 public:
  Builder() = default;

  explicit Builder(Record base_record) noexcept;

  auto with_instrument(std::string instrument) noexcept -> Builder&;

  auto with_receive_time(historical::Timepoint receive_time) noexcept
      -> Builder&;

  auto with_message_time(historical::Timepoint message_time) noexcept
      -> Builder&;

  auto with_source_name(std::string source_name) noexcept -> Builder&;

  auto with_source_connection(std::string source_conn) noexcept -> Builder&;

  auto with_source_row(std::uint64_t source_row) noexcept -> Builder&;

  auto add_level(std::uint64_t index, Level level) -> Builder&;

  static auto construct(Builder builder) -> Record;

 private:
  static auto validate(const Builder& builder) -> void;

  std::optional<std::string> instrument_;
  std::optional<std::string> source_name_;
  std::optional<std::string> source_conn_;

  std::vector<Level> levels_;

  std::optional<historical::Timepoint> message_time_;
  std::optional<historical::Timepoint> received_time_;

  std::optional<std::uint64_t> source_row_;
};

class Action {
 public:
  class Builder;

  using RecordStealer = std::function<void(Record)>;
  using RecordVisitor = std::function<void(const Record&)>;

  [[nodiscard]]
  auto action_time() const noexcept -> historical::Timepoint;

  auto steal_records(const RecordStealer& stealer) -> void;

  auto visit_records(const RecordVisitor& visitor) const -> void;

  static auto update_time(Action base_action, historical::Timepoint action_time)
      -> Action;

 private:
  Action() = default;

  static auto update_timepoints(Record&& record,
                                historical::Duration time_offset) -> Record;

  std::vector<Record> records_;

  historical::Timepoint action_time_;
};

class Action::Builder {
 public:
  auto add(Record record, historical::Duration time_offset) -> void;

  static auto construct(Action::Builder builder) -> Action;

 private:
  auto validate(const Record& record) const -> void;

  std::vector<Record> records_;
};

auto operator<<(std::ostream& os, const Level& level) -> std::ostream&;

auto operator<<(std::ostream& os, const Record& record) -> std::ostream&;

auto operator<<(std::ostream& os, const Action& action) -> std::ostream&;

}  // namespace simulator::generator::historical

template <>
struct fmt::formatter<simulator::generator::historical::Level>
    : fmt::formatter<std::string_view> {
  using formattable = simulator::generator::historical::Level;

  auto format(const formattable& level, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::generator::historical::Record>
    : fmt::formatter<std::string_view> {
  using formattable = simulator::generator::historical::Record;

  auto format(const formattable& record, format_context& context) const
      -> decltype(context.out());

 private:
  auto format_levels(const formattable& record, format_context& context) const
      -> void;
};

template <>
struct fmt::formatter<simulator::generator::historical::Action>
    : fmt::formatter<std::string_view> {
  using formattable = simulator::generator::historical::Action;

  auto format(const formattable& action, format_context& context) const
      -> decltype(context.out());

 private:
  auto format_records(const formattable& action, format_context& context) const
      -> void;
};

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_RECORD_HPP_
