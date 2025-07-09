#ifndef SIMULATOR_PROTOCOL_ADMIN_GENERATOR_HPP_
#define SIMULATOR_PROTOCOL_ADMIN_GENERATOR_HPP_

#include <fmt/format.h>

#include <optional>
#include <string_view>

#include "core/common/std_formatter.hpp"

namespace simulator::protocol {

struct GenerationStatusRequest {};

struct GenerationStatusReply {
  enum class Status : std::uint8_t { Running, Stopped };

  std::optional<Status> status;
};

struct StartGenerationRequest {};

struct StartGenerationReply {
  enum class Result : std::uint8_t { Started, ErrorOccurred };

  std::optional<Result> result;
};

struct StopGenerationRequest {};

struct StopGenerationReply {
  enum class Result : std::uint8_t { Stopped, ErrorOccurred };

  std::optional<Result> result;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::GenerationStatusReply::Status>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::GenerationStatusReply::Status;

  auto format(formattable status, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::GenerationStatusRequest> {
  using formattable = simulator::protocol::GenerationStatusRequest;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); };

  auto format(const formattable& request, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::GenerationStatusReply> {
  using formattable = simulator::protocol::GenerationStatusReply;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); };

  auto format(const formattable& reply, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::StartGenerationReply::Result>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::StartGenerationReply::Result;

  auto format(formattable result, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::StartGenerationRequest> {
  using formattable = simulator::protocol::StartGenerationRequest;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& request, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::StartGenerationReply> {
  using formattable = simulator::protocol::StartGenerationReply;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& reply, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::StopGenerationReply::Result>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::StopGenerationReply::Result;

  auto format(formattable result, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::StopGenerationRequest> {
  using formattable = simulator::protocol::StopGenerationRequest;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& request, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::StopGenerationReply> {
  using formattable = simulator::protocol::StopGenerationReply;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& reply, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_ADMIN_GENERATOR_HPP_