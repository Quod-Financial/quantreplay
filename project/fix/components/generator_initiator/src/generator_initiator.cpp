#include "fix/generator_initiator/generator_initiator.hpp"

#include <cstdlib>
#include <memory>
#include <utility>

#include "ih/implementation.hpp"
#include "log/logging.hpp"

namespace simulator::fix {

GeneratorInitiator::GeneratorInitiator(
    std::unique_ptr<Implementation> impl) noexcept
    : impl_{std::move(impl)} {}

GeneratorInitiator::GeneratorInitiator(GeneratorInitiator&&) noexcept = default;

GeneratorInitiator::~GeneratorInitiator() noexcept = default;

auto GeneratorInitiator::operator=(GeneratorInitiator&&) noexcept
    -> GeneratorInitiator& = default;

auto GeneratorInitiator::implementation() noexcept -> Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  log::err(
      "fix initiator implementation has not been allocated/initialized, "
      "this may indicate a critical bug in the component, can not continue "
      "execution, aborting...");

  std::abort();
}

}  // namespace simulator::fix
