#include <fmt/format.h>

#include <string_view>

#include "core/common/name.hpp"
#include "core/common/std_formatter.hpp"
#include "core/tools/format.hpp"
#include "protocol/admin/generator.hpp"
#include "protocol/admin/market_state.hpp"
#include "protocol/admin/quote.hpp"
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
    const formattable& request, format_context& context) const
    -> decltype(context.out()) {
  return format_to(
      context.out(), "StartGenerationRequest={{ seed={} }}", request.seed);
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
    case formattable::AuctionInProgress:
      return base_formatter::format("AuctionInProgress", context);
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
    case formattable::AuctionInProgress:
      return base_formatter::format("AuctionInProgress", context);
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

auto fmt::formatter<protocol::QuoteEntry>::format(const formattable& entry,
                                                  format_context& context)
    -> decltype(context.out()) {
  using simulator::core::name_of;
  return format_to(context.out(),
                   "QuoteEntry={{ {}={}, {}={} }}",
                   name_of(entry.price),
                   entry.price,
                   name_of(entry.quantity),
                   entry.quantity);
}

auto fmt::formatter<protocol::QuotePublicationRequest>::format(
    const formattable& request, format_context& context)
    -> decltype(context.out()) {
  using simulator::core::format_collection;
  return format_to(context.out(),
                   "QuotePublicationRequest={{ {}, Bids={}, Asks={} }}",
                   request.instrument,
                   format_collection(request.bids),
                   format_collection(request.asks));
}

auto fmt::formatter<protocol::QuotePublicationReply::Result>::format(
    formattable result, format_context& context) const
    -> decltype(context.out()) {
  using base_formatter = formatter<std::string_view>;
  switch (result) {
    case formattable::Published:
      return base_formatter::format("Published", context);
    case formattable::ListingNotFound:
      return base_formatter::format("ListingNotFound", context);
    case formattable::AmbiguousListing:
      return base_formatter::format("AmbiguousListing", context);
    case formattable::InvalidRequest:
      return base_formatter::format("InvalidRequest", context);
  }
  return base_formatter::format("undefined", context);
}

auto fmt::formatter<protocol::QuotePublicationReply>::format(
    const formattable& reply, format_context& context)
    -> decltype(context.out()) {
  return format_to(
      context.out(),
      "QuotePublicationReply={{ Result={}, QuoteId={}, ErrorMessage={} }}",
      reply.result,
      reply.quote_id,
      reply.error_message);
}
