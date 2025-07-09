#ifndef SIMULATOR_FIX_ACCEPTOR_ACCEPTOR_HPP_
#define SIMULATOR_FIX_ACCEPTOR_ACCEPTOR_HPP_

#include <memory>

namespace simulator::fix {

struct Acceptor {
  struct Implementation;

  explicit Acceptor(std::unique_ptr<Implementation> impl) noexcept;
  Acceptor(const Acceptor&) = delete;
  Acceptor(Acceptor&&) noexcept;
  ~Acceptor() noexcept;

  auto operator=(const Acceptor&) -> Acceptor& = delete;
  auto operator=(Acceptor&&) noexcept -> Acceptor&;

  auto implementation() noexcept -> Implementation&;

 private:
  std::unique_ptr<Implementation> impl_;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_ACCEPTOR_ACCEPTOR_HPP_