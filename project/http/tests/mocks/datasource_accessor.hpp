#ifndef SIMULATOR_HTTP_TESTS_MOCKS_DATASOURCE_ACCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_DATASOURCE_ACCESSOR_HPP_

#include <gmock/gmock.h>

#include <cstdint>
#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/models/datasource.hpp"
#include "ih/data_bridge/datasource_accessor.hpp"
#include "ih/data_bridge/operation_failure.hpp"

namespace simulator::http::mock {

class DatasourceAccessor : public data_bridge::DatasourceAccessor {
  template <typename T>
  using Result = tl::expected<T, data_bridge::FailureInfo>;
  using Datasource = data_layer::Datasource;
  using Patch = Datasource::Patch;

 public:
  using DatasourceResult = Result<Datasource>;
  using DatasourcesResult = Result<std::vector<Datasource>>;

  MOCK_METHOD(DatasourceResult,
              select_single,
              (std::uint64_t),
              (const, noexcept, override));

  MOCK_METHOD(DatasourcesResult, select_all, (), (const, noexcept, override));

  MOCK_METHOD(DatasourceResult, add, (Patch), (const, noexcept, override));

  MOCK_METHOD(DatasourceResult,
              update,
              (Patch, std::uint64_t),
              (const, noexcept, override));
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_DATASOURCE_ACCESSOR_HPP_
