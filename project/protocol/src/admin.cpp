#include <fmt/format.h>

#include <string_view>

#include "core/common/std_formatter.hpp"
#include "protocol/admin/generator.hpp"
#include "protocol/admin/market_state.hpp"
#include "protocol/admin/trading_phase.hpp"

namespace protocol = simulator::protocol;

auto fmt::formatter<protocol::GenerationStatusReply::Status>::format(
    formattable status, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  switch (status) {
    case formattable::Running:
      return base_formatter::format("Running", context);
    case formattable::Stopped:
      return base_formatter::format("Stopped", context);
  }
  return base_formatter::format("undefined", context);
}

auto fmt::formatter<protocol::GenerationStatusRequest>::format(
    [[maybe_unused]] const formattable& request, format_context& context) const
    -> decltype(context.out()) {
  return format_to(context.out(), "GenerationStatusRequest={{}}");
}

auto fmt::formatter<protocol::GenerationStatusReply>::format(
    const formattable& reply, format_context& context) const
    -> decltype(context.out()) {
  return format_to(context.out(),
                   "GenerationStatusReply={{ GenerationStatus={} }}",
                   reply.status);
}

auto fmt::formatter<protocol::StartGenerationReply::Result>::format(
    formattable result, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  switch (result) {
    case formattable::Started:
      return base_formatter::format("Started", context);
    case formattable::ErrorOccurred:
      return base_formatter::format("ErrorOccurred", context);
  }
  return base_formatter::format("undefined", context);
}

auto fmt::formatter<protocol::StartGenerationRequest>::format(
    [[maybe_unused]] const formattable& request, format_context& context) const
    -> decltype(context.out()) {
  return format_to(context.out(), "StartGenerationRequest={{}}");
}

auto fmt::formatter<protocol::StartGenerationReply>::format(
    const formattable& reply, format_context& context) const
    -> decltype(context.out()) {
  return format_to(
      context.out(), "StartGenerationReply={{ Result={} }}", reply.result);
}

auto fmt::formatter<protocol::StopGenerationReply::Result>::format(
    formattable result, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  switch (result) {
    case formattable::Stopped:
      return base_formatter::format("Stopped", context);
    case formattable::ErrorOccurred:
      return base_formatter::format("ErrorOccurred", context);
  }
  return base_formatter::format("undefined", context);
}

auto fmt::formatter<protocol::StopGenerationRequest>::format(
    [[maybe_unused]] const formattable& request, format_context& context) const
    -> decltype(context.out()) {
  return format_to(context.out(), "StopGenerationRequest={{}}");
}

auto fmt::formatter<protocol::StopGenerationReply>::format(
    const formattable& reply, format_context& context) const
    -> decltype(context.out()) {
  return format_to(
      context.out(), "StopGenerationReply={{ Result={} }}", reply.result);
}

auto fmt::formatter<protocol::HaltPhaseRequest>::format(
    formattable request, format_context& context) const
    -> decltype(context.out()) {
  return format_to(context.out(),
                   "HaltPhaseRequest={{ allow_cancels={} }}",
                   request.allow_cancels);
}

auto fmt::formatter<protocol::HaltPhaseReply::Result>::format(
    formattable result, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  switch (result) {
    case formattable::Halted:
      return base_formatter::format("Halted", context);
    case formattable::AlreadyHaltedByRequest:
      return base_formatter::format("AlreadyHaltedByRequest", context);
    case formattable::NoActivePhase:
      return base_formatter::format("NoActivePhase", context);
    case formattable::UnableToHalt:
      return base_formatter::format("UnableToHalt", context);
  }
  return base_formatter::format("undefined", context);
}

auto fmt::formatter<protocol::HaltPhaseReply>::format(
    formattable reply, format_context& context) const
    -> decltype(context.out()) {
  return format_to(
      context.out(), "HaltPhaseReply={{ Result={} }}", reply.result);
}

auto fmt::formatter<protocol::ResumePhaseRequest>::format(
    [[maybe_unused]] formattable request, format_context& context) const
    -> decltype(context.out()) {
  return format_to(context.out(), "ResumePhaseRequest={{}}");
}

auto fmt::formatter<protocol::ResumePhaseReply::Result>::format(
    formattable result, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  switch (result) {
    case formattable::Resumed:
      return base_formatter::format("Resumed", context);
    case formattable::NoRequestedHalt:
      return base_formatter::format("NoRequestedHalt", context);
  }
  return base_formatter::format("undefined", context);
}

auto fmt::formatter<protocol::ResumePhaseReply>::format(
    formattable reply, format_context& context) const
    -> decltype(context.out()) {
  return format_to(
      context.out(), "ResumePhaseReply={{ Result={} }}", reply.result);
}

auto fmt::formatter<protocol::StoreMarketStateRequest>::format(
    [[maybe_unused]] formattable request, format_context& context) const
    -> decltype(context.out()) {
  return format_to(context.out(), "StoreMarketStateRequest={{}}");
}

auto fmt::formatter<protocol::StoreMarketStateReply>::format(
    formattable reply, format_context& context) const
    -> decltype(context.out()) {
  return format_to(
      context.out(), "StoreMarketStateReply={{ Result={} }}", reply.result);
}

auto fmt::formatter<protocol::RecoverMarketStateRequest>::format(
    [[maybe_unused]] formattable reply, format_context& context) const
    -> decltype(context.out()) {
  return format_to(context.out(), "RecoverMarketStateRequest={{}}");
}

auto fmt::formatter<protocol::RecoverMarketStateReply>::format(
    formattable reply, format_context& context) const
    -> decltype(context.out()) {
  return format_to(context.out(),
                   "RecoverMarketStateReply={{ Result={}, ErrorMessage={} }}",
                   reply.result,
                   reply.error_message);
}