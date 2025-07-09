#ifndef SIMULATOR_INSTRUMENTS_IH_INSTRUMENTS_CONTAINER_HPP_
#define SIMULATOR_INSTRUMENTS_IH_INSTRUMENTS_CONTAINER_HPP_

#include <algorithm>
#include <vector>

#include "common/instrument.hpp"
#include "idgen/instrument_id.hpp"

namespace simulator::trading_system::instrument {

class Container {
 private:
  using Storage = std::vector<Instrument>;

 public:
  using value_type = typename Storage::value_type;
  using const_iterator = typename Storage::const_iterator;
  using iterator = const_iterator;

  [[nodiscard]] auto begin() const noexcept -> const_iterator {
    return storage_.begin();
  }

  [[nodiscard]] auto end() const noexcept -> const_iterator {
    return storage_.end();
  }

  [[nodiscard]] auto size() const noexcept -> std::size_t {
    return storage_.size();
  }

  // Searches for instrument by a given identifier
  // and returns an iterator to a found element.
  // An end iterator is returned when no instrument is found with an identifier.
  [[nodiscard]] auto find_by_identifier(InstrumentId identifier) const
      -> const_iterator {
    const auto pos = find_position(identifier);
    return pos != end() && (pos->identifier == identifier) ? pos : end();
  }

  // Inserts a new instrument into container and returns an interator to it.
  // Does not insert anything when other instrument with the same identifier
  // already exists in the container. An end iterator is returned in this case.
  auto emplace(const Instrument& instrument) -> const_iterator {
    const auto pos = find_position(instrument.identifier);
    if (pos != end() && pos->identifier == instrument.identifier) {
      return end();
    }
    return storage_.emplace(pos, instrument);
  }

 private:
  [[nodiscard]] consteval static auto make_comparator() {
    return [](const Instrument& stored, InstrumentId given) noexcept {
      return stored.identifier < given;
    };
  }

  [[nodiscard]] auto find_position(const InstrumentId& identifier) const
      -> const_iterator {
    return std::lower_bound(begin(), end(), identifier, make_comparator());
  }

  Storage storage_;
};

}  // namespace simulator::trading_system::instrument

#endif  // SIMULATOR_INSTRUMENTS_IH_INSTRUMENTS_CONTAINER_HPP_