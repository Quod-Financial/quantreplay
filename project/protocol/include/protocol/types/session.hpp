#ifndef SIMULATOR_PROTOCOL_TYPES_SESSION_HPP_
#define SIMULATOR_PROTOCOL_TYPES_SESSION_HPP_

#include <fmt/base.h>

#include <concepts>
#include <optional>
#include <utility>
#include <variant>

#include "core/common/attribute.hpp"
#include "core/common/name.hpp"

namespace simulator::protocol::fix::tag {

struct BeginString {
  constexpr static core::Name name{.singular = "BeginString",
                                   .plural = "BeginStrings"};
};

struct SenderCompId {
  constexpr static core::Name name{.singular = "SenderCompID",
                                   .plural = "SenderCompIDs"};
};

struct TargetCompId {
  constexpr static core::Name name{.singular = "TargetCompID",
                                   .plural = "TargetCompIDs"};
};

struct ClientSubId {
  constexpr static core::Name name{.singular = "ClientSubID",
                                   .plural = "ClientSubIDs"};
};

}  // namespace simulator::protocol::fix::tag

SIMULATOR_DECLARE_ATTRIBUTE(simulator::protocol::fix, BeginString, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::protocol::fix, SenderCompId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::protocol::fix, TargetCompId, Literal);
SIMULATOR_DECLARE_ATTRIBUTE(simulator::protocol::fix, ClientSubId, Literal);

namespace simulator::protocol {
namespace fix {

// An internal market simulator descriptor for a client trading session
// established via FIX protocol
struct Session {
  Session(BeginString fix_begin_string,
          SenderCompId fix_sender_comp_id,
          TargetCompId fix_target_comp_id) noexcept;

  Session(BeginString fix_begin_string,
          SenderCompId fix_sender_comp_id,
          TargetCompId fix_target_comp_id,
          ClientSubId fix_client_sub_id) noexcept;

  [[nodiscard]] auto operator==(const Session& other) const noexcept -> bool {
    return begin_string == other.begin_string &&
           sender_comp_id == other.sender_comp_id &&
           target_comp_id == other.target_comp_id;
  }

  [[nodiscard]] consteval static auto name() noexcept -> core::Name {
    return {.singular = "FixSession", .plural = "FixSessions"};
  }

  BeginString begin_string;
  SenderCompId sender_comp_id;
  TargetCompId target_comp_id;
  std::optional<ClientSubId> client_sub_id;
};

}  // namespace fix

namespace generator {

// An internal market simulator descriptor for a trading session,
// dedicated to the Generator
struct Session {
  [[nodiscard]] constexpr auto operator==(Session /*other*/) const noexcept
      -> bool {
    // Generator session objects always represent the same session
    return true;
  }

  [[nodiscard]] consteval static auto name() noexcept -> core::Name {
    return {.singular = "GeneratorSession", .plural = "GeneratorSessions"};
  }
};

}  // namespace generator

struct Session {
  using ValueType = std::variant<fix::Session, generator::Session>;

  template <typename SessionType>
    requires std::constructible_from<ValueType, SessionType>
  explicit Session(SessionType&& session)
      : value(std::forward<SessionType>(session)) {}

  Session() = delete;
  Session(const Session&) = default;
  Session(Session&&) = default;
  ~Session() = default;

  auto operator=(const Session&) -> Session& = default;
  auto operator=(Session&&) -> Session& = default;

  auto operator==(const Session& other) const -> bool = default;

  ValueType value;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::fix::Session> {
  using formattable = simulator::protocol::fix::Session;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& session, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::Session> {
  using formattable = simulator::protocol::Session;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& session, format_context& context) const
      -> decltype(context.out());

 private:
  static auto format_session(const simulator::protocol::fix::Session& session,
                             format_context& context)
      -> decltype(context.out());

  static auto format_session(
      const simulator::protocol::generator::Session& session,
      format_context& context) -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_TYPES_SESSION_HPP_