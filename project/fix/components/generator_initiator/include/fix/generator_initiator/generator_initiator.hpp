#ifndef SIMULATOR_FIX_GENERATOR_INITIATOR_GENERATOR_INITIATOR_HPP_
#define SIMULATOR_FIX_GENERATOR_INITIATOR_GENERATOR_INITIATOR_HPP_

#include <memory>

namespace simulator::fix {

struct GeneratorInitiator {
  struct Implementation;

  explicit GeneratorInitiator(std::unique_ptr<Implementation> impl) noexcept;
  GeneratorInitiator(const GeneratorInitiator&) = delete;
  GeneratorInitiator(GeneratorInitiator&&) noexcept;
  ~GeneratorInitiator() noexcept;

  auto operator=(const GeneratorInitiator&) -> GeneratorInitiator& = delete;
  auto operator=(GeneratorInitiator&&) noexcept -> GeneratorInitiator&;

  auto implementation() noexcept -> Implementation&;

 private:
  std::unique_ptr<Implementation> impl_;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_GENERATOR_INITIATOR_GENERATOR_INITIATOR_HPP_
