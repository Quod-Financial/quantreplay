#include "ih/utils/host_resolver.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <climits>
#include <string>

#include "log/logging.hpp"

namespace simulator::http {
namespace {

[[nodiscard]]
auto resolve_local_host_name() -> std::string {
  std::array<char, HOST_NAME_MAX + 1> buffer{};
  if (::gethostname(buffer.data(), buffer.size() - 1) != 0) {
    log::warn("failed to resolve local host name for fix session reporting");
    return {};
  }
  return std::string{buffer.data()};
}

}  // namespace

auto resolve_host_ip() -> std::string {
  const auto host_name = resolve_local_host_name();
  if (host_name.empty()) {
    return {};
  }

  addrinfo hints{};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  addrinfo* resolved = nullptr;
  if (const int code =
          ::getaddrinfo(host_name.c_str(), nullptr, &hints, &resolved);
      code != 0 || resolved == nullptr) {
    log::warn("failed to resolve local host ip for fix session reporting: {}",
              ::gai_strerror(code));
    return {};
  }

  std::array<char, INET_ADDRSTRLEN> host_ip{};
  const auto& address =
      *reinterpret_cast<const sockaddr_in*>(resolved->ai_addr);  // NOLINT
  const auto* converted =
      ::inet_ntop(AF_INET, &address.sin_addr, host_ip.data(), host_ip.size());
  ::freeaddrinfo(resolved);

  if (converted == nullptr) {
    log::warn("failed to convert local host ip for fix session reporting");
    return {};
  }
  return std::string{host_ip.data()};
}

}  // namespace simulator::http
