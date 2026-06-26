#ifndef SIMULATOR_HTTP_IH_UTILS_HOST_RESOLVER_HPP_
#define SIMULATOR_HTTP_IH_UTILS_HOST_RESOLVER_HPP_

#include <string>

namespace simulator::http {

[[nodiscard]]
auto resolve_host_ip() -> std::string;

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_UTILS_HOST_RESOLVER_HPP_
