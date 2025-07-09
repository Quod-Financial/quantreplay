#include "core/tools/string_checkers.hpp"

#include <algorithm>
#include <cctype>

namespace simulator::core {
namespace {

[[nodiscard]] auto is_base10_digit(unsigned char character) noexcept -> bool {
  return std::isdigit(character) != 0;
}

}  // namespace

auto represents_base10_unsigned_int(std::string_view string) noexcept -> bool {
  return !string.empty() &&
         std::all_of(std::begin(string), std::end(string), is_base10_digit);
}

}  // namespace simulator::core