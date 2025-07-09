#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_MATCHER_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_MATCHER_HPP_

#include <memory>
#include <tl/expected.hpp>

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "instruments/lookup_error.hpp"

namespace simulator::trading_system::instrument {

class Matcher {
 public:
  class Implementation;

  Matcher() = delete;
  Matcher(const Matcher&) = delete;
  Matcher(Matcher&&) noexcept;
  ~Matcher() noexcept;

  auto operator=(const Matcher&) -> Matcher& = delete;
  auto operator=(Matcher&&) noexcept -> Matcher& = default;

  static auto create(const Instrument& instrument) -> Matcher;

  [[nodiscard]]
  auto operator()(const InstrumentDescriptor& descriptor) const
      -> tl::expected<void, LookupError>;

 private:
  explicit Matcher(std::unique_ptr<Implementation> impl) noexcept;

  [[nodiscard]]
  auto impl() const noexcept -> Implementation&;

  std::unique_ptr<Implementation> impl_;
};

}  // namespace simulator::trading_system::instrument

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_MATCHER_HPP_
