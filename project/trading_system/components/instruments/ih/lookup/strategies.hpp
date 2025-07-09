#ifndef SIMULATOR_INSTRUMENTS_IH_LOOKUP_STRATEGIES_HPP_
#define SIMULATOR_INSTRUMENTS_IH_LOOKUP_STRATEGIES_HPP_

#include <tl/expected.hpp>

#include "common/instrument.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "ih/lookup/match_rate.hpp"
#include "instruments/lookup_error.hpp"

namespace simulator::trading_system::instrument::lookup {

class SymbolLookup {
 public:
  SymbolLookup() = delete;

  [[nodiscard]]
  auto
  operator()(const Instrument& instrument) const -> MatchRate;

  [[nodiscard]]
  static auto create(const InstrumentDescriptor& descriptor)
      -> tl::expected<SymbolLookup, LookupError>;

 private:
  explicit SymbolLookup(const InstrumentDescriptor& descriptor);

  const InstrumentDescriptor* descriptor_;
};

class SedolIdLookup {
 public:
  SedolIdLookup() = delete;

  [[nodiscard]]
  auto
  operator()(const Instrument& instrument) const -> MatchRate;

  [[nodiscard]]
  static auto create(const InstrumentDescriptor& descriptor)
      -> tl::expected<SedolIdLookup, LookupError>;

 private:
  explicit SedolIdLookup(const InstrumentDescriptor& descriptor);

  const InstrumentDescriptor* descriptor_;
};

class CusipIdLookup {
 public:
  CusipIdLookup() = delete;

  [[nodiscard]]
  auto
  operator()(const Instrument& instrument) const -> MatchRate;

  [[nodiscard]]
  static auto create(const InstrumentDescriptor& descriptor)
      -> tl::expected<CusipIdLookup, LookupError>;

 private:
  explicit CusipIdLookup(const InstrumentDescriptor& descriptor);

  const InstrumentDescriptor* descriptor_;
};

class IsinIdLookup {
 public:
  IsinIdLookup() = delete;

  [[nodiscard]]
  auto
  operator()(const Instrument& instrument) const -> MatchRate;

  [[nodiscard]]
  static auto create(const InstrumentDescriptor& descriptor)
      -> tl::expected<IsinIdLookup, LookupError>;

 private:
  explicit IsinIdLookup(const InstrumentDescriptor& descriptor);

  const InstrumentDescriptor* descriptor_;
};

class RicIdLookup {
 public:
  RicIdLookup() = delete;

  [[nodiscard]]
  auto
  operator()(const Instrument& instrument) const -> MatchRate;

  [[nodiscard]]
  static auto create(const InstrumentDescriptor& descriptor)
      -> tl::expected<RicIdLookup, LookupError>;

 private:
  explicit RicIdLookup(const InstrumentDescriptor& descriptor);

  const InstrumentDescriptor* descriptor_;
};

class ExchangeIdLookup {
 public:
  ExchangeIdLookup() = delete;

  [[nodiscard]]
  auto
  operator()(const Instrument& instrument) const -> MatchRate;

  [[nodiscard]]
  static auto create(const InstrumentDescriptor& descriptor)
      -> tl::expected<ExchangeIdLookup, LookupError>;

 private:
  explicit ExchangeIdLookup(const InstrumentDescriptor& descriptor);

  const InstrumentDescriptor* descriptor_;
};

class BloombergIdLookup {
 public:
  BloombergIdLookup() = delete;

  [[nodiscard]]
  auto
  operator()(const Instrument& instrument) const -> MatchRate;

  [[nodiscard]]
  static auto create(const InstrumentDescriptor& descriptor)
      -> tl::expected<BloombergIdLookup, LookupError>;

 private:
  explicit BloombergIdLookup(const InstrumentDescriptor& descriptor);

  const InstrumentDescriptor* descriptor_;
};

}  // namespace simulator::trading_system::instrument::lookup

#endif  // SIMULATOR_INSTRUMENTS_IH_LOOKUP_STRATEGIES_HPP_
