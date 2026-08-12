#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_FIX_SESSION_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_FIX_SESSION_HPP_

#include <optional>
#include <string>

#include "core/tools/time.hpp"
#include "data_layer/api/predicate/definitions.hpp"

namespace simulator::data_layer {

class FixSession {
 public:
  class Patch;

  using Predicate = predicate::Expression<FixSession>;

  enum class Attribute { VenueId, SessionId, LastConnectedTime };

  static auto create(Patch snapshot) -> FixSession;

  [[nodiscard]]
  auto venue_id() const noexcept -> const std::string&;

  [[nodiscard]]
  auto session_id() const noexcept -> const std::string&;

  [[nodiscard]]
  auto last_connected_time() const noexcept
      -> const std::optional<core::sys_us>&;

 private:
  FixSession() = default;

  std::string venue_id_;
  std::string session_id_;
  std::optional<core::sys_us> last_connected_time_;
};

class FixSession::Patch {
  friend class FixSession;

 public:
  using Attribute = FixSession::Attribute;

  [[nodiscard]]
  auto venue_id() const noexcept -> const std::optional<std::string>&;
  auto with_venue_id(std::string venue_id) noexcept -> Patch&;

  [[nodiscard]]
  auto session_id() const noexcept -> const std::optional<std::string>&;
  auto with_session_id(std::string session_id) noexcept -> Patch&;

  [[nodiscard]]
  auto last_connected_time() const noexcept
      -> const std::optional<core::sys_us>&;
  auto with_last_connected_time(core::sys_us last_connected_time) noexcept
      -> Patch&;

 private:
  std::optional<std::string> venue_id_;
  std::optional<std::string> session_id_;
  std::optional<core::sys_us> last_connected_time_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_FIX_SESSION_HPP_
