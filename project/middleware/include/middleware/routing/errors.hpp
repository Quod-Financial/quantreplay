#ifndef SIMULATOR_MIDDLEWARE_ROUTING_ERRORS_HPP_
#define SIMULATOR_MIDDLEWARE_ROUTING_ERRORS_HPP_

#include <exception>

namespace simulator::middleware {

struct ChannelUnboundError : public std::exception {};

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_ROUTING_ERRORS_HPP_