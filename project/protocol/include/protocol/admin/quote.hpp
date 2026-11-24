#ifndef SIMULATOR_PROTOCOL_ADMIN_QUOTE_HPP_
#define SIMULATOR_PROTOCOL_ADMIN_QUOTE_HPP_

#include <fmt/format.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"

namespace simulator::protocol {

struct QuoteEntry {
  Price price;
  Quantity quantity;

  [[nodiscard]]
  auto operator==(const QuoteEntry&) const -> bool = default;
};

struct QuotePublicationRequest {
  InstrumentDescriptor instrument;
  std::vector<QuoteEntry> bids;
  std::vector<QuoteEntry> asks;
};

struct QuotePublicationReply {
  enum class Result : std::uint8_t {
    Published,
    ListingNotFound,
    AmbiguousListing,
    InvalidRequest
  };

  std::optional<Result> result;
  std::optional<QuoteId> quote_id;
  std::string error_message;
};

struct QuoteCancellationRequest {
  QuoteId quote_id;
};

struct QuoteCancellationReply {
  enum class Result : std::uint8_t { Cancelled, QuoteNotFound };

  std::optional<Result> result;
};

}  // namespace simulator::protocol

template <>
struct fmt::formatter<simulator::protocol::QuoteEntry> {
  using formattable = simulator::protocol::QuoteEntry;

  constexpr static auto parse(format_parse_context& ctx) { return ctx.begin(); }

  static auto format(const formattable& entry, format_context& context)
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::QuotePublicationRequest> {
  using formattable = simulator::protocol::QuotePublicationRequest;

  constexpr static auto parse(format_parse_context& ctx) { return ctx.begin(); }

  static auto format(const formattable& request, format_context& context)
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::QuotePublicationReply::Result>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::QuotePublicationReply::Result;

  auto format(formattable result, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::QuotePublicationReply> {
  using formattable = simulator::protocol::QuotePublicationReply;

  constexpr static auto parse(format_parse_context& ctx) { return ctx.begin(); }

  static auto format(const formattable& reply, format_context& context)
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::QuoteCancellationRequest> {
  using formattable = simulator::protocol::QuoteCancellationRequest;

  constexpr static auto parse(format_parse_context& ctx) { return ctx.begin(); }

  static auto format(const formattable& request, format_context& context)
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::QuoteCancellationReply::Result>
    : public formatter<std::string_view> {
  using formattable = simulator::protocol::QuoteCancellationReply::Result;

  auto format(formattable result, format_context& context) const
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::protocol::QuoteCancellationReply> {
  using formattable = simulator::protocol::QuoteCancellationReply;

  constexpr static auto parse(format_parse_context& ctx) { return ctx.begin(); }

  static auto format(const formattable& reply, format_context& context)
      -> decltype(context.out());
};

#endif  // SIMULATOR_PROTOCOL_ADMIN_QUOTE_HPP_
