#include "api/models/datasource.hpp"

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <vector>

#include "ih/common/exceptions.hpp"

namespace simulator::data_layer {

auto Datasource::create(Datasource::Patch snapshot, std::uint64_t datasource_id)
    -> Datasource {
  if (!snapshot.name_.has_value()) {
    throw RequiredAttributeMissing("Datasource", "Name");
  }
  if (!snapshot.venue_id_.has_value()) {
    throw RequiredAttributeMissing("Datasource", "VenueID");
  }
  if (!snapshot.connection_.has_value()) {
    throw RequiredAttributeMissing("Datasource", "Connection");
  }
  if (!snapshot.format_.has_value()) {
    throw RequiredAttributeMissing("Datasource", "Format");
  }
  if (!snapshot.type_.has_value()) {
    throw RequiredAttributeMissing("Datasource", "Type");
  }

  Datasource datasource{};
  datasource.datasource_id_ = datasource_id;

#define SIM_ASSIGN_FIELD(VENUE_FIELD, SNAPSHOT_FIELD)               \
  if (snapshot.SNAPSHOT_FIELD.has_value()) {                        \
    datasource.VENUE_FIELD = std::move(*(snapshot.SNAPSHOT_FIELD)); \
  }                                                                 \
  void(0)

  SIM_ASSIGN_FIELD(table_name_, table_name_);
  SIM_ASSIGN_FIELD(name_, name_);
  SIM_ASSIGN_FIELD(venue_id_, venue_id_);
  SIM_ASSIGN_FIELD(connection_, connection_);
  SIM_ASSIGN_FIELD(text_header_row_, text_header_row_);
  SIM_ASSIGN_FIELD(text_data_row_, text_data_row_);
  SIM_ASSIGN_FIELD(format_, format_);
  SIM_ASSIGN_FIELD(type_, type_);
  SIM_ASSIGN_FIELD(text_delimiter_, text_delimiter_);
  SIM_ASSIGN_FIELD(enabled_flag_, enabled_flag_);
  SIM_ASSIGN_FIELD(repeat_flag_, repeat_flag_);
  SIM_ASSIGN_FIELD(max_depth_levels_, max_depth_levels_);

#undef SIM_ASSIGN_FIELD

  if (auto& columns_mapping = snapshot.columns_mapping_) {
    datasource.columns_mapping_.reserve(columns_mapping->size());
    std::transform(
        std::make_move_iterator(columns_mapping->begin()),
        std::make_move_iterator(columns_mapping->end()),
        std::back_inserter(datasource.columns_mapping_),
        [datasource_id](ColumnMapping::Patch&& patch) -> ColumnMapping {
          return ColumnMapping::create(std::move(patch), datasource_id);
        });
  }

  return datasource;
}

auto Datasource::datasource_id() const noexcept -> std::uint64_t {
  return datasource_id_;
}

auto Datasource::enabled_flag() const noexcept -> std::optional<bool> {
  return enabled_flag_;
}

auto Datasource::name() const noexcept -> const std::string& { return name_; }

auto Datasource::venue_id() const noexcept -> const std::string& {
  return venue_id_;
}

auto Datasource::connection() const noexcept -> const std::string& {
  return connection_;
}

auto Datasource::format() const noexcept -> Format { return format_; }

auto Datasource::type() const noexcept -> Type { return type_; }

auto Datasource::repeat_flag() const noexcept -> std::optional<bool> {
  return repeat_flag_;
}

auto Datasource::text_delimiter() const noexcept -> std::optional<char> {
  return text_delimiter_;
}

auto Datasource::text_header_row() const noexcept
    -> std::optional<std::uint64_t> {
  return text_header_row_;
}

auto Datasource::text_data_row() const noexcept
    -> std::optional<std::uint64_t> {
  return text_data_row_;
}

auto Datasource::table_name() const noexcept
    -> const std::optional<std::string>& {
  return table_name_;
}

auto Datasource::columns_mapping() const noexcept
    -> const std::vector<ColumnMapping>& {
  return columns_mapping_;
}

auto Datasource::max_depth_levels() const noexcept
    -> std::optional<std::uint32_t> {
  return max_depth_levels_;
}

auto Datasource::Patch::enabled_flag() const noexcept -> std::optional<bool> {
  return enabled_flag_;
}

auto Datasource::Patch::with_enabled_flag(bool flag) noexcept -> Patch& {
  enabled_flag_ = flag;
  return *this;
}

auto Datasource::Patch::name() const noexcept
    -> const std::optional<std::string>& {
  return name_;
}

auto Datasource::Patch::with_name(std::string name) noexcept -> Patch& {
  name_ = std::move(name);
  return *this;
}

auto Datasource::Patch::venue_id() const noexcept
    -> const std::optional<std::string>& {
  return venue_id_;
}

auto Datasource::Patch::with_venue_id(std::string venue_id) noexcept -> Patch& {
  venue_id_ = std::move(venue_id);
  return *this;
}

auto Datasource::Patch::connection() const noexcept
    -> const std::optional<std::string>& {
  return connection_;
}

auto Datasource::Patch::with_connection(std::string connection) noexcept
    -> Patch& {
  connection_ = std::move(connection);
  return *this;
}

auto Datasource::Patch::format() const noexcept -> std::optional<Format> {
  return format_;
}

auto Datasource::Patch::with_format(Format format) noexcept -> Patch& {
  format_ = format;
  return *this;
}

auto Datasource::Patch::type() const noexcept -> std::optional<Type> {
  return type_;
}

auto Datasource::Patch::with_type(Type type) noexcept -> Patch& {
  type_ = type;
  return *this;
}

auto Datasource::Patch::repeat_flag() const noexcept -> std::optional<bool> {
  return repeat_flag_;
}

auto Datasource::Patch::with_repeat_flag(bool flag) noexcept -> Patch& {
  repeat_flag_ = flag;
  return *this;
}

auto Datasource::Patch::text_delimiter() const noexcept -> std::optional<char> {
  return text_delimiter_;
}

auto Datasource::Patch::with_text_delimiter(char delimiter) noexcept -> Patch& {
  text_delimiter_ = delimiter;
  return *this;
}

auto Datasource::Patch::text_header_row() const noexcept
    -> std::optional<std::uint64_t> {
  return text_header_row_;
}

auto Datasource::Patch::with_text_header_row(std::uint64_t row) noexcept
    -> Patch& {
  text_header_row_ = row;
  return *this;
}

auto Datasource::Patch::text_data_row() const noexcept
    -> std::optional<std::uint64_t> {
  return text_data_row_;
}

auto Datasource::Patch::with_text_data_row(std::uint64_t row) noexcept
    -> Patch& {
  text_data_row_ = row;
  return *this;
}

auto Datasource::Patch::table_name() const
    -> const std::optional<std::string>& {
  return table_name_;
}

auto Datasource::Patch::with_table_name(std::string table_name) noexcept
    -> Patch& {
  table_name_ = std::move(table_name);
  return *this;
}

auto Datasource::Patch::columns_mapping() const noexcept
    -> const std::optional<std::vector<ColumnMapping::Patch>>& {
  return columns_mapping_;
}

auto Datasource::Patch::with_column_mapping(ColumnMapping::Patch patch_snapshot)
    -> Patch& {
  if (!columns_mapping_.has_value()) {
    columns_mapping_ = decltype(columns_mapping_)::value_type{};
  }
  columns_mapping_->emplace_back(std::move(patch_snapshot));
  return *this;
}

auto Datasource::Patch::without_column_mapping() noexcept -> Patch& {
  columns_mapping_ = decltype(columns_mapping_)::value_type{};
  return *this;
}

auto Datasource::Patch::max_depth_levels() const noexcept
    -> std::optional<std::uint32_t> {
  return max_depth_levels_;
}

auto Datasource::Patch::with_max_depth_levels(std::uint32_t levels) noexcept
    -> Patch& {
  max_depth_levels_ = levels;
  return *this;
}

}  // namespace simulator::data_layer
