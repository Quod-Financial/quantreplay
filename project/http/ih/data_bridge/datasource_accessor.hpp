#ifndef SIMULATOR_HTTP_IH_DATA_BRIDGE_DATASOURCE_ACCESSOR_HPP_
#define SIMULATOR_HTTP_IH_DATA_BRIDGE_DATASOURCE_ACCESSOR_HPP_

#include <cstdint>
#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/database/context.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "ih/data_bridge/operation_failure.hpp"

namespace simulator::http::data_bridge {

class DatasourceAccessor {
 public:
  DatasourceAccessor() = default;
  DatasourceAccessor(const DatasourceAccessor&) = default;
  DatasourceAccessor(DatasourceAccessor&&) noexcept = default;

  auto operator=(const DatasourceAccessor&) -> DatasourceAccessor& = default;
  auto operator=(DatasourceAccessor&&) noexcept
      -> DatasourceAccessor& = default;

  virtual ~DatasourceAccessor() = default;

  [[nodiscard]]
  virtual auto select_single(std::uint64_t datasource_id) const noexcept
      -> tl::expected<data_layer::Datasource, FailureInfo> = 0;

  [[nodiscard]]
  virtual auto select_all() const noexcept
      -> tl::expected<std::vector<data_layer::Datasource>, FailureInfo> = 0;

  [[nodiscard]]
  virtual auto add(data_layer::Datasource::Patch snapshot) const noexcept
      -> tl::expected<void, FailureInfo> = 0;

  [[nodiscard]]
  virtual auto update(data_layer::Datasource::Patch update,
                      std::uint64_t datasource_id) const noexcept
      -> tl::expected<void, FailureInfo> = 0;
};

class DataLayerDatasourceAccessor final : public DatasourceAccessor {
 public:
  using DbContext = data_layer::database::Context;

  DataLayerDatasourceAccessor() = delete;

  explicit DataLayerDatasourceAccessor(DbContext context) noexcept;

  [[nodiscard]]
  auto select_single(std::uint64_t datasource_id) const noexcept
      -> tl::expected<data_layer::Datasource, FailureInfo> override;

  [[nodiscard]]
  auto select_all() const noexcept
      -> tl::expected<std::vector<data_layer::Datasource>,
                      FailureInfo> override;

  [[nodiscard]]
  auto add(data_layer::Datasource::Patch snapshot) const noexcept
      -> tl::expected<void, FailureInfo> override;

  [[nodiscard]]
  auto update(data_layer::Datasource::Patch update,
              std::uint64_t datasource_id) const noexcept
      -> tl::expected<void, FailureInfo> override;

 private:
  data_layer::database::Context context_;
};

}  // namespace simulator::http::data_bridge

#endif  // SIMULATOR_HTTP_IH_DATA_BRIDGE_DATASOURCE_ACCESSOR_HPP_
