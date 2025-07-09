#ifndef SIMULATOR_MIDDLEWARE_CHANNELS_DETAIL_RECEIVER_HPP_
#define SIMULATOR_MIDDLEWARE_CHANNELS_DETAIL_RECEIVER_HPP_

namespace simulator::middleware::detail {

struct Receiver {
  Receiver() = default;
  Receiver(const Receiver&) = default;
  Receiver(Receiver&&) = default;
  virtual ~Receiver() = default;

  auto operator=(const Receiver&) -> Receiver& = default;
  auto operator=(Receiver&&) -> Receiver& = default;
};

}  // namespace simulator::middleware::detail

#endif  // SIMULATOR_MIDDLEWARE_CHANNELS_DETAIL_RECEIVER_HPP_