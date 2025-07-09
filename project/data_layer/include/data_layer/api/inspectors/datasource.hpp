#ifndef SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_DATASOURCE_HPP_
#define SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_DATASOURCE_HPP_

#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include "data_layer/api/models/datasource.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class DatasourceReader final {
  using Attribute = Datasource::Attribute;

  template <typename T>
  constexpr static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit DatasourceReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const Datasource& datasource) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Marshaller>
class DatasourcePatchReader final {
  using Attribute = Datasource::Attribute;

  template <typename T>
  constexpr static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit DatasourcePatchReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const Datasource::Patch& patch) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class DatasourcePatchWriter final {
  using Attribute = Datasource::Attribute;

  template <typename T>
  constexpr static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit DatasourcePatchWriter(Unmarshaller& unmarshaller) noexcept
      : unmarshaller_(unmarshaller) {}

  auto write(Datasource::Patch& target_patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
inline auto DatasourceReader<Marshaller>::read(const Datasource& datasource)
    -> void {
  const auto datasource_id = datasource.datasource_id();
  static_assert(can_marshall_v<decltype(datasource_id)>);
  marshaller_(Attribute::DatasourceId, datasource_id);

  const auto& name = datasource.name();
  static_assert(can_marshall_v<decltype(name)>);
  marshaller_(Attribute::Name, name);

  const auto& venue_id = datasource.venue_id();
  static_assert(can_marshall_v<decltype(venue_id)>);
  marshaller_(Attribute::VenueId, venue_id);

  const auto& connection = datasource.connection();
  static_assert(can_marshall_v<decltype(connection)>);
  marshaller_(Attribute::Connection, connection);

  const auto format = datasource.format();
  static_assert(can_marshall_v<decltype(format)>);
  marshaller_(Attribute::Format, format);

  const auto type = datasource.type();
  static_assert(can_marshall_v<decltype(type)>);
  marshaller_(Attribute::Type, type);

  if (const auto value = datasource.enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Enabled, *value);
  }

  if (const auto value = datasource.repeat_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Repeat, *value);
  }

  if (const auto value = datasource.text_delimiter()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TextDelimiter, *value);
  }

  if (const auto value = datasource.text_header_row()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TextHeaderRow, *value);
  }

  if (const auto value = datasource.text_data_row()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TextDataRow, *value);
  }

  if (const auto& value = datasource.table_name()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TableName, *value);
  }

  if (const auto& value = datasource.max_depth_levels()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::MaxDepthLevels, *value);
  }
}

template <typename Marshaller>
inline auto DatasourcePatchReader<Marshaller>::read(
    const Datasource::Patch& patch) -> void {
  if (const auto& value = patch.enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Enabled, *value);
  }

  if (const auto& value = patch.name()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Name, *value);
  }

  if (const auto& value = patch.venue_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::VenueId, *value);
  }

  if (const auto& value = patch.connection()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Connection, *value);
  }

  if (const auto& value = patch.format()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Format, *value);
  }

  if (const auto& value = patch.type()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Type, *value);
  }

  if (const auto& value = patch.repeat_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Repeat, *value);
  }

  if (const auto& value = patch.text_delimiter()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TextDelimiter, *value);
  }

  if (const auto& value = patch.text_header_row()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TextHeaderRow, *value);
  }

  if (const auto& value = patch.text_data_row()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TextDataRow, *value);
  }

  if (const auto& value = patch.table_name()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::TableName, *value);
  }

  if (const auto& value = patch.max_depth_levels()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::MaxDepthLevels, *value);
  }
}

template <typename Unmarshaller>
inline auto DatasourcePatchWriter<Unmarshaller>::write(
    Datasource::Patch& target_patch) -> void {
  bool enabled_flag{};
  static_assert(can_unmarshall_v<decltype(enabled_flag)>);
  if (unmarshaller_(Attribute::Enabled, enabled_flag)) {
    target_patch.with_enabled_flag(enabled_flag);
  }

  std::string name{};
  static_assert(can_unmarshall_v<decltype(name)>);
  if (unmarshaller_(Attribute::Name, name)) {
    target_patch.with_name(std::move(name));
  }

  std::string venue_id{};
  static_assert(can_unmarshall_v<decltype(venue_id)>);
  if (unmarshaller_(Attribute::VenueId, venue_id)) {
    target_patch.with_venue_id(std::move(venue_id));
  }

  std::string connection{};
  static_assert(can_unmarshall_v<decltype(connection)>);
  if (unmarshaller_(Attribute::Connection, connection)) {
    target_patch.with_connection(std::move(connection));
  }

  Datasource::Format format{};
  static_assert(can_unmarshall_v<decltype(format)>);
  if (unmarshaller_(Attribute::Format, format)) {
    target_patch.with_format(format);
  }

  Datasource::Type type{};
  static_assert(can_unmarshall_v<decltype(type)>);
  if (unmarshaller_(Attribute::Type, type)) {
    target_patch.with_type(type);
  }

  bool repeat_flag{};
  static_assert(can_unmarshall_v<decltype(repeat_flag)>);
  if (unmarshaller_(Attribute::Repeat, repeat_flag)) {
    target_patch.with_repeat_flag(repeat_flag);
  }

  char text_delimiter{};
  static_assert(can_unmarshall_v<decltype(text_delimiter)>);
  if (unmarshaller_(Attribute::TextDelimiter, text_delimiter)) {
    target_patch.with_text_delimiter(text_delimiter);
  }

  std::uint64_t text_header_row{};
  static_assert(can_unmarshall_v<decltype(text_header_row)>);
  if (unmarshaller_(Attribute::TextHeaderRow, text_header_row)) {
    target_patch.with_text_header_row(text_header_row);
  }

  std::uint64_t text_data_row{};
  static_assert(can_unmarshall_v<decltype(text_data_row)>);
  if (unmarshaller_(Attribute::TextDataRow, text_data_row)) {
    target_patch.with_text_data_row(text_data_row);
  }

  std::string table_name{};
  static_assert(can_unmarshall_v<decltype(table_name)>);
  if (unmarshaller_(Attribute::TableName, table_name)) {
    target_patch.with_table_name(std::move(table_name));
  }

  std::uint32_t max_depth_levels;
  static_assert(can_unmarshall_v<decltype(max_depth_levels)>);
  if (unmarshaller_(Attribute::MaxDepthLevels, max_depth_levels)) {
    target_patch.with_max_depth_levels(max_depth_levels);
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_DATASOURCE_HPP_
