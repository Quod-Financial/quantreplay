#ifndef SIMULATOR_FIX_COMMON_MAPPING_CHECKS_HPP_
#define SIMULATOR_FIX_COMMON_MAPPING_CHECKS_HPP_

#include <quickfix/Field.h>

namespace simulator::fix {

template <int... FixFieldTags>
auto contains(const FIX::FieldMap& fix_source) -> bool {
  static_assert(sizeof...(FixFieldTags) > 0,
                "No FIX field tags are provided to check their presence");
  return (fix_source.isSetField(FixFieldTags) && ...);
}

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_MAPPING_CHECKS_HPP_