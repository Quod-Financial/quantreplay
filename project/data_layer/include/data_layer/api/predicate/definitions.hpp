#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_DEFINITIONS_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_DEFINITIONS_HPP_

#include <cmath>
#include <cstdint>
#include <string>
#include <tuple>

namespace simulator::data_layer::predicate {

enum class BasicOperation { Eq, Neq, Less, Greater, LessEq, GreaterEq };

enum class CompositeOperation { And, Or };

// A tuple of standard types, that can be stored by any predicate expression.
using StandardTypes =
    std::tuple<bool, std::int64_t, std::uint64_t, std::double_t, std::string>;

// A pre-declaration of a generic predicate expression.
template <typename Model>
class Expression;

}  // namespace simulator::data_layer::predicate

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_DEFINITIONS_HPP_
