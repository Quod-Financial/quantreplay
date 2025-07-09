#ifndef SIMULATOR_HTTP_IH_FORMATTERS_REDIRECT_HPP_
#define SIMULATOR_HTTP_IH_FORMATTERS_REDIRECT_HPP_

#include <fmt/format.h>

#include "ih/redirect/destination.hpp"
#include "ih/redirect/request.hpp"
#include "ih/redirect/result.hpp"

namespace fmt {

template <>
struct formatter<simulator::http::redirect::Destination> {
  using FormatableType = simulator::http::redirect::Destination;

  static constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormattingContext>
  auto format(const FormatableType& destination, FormattingContext& ctx) const {
    return format_to(ctx.out(),
                     "redirect::Destination: \"{}:{}\"",
                     destination.host(),
                     destination.port());
  }
};

template <>
struct formatter<simulator::http::redirect::Request> {
  using FormatableType = simulator::http::redirect::Request;

  static constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormattingContext>
  auto format(const FormatableType& request, FormattingContext& ctx) const {
    using Pistache::Http::methodString;

    return format_to(ctx.out(),
                     "redirect::Request: {{ Resource: '{}:{}', {} }}",
                     methodString(request.method()),
                     request.url(),
                     request.destination());
  }
};

template <>
struct formatter<simulator::http::redirect::Result> {
  using FormatableType = simulator::http::redirect::Result;

  static constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormattingContext>
  auto format(const FormatableType& result, FormattingContext& ctx) const {
    using Pistache::Http::codeString;

    return format_to(ctx.out(),
                     "redirect::Response: {{ "
                     "Status: \"{}\", Code: {}, Body: \"{}-byte providers\" "
                     "}}",
                     codeString(result.http_code()),
                     static_cast<int>(result.http_code()),
                     result.body_content().size());
  }
};

}  // namespace fmt

#endif  // SIMULATOR_HTTP_IH_FORMATTERS_REDIRECT_HPP_
