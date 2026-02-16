#ifndef SIMULATOR_HTTP_TESTS_TEST_UTILS_PROCESSORS_HPP_
#define SIMULATOR_HTTP_TESTS_TEST_UTILS_PROCESSORS_HPP_

#include <pistache/http.h>
#include <pistache/peer.h>

#include <ranges>

namespace simulator::http::test::util {

inline auto has_content_length(
    const std::vector<std::pair<std::string, std::string>>& headers) -> bool {
  constexpr std::string_view content_length{"content-length"};

  constexpr auto lower_view = std::views::transform(
      [](char c) -> char { return static_cast<char>(std::tolower(c)); });

  return std::ranges::any_of(headers, [&](const auto& header) {
    const auto& name = header.first;
    return name.size() == content_length.size() &&
           std::ranges::equal(name | lower_view, content_length);
  });
}

inline auto make_request(
    const std::string& method,
    const std::string& target,
    const std::vector<std::pair<std::string, std::string>>& headers =
        std::vector<std::pair<std::string, std::string>>{},
    const std::string& body = "") -> Pistache::Http::Request {
  std::ostringstream raw_request;
  raw_request << method << ' ' << target << " HTTP/1.1\r\n";

  for (const auto& header : headers) {
    raw_request << header.first << ": " << header.second << "\r\n";
  }

  if (!body.empty() && !has_content_length(headers)) {
    raw_request << "Content-Length: " << body.size() << "\r\n";
  }

  raw_request << "\r\n";
  raw_request << body;

  const auto& raw = raw_request.str();

  Pistache::Http::RequestParser parser{Pistache::Const::DefaultMaxRequestSize};
  if (!parser.feed(raw.data(), raw.size())) {
    throw std::runtime_error(
        "Unable to feed the HTTP parser with the request data");
  }

  const auto state = parser.parse();
  if (state != Pistache::Http::Private::State::Done) {
    throw std::runtime_error("HTTP parser did not finish building the request");
  }
  return parser.request;
}

/**
 * A bundle containing a ResponseWriter and the backing transport objects, which
 * the ResponseWriter uses but does not own.
 */
struct DummyResponseWriter {
  Pistache::Http::ResponseWriter writer;
  std::shared_ptr<Pistache::Tcp::Transport> transport;
  std::shared_ptr<Pistache::Tcp::Peer> peer;
};

class NullTcpHandler : public Pistache::Tcp::Handler {
 public:
  HTTP_PROTOTYPE(NullTcpHandler)

  auto onInput(const char* /*buffer*/,
               size_t /*len*/,
               const std::shared_ptr<Pistache::Tcp::Peer>& /*peer*/)
      -> void override {}
};

inline auto make_response_writer(Pistache::Http::Handler& handler)
    -> DummyResponseWriter {
  constexpr int empty_fd = -1;

  auto transport_handler = std::make_shared<NullTcpHandler>();
  auto transport =
      std::make_shared<Pistache::Tcp::Transport>(transport_handler);

  auto peer = Pistache::Tcp::Peer::Create(
      empty_fd, Pistache::Address{"127.0.0.1", Pistache::Port{0}});

  Pistache::Http::ResponseWriter writer{
      Pistache::Http::Version::Http11, transport.get(), &handler, peer};

  return DummyResponseWriter{
      std::move(writer), std::move(transport), std::move(peer)};
}

}  // namespace simulator::http::test::util

#endif  // SIMULATOR_HTTP_TESTS_TEST_UTILS_PROCESSORS_HPP_
