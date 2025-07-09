#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_DATASOURCE_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_DATASOURCE_HPP_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "data_layer/api/models/column_mapping.hpp"
#include "data_layer/api/predicate/definitions.hpp"

namespace simulator::data_layer {

class Datasource {
 public:
  class Patch;

  using Predicate = predicate::Expression<Datasource>;

  enum class Attribute {
    DatasourceId,
    Enabled,
    Name,
    VenueId,
    Connection,
    Format,
    Type,
    Repeat,
    TextDelimiter,
    TextHeaderRow,
    TextDataRow,
    TableName,
    MaxDepthLevels
  };

  enum class Format { Csv, Postgres };

  enum class Type { OrderBook };

  [[nodiscard]]
  static auto create(Datasource::Patch snapshot, std::uint64_t datasource_id)
      -> Datasource;

  [[nodiscard]]
  auto datasource_id() const noexcept -> std::uint64_t;

  [[nodiscard]]
  auto enabled_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto name() const noexcept -> const std::string&;

  [[nodiscard]]
  auto venue_id() const noexcept -> const std::string&;

  [[nodiscard]]
  auto connection() const noexcept -> const std::string&;

  [[nodiscard]]
  auto format() const noexcept -> Format;

  [[nodiscard]]
  auto type() const noexcept -> Type;

  [[nodiscard]]
  auto repeat_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto text_delimiter() const noexcept -> std::optional<char>;

  [[nodiscard]]
  auto text_header_row() const noexcept -> std::optional<std::uint64_t>;

  [[nodiscard]]
  auto text_data_row() const noexcept -> std::optional<std::uint64_t>;

  [[nodiscard]]
  auto table_name() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto columns_mapping() const noexcept -> const std::vector<ColumnMapping>&;

  [[nodiscard]]
  auto max_depth_levels() const noexcept -> std::optional<std::uint32_t>;

 private:
  Datasource() = default;

  std::optional<std::string> table_name_;

  std::string name_;
  std::string venue_id_;
  std::string connection_;

  std::vector<ColumnMapping> columns_mapping_;

  std::optional<std::uint64_t> text_header_row_;
  std::optional<std::uint64_t> text_data_row_;
  std::optional<std::uint32_t> max_depth_levels_;

  std::uint64_t datasource_id_{0};

  Format format_{0};
  Type type_{0};

  std::optional<char> text_delimiter_;
  std::optional<bool> enabled_flag_;
  std::optional<bool> repeat_flag_;
};

class Datasource::Patch {
  friend class Datasource;

 public:
  using Attribute = Datasource::Attribute;

  [[nodiscard]]
  auto enabled_flag() const noexcept -> std::optional<bool>;
  auto with_enabled_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto name() const noexcept -> const std::optional<std::string>&;
  auto with_name(std::string name) noexcept -> Patch&;

  [[nodiscard]]
  auto venue_id() const noexcept -> const std::optional<std::string>&;
  auto with_venue_id(std::string venue_id) noexcept -> Patch&;

  [[nodiscard]]
  auto connection() const noexcept -> const std::optional<std::string>&;
  auto with_connection(std::string connection) noexcept -> Patch&;

  [[nodiscard]]
  auto format() const noexcept -> std::optional<Format>;
  auto with_format(Format format) noexcept -> Patch&;

  [[nodiscard]]
  auto type() const noexcept -> std::optional<Type>;
  auto with_type(Type type) noexcept -> Patch&;

  [[nodiscard]]
  auto repeat_flag() const noexcept -> std::optional<bool>;
  auto with_repeat_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto text_delimiter() const noexcept -> std::optional<char>;
  auto with_text_delimiter(char delimiter) noexcept -> Patch&;

  [[nodiscard]]
  auto text_header_row() const noexcept -> std::optional<std::uint64_t>;
  auto with_text_header_row(std::uint64_t row) noexcept -> Patch&;

  [[nodiscard]]
  auto text_data_row() const noexcept -> std::optional<std::uint64_t>;
  auto with_text_data_row(std::uint64_t row) noexcept -> Patch&;

  [[nodiscard]]
  auto table_name() const -> const std::optional<std::string>&;
  auto with_table_name(std::string table_name) noexcept -> Patch&;

  [[nodiscard]]
  auto columns_mapping() const noexcept
      -> const std::optional<std::vector<ColumnMapping::Patch>>&;
  auto with_column_mapping(ColumnMapping::Patch patch_snapshot) -> Patch&;
  auto without_column_mapping() noexcept -> Patch&;

  [[nodiscard]]
  auto max_depth_levels() const noexcept -> std::optional<std::uint32_t>;
  auto with_max_depth_levels(std::uint32_t levels) noexcept -> Patch&;

 private:
  std::optional<std::string> table_name_;
  std::optional<std::string> name_;
  std::optional<std::string> venue_id_;
  std::optional<std::string> connection_;

  std::optional<std::vector<ColumnMapping::Patch>> columns_mapping_;

  std::optional<std::uint64_t> text_header_row_;
  std::optional<std::uint64_t> text_data_row_;
  std::optional<std::uint32_t> max_depth_levels_;

  std::optional<Format> format_;
  std::optional<Type> type_;

  std::optional<char> text_delimiter_;
  std::optional<bool> enabled_flag_;
  std::optional<bool> repeat_flag_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_DATASOURCE_HPP_
