#ifndef SIMULATOR_HTTP_IH_FORMATTERS_REQUEST_FORMATTER_HPP_
#define SIMULATOR_HTTP_IH_FORMATTERS_REQUEST_FORMATTER_HPP_

#include <fmt/format.h>
#include <pistache/http.h>

#include <string>

namespace simulator::http {

class RequestFormatter {
 public:
  auto method() const -> std::string {
    return Pistache::Http::methodString(request_.get().method());
  }

  auto resource() const -> const std::string& {
    return request_.get().resource();
  }

  auto address() const -> std::string {
    const Pistache::Address& address = request_.get().address();
    return fmt::format(
        "{}:{}", address.host(), static_cast<std::uint16_t>(address.port()));
  }

  auto has_body() const -> bool { return !request_.get().body().empty(); }

  auto body() const -> std::string { return request_.get().body(); }

  static auto format(const Pistache::Http::Request& request)
      -> RequestFormatter {
    return RequestFormatter{request};
  }

 private:
  explicit RequestFormatter(const Pistache::Http::Request& request)
      : request_{request} {}

  std::reference_wrapper<const Pistache::Http::Request> request_;
};

}  // namespace simulator::http

namespace fmt {

template <>
class formatter<simulator::http::RequestFormatter> {
  using RequestFormatter = simulator::http::RequestFormatter;

 public:
  static auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename Context>
  auto format(const RequestFormatter& formatter, Context& ctx) const
      -> decltype(ctx.out()) {
    format_to(ctx.out(),
              "RestRequest={{ Method={} URL=\"{}\" Source={} ",
              formatter.method(),
              formatter.resource(),
              formatter.address());
    if (formatter.has_body()) {
      format_to(ctx.out(), "Body=\"{}\" ", formatter.body());
    }
    return format_to(ctx.out(), "}}");
  }
};

}  // namespace fmt

#endif  // SIMULATOR_HTTP_IH_FORMATTERS_REQUEST_FORMATTER_HPP_
