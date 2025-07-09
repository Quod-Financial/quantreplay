#ifndef SIMULATOR_PROTOCOL_ADMIN_TRADING_PHASE_HPP_
#define SIMULATOR_PROTOCOL_ADMIN_TRADING_PHASE_HPP_

#include <fmt/format.h>

#include <cstdint>
#include <optional>

namespace simulator::protocol {

struct HaltPhaseRequest {
  bool allow_cancels = false;
};

struct HaltPhaseReply {
  enum class Result : std::uint8_t {
    Halted,
    AlreadyHaltedByRequest,
    NoActivePhase,
    UnableToHalt
  };

  std::optional<Result> result;
};

struct ResumePhaseRequest {};

struct ResumePhaseReply {
  enum class Result : std::uint8_t { Resumed, NoRequestedHalt };

  std::optional<Result> result;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::HaltPhaseRequest> {
  using formattable = simulator::protocol::HaltPhaseRequest;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); };

  auto format(formattable request, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::HaltPhaseReply::Result>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::HaltPhaseReply::Result;

  auto format(formattable result, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::HaltPhaseReply> {
  using formattable = simulator::protocol::HaltPhaseReply;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); };

  auto format(formattable reply, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::ResumePhaseRequest> {
  using formattable = simulator::protocol::ResumePhaseRequest;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); };

  auto format(formattable request, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::ResumePhaseReply::Result>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::ResumePhaseReply::Result;

  auto format(formattable result, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::ResumePhaseReply> {
  using formattable = simulator::protocol::ResumePhaseReply;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); };

  auto format(formattable reply, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_ADMIN_TRADING_PHASE_HPP_
