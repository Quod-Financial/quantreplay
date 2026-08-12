#include "ih/random/seed_hasher.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "data_layer/api/models/listing.hpp"

namespace simulator::generator::random {
namespace {

// FNV-1a 64-bit. Chosen because the simulator needs a hash that is:
//   * deterministic across compilers, libc++/libstdc++ versions, and host
//     architectures (std::hash is implementation-defined and varies between
//     toolchains, which would break "same seed -> same sequence" across
//     simulator deployments);
//   * non-cryptographic (cryptographic strength is unnecessary here — the
//     output is fed straight into std::seed_seq, not used as a secret);
// See:
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
constexpr std::uint64_t FnvOffsetBasis = 0xCBF29CE484222325ULL;
constexpr std::uint64_t FnvPrime = 0x00000100000001B3ULL;

constexpr std::uint8_t TagAbsent = 0x00;
constexpr std::uint8_t TagPresent = 0x01;

auto mix_byte(std::uint64_t state, std::uint8_t byte) -> std::uint64_t {
  state ^= static_cast<std::uint64_t>(byte);
  state *= FnvPrime;
  return state;
}

auto mix_bytes(std::uint64_t state, std::string_view bytes) -> std::uint64_t {
  for (const char byte : bytes) {
    state = mix_byte(state, static_cast<unsigned char>(byte));
  }
  return state;
}

auto mix_uint64(std::uint64_t state, std::uint64_t value) -> std::uint64_t {
  for (std::uint8_t shift = 0; shift < 64U; shift += 8U) {
    const auto byte = static_cast<std::uint8_t>((value >> shift) & 0xFFU);
    state = mix_byte(state, byte);
  }
  return state;
}

auto mix_field(std::uint64_t state, std::string_view value) -> std::uint64_t {
  state = mix_uint64(state, value.size());
  return mix_bytes(state, value);
}

auto mix_field(std::uint64_t state, const std::optional<std::string>& value)
    -> std::uint64_t {
  if (!value.has_value()) {
    return mix_byte(state, TagAbsent);
  }
  state = mix_byte(state, TagPresent);
  return mix_field(state, std::string_view{*value});
}

auto compute_listing_hash(const data_layer::Listing& listing) -> std::uint64_t {
  std::uint64_t state = FnvOffsetBasis;

  state = mix_field(state, std::string_view{listing.venue_id()});
  state = mix_field(state, listing.symbol());

  state = mix_field(state, listing.security_type());
  state = mix_field(state, listing.price_currency());
  state = mix_field(state, listing.fx_base_currency());
  state = mix_field(state, listing.instr_symbol());
  state = mix_field(state, listing.security_exchange());
  state = mix_field(state, listing.party_id());
  state = mix_field(state, listing.party_role());
  state = mix_field(state, listing.cusip_id());
  state = mix_field(state, listing.sedol_id());
  state = mix_field(state, listing.isin_id());
  state = mix_field(state, listing.ric_id());
  state = mix_field(state, listing.exchange_symbol_id());
  state = mix_field(state, listing.bloomberg_symbol_id());

  return state;
}

}  // namespace

ListingSeedHasher::ListingSeedHasher(const data_layer::Listing& listing)
    : listing_hash_{compute_listing_hash(listing)} {}

auto ListingSeedHasher::with_seed(std::string_view user_seed) const
    -> std::uint64_t {
  return mix_field(listing_hash_, user_seed);
}

}  // namespace simulator::generator::random
