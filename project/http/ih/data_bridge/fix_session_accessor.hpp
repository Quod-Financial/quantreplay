#ifndef SIMULATOR_HTTP_IH_DATA_BRIDGE_FIX_SESSION_ACCESSOR_HPP_
#define SIMULATOR_HTTP_IH_DATA_BRIDGE_FIX_SESSION_ACCESSOR_HPP_

#include <string>
#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/database/context.hpp"
#include "data_layer/api/models/fix_session.hpp"
#include "ih/data_bridge/operation_failure.hpp"

namespace simulator::http::data_bridge {

class FixSessionAccessor {
 public:
  FixSessionAccessor() = default;
  FixSessionAccessor(const FixSessionAccessor&) = default;
  FixSessionAccessor(FixSessionAccessor&&) noexcept = default;

  auto operator=(const FixSessionAccessor&) -> FixSessionAccessor& = default;
  auto operator=(FixSessionAccessor&&) noexcept
      -> FixSessionAccessor& = default;

  virtual ~FixSessionAccessor() = default;

  [[nodiscard]]
  virtual auto select_all(const std::string& venue_id) const noexcept
      -> tl::expected<std::vector<data_layer::FixSession>, Failure> = 0;

  [[nodiscard]]
  virtual auto add(data_layer::FixSession::Patch snapshot) const noexcept
      -> tl::expected<void, Failure> = 0;

  [[nodiscard]]
  virtual auto update(data_layer::FixSession::Patch update,
                      const std::string& venue_id,
                      const std::string& session_id) const noexcept
      -> tl::expected<void, Failure> = 0;

  [[nodiscard]]
  virtual auto delete_all(const std::string& venue_id,
                          const std::vector<std::string>& session_ids)
      const noexcept -> tl::expected<void, Failure> = 0;
};

class DataLayerFixSessionAccessor final : public FixSessionAccessor {
 public:
  using DbContext = data_layer::database::Context;

  DataLayerFixSessionAccessor() = delete;

  explicit DataLayerFixSessionAccessor(DbContext context) noexcept;

  [[nodiscard]]
  auto select_all(const std::string& venue_id) const noexcept
      -> tl::expected<std::vector<data_layer::FixSession>, Failure> override;

  [[nodiscard]]
  auto add(data_layer::FixSession::Patch snapshot) const noexcept
      -> tl::expected<void, Failure> override;

  [[nodiscard]]
  auto update(data_layer::FixSession::Patch update,
              const std::string& venue_id,
              const std::string& session_id) const noexcept
      -> tl::expected<void, Failure> override;

  [[nodiscard]]
  auto delete_all(const std::string& venue_id,
                  const std::vector<std::string>& session_ids) const noexcept
      -> tl::expected<void, Failure> override;

 private:
  data_layer::database::Context context_;
};

}  // namespace simulator::http::data_bridge

#endif  // SIMULATOR_HTTP_IH_DATA_BRIDGE_FIX_SESSION_ACCESSOR_HPP_
