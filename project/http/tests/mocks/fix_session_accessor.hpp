#ifndef SIMULATOR_HTTP_TESTS_MOCKS_FIX_SESSION_ACCESSOR_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_FIX_SESSION_ACCESSOR_HPP_

#include <gmock/gmock.h>

#include <string>
#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/models/fix_session.hpp"
#include "ih/data_bridge/fix_session_accessor.hpp"
#include "ih/data_bridge/operation_failure.hpp"

namespace simulator::http::mock {

class FixSessionAccessor : public data_bridge::FixSessionAccessor {
  template <typename T>
  using Result = tl::expected<T, data_bridge::Failure>;
  using FixSession = data_layer::FixSession;
  using Patch = FixSession::Patch;

 public:
  using FixSessionsResult = Result<std::vector<FixSession>>;
  using EmptyResult = Result<void>;

  MOCK_METHOD(FixSessionsResult,
              select_all,
              (const std::string&),
              (const, noexcept, override));

  MOCK_METHOD(EmptyResult, add, (Patch), (const, noexcept, override));

  MOCK_METHOD(EmptyResult,
              update,
              (Patch, const std::string&, const std::string&),
              (const, noexcept, override));

  MOCK_METHOD(EmptyResult,
              delete_all,
              (const std::string&, const std::vector<std::string>&),
              (const, noexcept, override));
};

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_FIX_SESSION_ACCESSOR_HPP_
