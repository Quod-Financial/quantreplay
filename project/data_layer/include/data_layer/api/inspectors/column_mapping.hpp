#ifndef SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_COLUMN_MAPPING_HPP_
#define SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_COLUMN_MAPPING_HPP_

#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include "data_layer/api/models/column_mapping.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class ColumnMappingReader final {
  using Attribute = ColumnMapping::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit ColumnMappingReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const ColumnMapping& column_mapping) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class ColumnMappingPatchWriter final {
  using Attribute = ColumnMapping::Attribute;

  template <typename T>
  constexpr inline static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit ColumnMappingPatchWriter(Unmarshaller& unmarshaller) noexcept
      : unmarshaller_(unmarshaller) {}

  auto write(ColumnMapping::Patch& patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
inline auto ColumnMappingReader<Marshaller>::read(
    const ColumnMapping& column_mapping) -> void {
  static_assert(can_marshall_v<decltype(column_mapping.datasource_id())>);
  marshaller_(Attribute::DatasourceId, column_mapping.datasource_id());

  static_assert(can_marshall_v<decltype(column_mapping.column_from())>);
  marshaller_(Attribute::ColumnFrom, column_mapping.column_from());

  static_assert(can_marshall_v<decltype(column_mapping.column_to())>);
  marshaller_(Attribute::ColumnTo, column_mapping.column_to());
}

template <typename Unmarshaller>
inline auto ColumnMappingPatchWriter<Unmarshaller>::write(
    ColumnMapping::Patch& patch) -> void {
  std::string column_from;
  static_assert(can_unmarshall_v<decltype(column_from)>);
  if (unmarshaller_(Attribute::ColumnFrom, column_from)) {
    patch.with_column_from(std::move(column_from));
  }

  std::string column_to;
  static_assert(can_unmarshall_v<decltype(column_to)>);
  if (unmarshaller_(Attribute::ColumnTo, column_to)) {
    patch.with_column_to(std::move(column_to));
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_COLUMN_MAPPING_HPP_
