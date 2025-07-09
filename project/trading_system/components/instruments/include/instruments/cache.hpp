#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_CACHE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_CACHE_HPP_

#include <memory>
#include <tl/expected.hpp>
#include <vector>

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "instruments/lookup_error.hpp"
#include "instruments/view.hpp"

namespace simulator::trading_system::instrument {

class Cache {
 public:
  struct Implementation;

  Cache(const Cache&) = delete;
  Cache(Cache&&) noexcept;
  ~Cache() noexcept;

  auto operator=(const Cache&) -> Cache& = delete;
  auto operator=(Cache&&) noexcept -> Cache&;

  static auto create() -> Cache;

  [[nodiscard]]
  auto find(const InstrumentDescriptor& descriptor) const
      -> tl::expected<View, LookupError>;

  [[nodiscard]]
  auto find(const Instrument& instrument) const
      -> tl::expected<View, LookupError>;

  [[nodiscard]]
  auto retrieve_instruments() const -> std::vector<Instrument>;

  auto load(const struct DatabaseSource& source) -> void;

  auto load(const struct MemorySource& source) -> void;

 private:
  explicit Cache(std::unique_ptr<Implementation> impl) noexcept;

  template <typename SourceType>
  auto load_instruments(const SourceType& source) -> void;

  [[nodiscard]]
  auto impl() const noexcept -> Implementation&;

  std::unique_ptr<Implementation> impl_;
};

}  // namespace simulator::trading_system::instrument

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_INSTRUMENTS_CACHE_HPP_