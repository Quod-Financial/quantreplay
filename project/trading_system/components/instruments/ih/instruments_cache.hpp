#ifndef SIMULATOR_INSTRUMENTS_IH_INSTRUMENTS_CACHE_HPP_
#define SIMULATOR_INSTRUMENTS_IH_INSTRUMENTS_CACHE_HPP_

#include <tl/expected.hpp>
#include <vector>

#include "core/domain/instrument_descriptor.hpp"
#include "idgen/instrument_id.hpp"
#include "ih/instruments_container.hpp"
#include "instruments/cache.hpp"
#include "instruments/lookup_error.hpp"
#include "instruments/view.hpp"

namespace simulator::trading_system::instrument {

struct Cache::Implementation {
 public:
  Implementation();

  auto add_instrument(Instrument instrument) -> void;

  auto retrieve_instruments() const -> std::vector<Instrument>;

  auto find_instrument(const InstrumentDescriptor& descriptor) const
      -> tl::expected<View, LookupError>;

  auto find_instrument(const Instrument& instrument) const
      -> tl::expected<View, LookupError>;

  auto container() const -> const Container& { return container_; }

 private:
  [[nodiscard]] auto generate_new_id() -> InstrumentId;

  Container container_{};
  idgen::InstrumentIdContext id_generation_context_;
};

}  // namespace simulator::trading_system::instrument

#endif  // SIMULATOR_INSTRUMENTS_IH_INSTRUMENTS_CACHE_HPP_